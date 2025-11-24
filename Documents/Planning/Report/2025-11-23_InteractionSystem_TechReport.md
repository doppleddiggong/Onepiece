# UInteractionSystem & UInteractableComponent 기술 리포트

## 문서 정보
- **작성일**: 2025-11-23
- **대상 시스템**: Onepiece 프로젝트 Interaction System
- **관련 파일**:
  - `Source/Onepiece/Character/Public/UInteractionSystem.h`
  - `Source/Onepiece/Character/Private/UInteractionSystem.cpp`
  - `Source/Onepiece/Interactable/Public/InteractableComponent.h`
  - `Source/Onepiece/Interactable/Private/InteractableComponent.cpp`

---

## 목차
1. [시스템 개요](#시스템-개요)
2. [아키텍처 설계](#아키텍처-설계)
3. [핵심 컴포넌트 상세](#핵심-컴포넌트-상세)
4. [동작 메커니즘](#동작-메커니즘)
5. [Interactable 객체 구현](#interactable-객체-구현)
6. [확장 가이드](#확장-가이드)
7. [Best Practices](#best-practices)

---

## 시스템 개요

### 목적
플레이어와 환경 객체 간의 상호작용을 제공하는 범용 시스템으로, **Overlap 감지 + LineTrace 타겟팅** 방식을 사용하여 정밀한 상호작용을 구현합니다.

### 핵심 특징
- **이중 감지 시스템**: Overlap으로 근접 감지 → LineTrace로 정확한 타겟 확정
- **타입별 상호작용**: PickUp, Button 등 다양한 상호작용 타입 지원
- **확장 가능한 설계**: 새로운 상호작용 타입 추가 용이
- **멀티플레이어 지원**: Replication 기반 네트워크 동기화

### 시스템 구성 요소
1. **UInteractionSystem**: 플레이어에 부착되어 상호작용을 감지하고 처리
2. **UInteractableComponent**: 상호작용 가능한 객체에 부착되는 컴포넌트
3. **Interactable Actors**: luggage, PedestalSwitch, WeightSwitch, Door 등 구체적인 상호작용 객체

---

## 아키텍처 설계

### 시스템 다이어그램

```
┌─────────────────────────────────────────────────────────────┐
│                     Player Actor                             │
│  ┌────────────────────────────────────────────────────┐     │
│  │         UInteractionSystem                          │     │
│  │  - CurrentTarget                                    │     │
│  │  - HoldingInteractable                             │     │
│  │  - NearbyInteractables[]                           │     │
│  │                                                      │     │
│  │  Methods:                                           │     │
│  │  + TryInteract()                                    │     │
│  │  + TryPickUp()                                      │     │
│  │  + TryDrop()                                        │     │
│  │  + RegisterInteractable()                          │     │
│  │  + UnregisterInteractable()                        │     │
│  │  - DetectInteractableTarget()                      │     │
│  │  - PerformCenterLineTrace()                        │     │
│  └────────────────────────────────────────────────────┘     │
└─────────────────────────────────────────────────────────────┘
                        ↕ (Overlap Events)
┌─────────────────────────────────────────────────────────────┐
│                 Interactable Objects                         │
│  ┌────────────────────────────────────────────────────┐     │
│  │      UInteractableComponent                        │     │
│  │  - InteractionType (PickUp/Button)                 │     │
│  │  - DetectionRange (BoxComponent)                   │     │
│  │  - bCanInteract                                    │     │
│  │  - HoldingOwner                                    │     │
│  │                                                      │     │
│  │  Methods:                                           │     │
│  │  + PickUp() / Drop()                               │     │
│  │  + TriggerInteraction()                            │     │
│  │  + ShowDebugInfo()                                 │     │
│  │  - OnDetectionBeginOverlap()                       │     │
│  │  - OnDetectionEndOverlap()                         │     │
│  │                                                      │     │
│  │  Delegate:                                          │     │
│  │  • OnInteractionTriggered                          │     │
│  └────────────────────────────────────────────────────┘     │
│                                                              │
│  ┌─────────────┐ ┌──────────────┐ ┌──────────────┐         │
│  │  luggage    │ │PedestalSwitch│ │WeightSwitch  │  ...    │
│  └─────────────┘ └──────────────┘ └──────────────┘         │
└─────────────────────────────────────────────────────────────┘
```

### 데이터 흐름

```
1. 근접 감지 (Overlap)
   Interactable Object → DetectionRange Overlap → Player
   → RegisterInteractable() → NearbyInteractables[] 추가

2. 타겟팅 (LineTrace - 매 프레임)
   UInteractionSystem::TickComponent()
   → DetectInteractableTarget()
   → PerformCenterLineTrace() (화면 중앙)
   → NearbyInteractables 내에서 Hit된 객체 확정
   → CurrentTarget 설정

3. 상호작용 실행
   Player Input → TryInteract()
   → InteractionType 확인
      - PickUp: TryPickUp() → PickUp() → Attach to HoldPosition
      - Button: TriggerInteraction() → OnInteractionTriggered 델리게이트

4. 범위 이탈
   Player leaves DetectionRange
   → OnDetectionEndOverlap()
   → UnregisterInteractable()
   → NearbyInteractables[]에서 제거
```

---

## 핵심 컴포넌트 상세

### UInteractionSystem

**위치**: `Source/Onepiece/Character/`  
**부착 대상**: APlayerActor

#### 주요 속성

```cpp
// 설정
UPROPERTY(EditAnywhere, BlueprintReadWrite)
float InteractionDistance = 1200.0f;  // LineTrace 최대 거리

UPROPERTY(EditAnywhere, BlueprintReadWrite)
bool bShowDebugInfo = true;  // 디버그 표시 여부

// 상태
UPROPERTY(BlueprintReadOnly)
TObjectPtr<UInteractableComponent> CurrentTarget;  // 현재 타겟팅 중인 객체

UPROPERTY(BlueprintReadOnly)
TObjectPtr<UInteractableComponent> HoldingInteractable;  // 들고 있는 객체

// 내부 데이터
UPROPERTY()
TArray<TObjectPtr<UInteractableComponent>> NearbyInteractables;  // Overlap된 객체들
```

#### 핵심 메서드

##### 1. DetectInteractableTarget()
```cpp
UInteractableComponent* UInteractionSystem::DetectInteractableTarget()
```
- **목적**: 화면 중앙 LineTrace로 타겟 감지
- **로직**:
  1. NearbyInteractables가 비어있으면 nullptr 반환
  2. 화면 중앙에서 LineTrace 실행
  3. Hit된 Actor에서 InteractableComponent 검색
  4. NearbyInteractables에 포함된 객체만 반환 (범위 검증)

##### 2. PerformCenterLineTrace()
```cpp
bool UInteractionSystem::PerformCenterLineTrace(FHitResult& OutHit)
```
- **목적**: 화면 중앙에서 월드 공간으로 Ray 발사
- **구현 세부사항**:
  ```cpp
  // 뷰포트 크기 가져오기
  int32 ViewportSizeX, ViewportSizeY;
  PC->GetViewportSize(ViewportSizeX, ViewportSizeY);
  
  // 화면 중앙 픽셀 좌표
  float ScreenX = ViewportSizeX * 0.5f;
  float ScreenY = ViewportSizeY * 0.5f;
  
  // 화면 → 월드 변환
  FVector WorldLocation, WorldDirection;
  PC->DeprojectScreenPositionToWorld(ScreenX, ScreenY, WorldLocation, WorldDirection);
  
  // Ray Trace
  FVector TraceStart = WorldLocation;
  FVector TraceEnd = TraceStart + (WorldDirection * InteractionDistance);
  
  return GetWorld()->LineTraceSingleByChannel(OutHit, TraceStart, TraceEnd, ECC_Visibility);
  ```

##### 3. TryInteract()
```cpp
void UInteractionSystem::TryInteract()
```
- **목적**: 상호작용 타입별 자동 처리
- **로직**:
  ```cpp
  if (!CurrentTarget || !CurrentTarget->bCanInteract) return;
  
  switch (CurrentTarget->InteractionType)
  {
      case EInteractionType::PickUp:
          TryPickUp();
          break;
      
      case EInteractionType::Button:
          CurrentTarget->TriggerInteraction(OwnerPlayer);
          break;
  }
  ```

##### 4. TryPickUp()
```cpp
void UInteractionSystem::TryPickUp()
```
- **중요한 구현 디테일**:
  ```cpp
  // PickUp() 호출 **전**에 HoldingInteractable에 저장
  // 이유: PickUp() 과정에서 DetachFromActor로 인해
  //       Overlap이 해제되고 CurrentTarget이 nullptr이 될 수 있음
  HoldingInteractable = CurrentTarget;
  
  HoldingInteractable->HoldingOwner = OwnerPlayer;
  HoldingInteractable->PickUp();
  ```

##### 5. RegisterInteractable() / UnregisterInteractable()
```cpp
void UInteractionSystem::RegisterInteractable(UInteractableComponent* Interactable)
void UInteractionSystem::UnregisterInteractable(UInteractableComponent* Interactable)
```
- **목적**: Overlap 이벤트로 호출되어 NearbyInteractables 배열 관리
- **안전성**: 유효성 검사 + 중복 방지

---

### UInteractableComponent

**위치**: `Source/Onepiece/Interactable/`  
**부착 대상**: 상호작용 가능한 모든 Actor

#### 상호작용 타입

```cpp
UENUM(BlueprintType)
enum class EInteractionType : uint8
{
    None,
    PickUp,    // 집어올리기 (luggage)
    Button,    // 버튼 누르기 (PedestalSwitch)
};
```

#### 주요 속성

```cpp
// 상호작용 설정
UPROPERTY(EditAnywhere, BlueprintReadWrite)
EInteractionType InteractionType = EInteractionType::PickUp;

UPROPERTY(EditAnywhere, BlueprintReadWrite)
FString InteractionPrompt = TEXT("Press E to Interact");

UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
TObjectPtr<UBoxComponent> DetectionRange;  // 자동 생성됨

UPROPERTY(EditAnywhere, BlueprintReadWrite)
float DetectionDistance = 150.0f;  // 감지 범위 (cm)

// 상태
UPROPERTY(BlueprintReadOnly)
bool bCanInteract = true;

UPROPERTY(ReplicatedUsing=OnRep_HoldingOwner)
AActor* HoldingOwner;  // 현재 들고 있는 플레이어

UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
bool bIsPickedUp = false;

// 델리게이트
UPROPERTY(BlueprintAssignable)
FOnInteractionTriggered OnInteractionTriggered;
```

#### 핵심 메서드

##### 1. BeginPlay() - DetectionRange 자동 생성
```cpp
void UInteractableComponent::BeginPlay()
{
    // Owner Actor에 BoxComponent 생성
    DetectionRange = NewObject<UBoxComponent>(Owner, ...);
    DetectionRange->RegisterComponent();
    DetectionRange->AttachToComponent(Owner->GetRootComponent(), ...);
    
    // DetectionDistance 기반으로 크기 설정
    FVector BoxExtent(DetectionDistance, DetectionDistance, DetectionDistance);
    DetectionRange->SetBoxExtent(BoxExtent);
    
    // Overlap 이벤트만 감지
    DetectionRange->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    DetectionRange->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    
    // 콜백 바인딩
    DetectionRange->OnComponentBeginOverlap.AddDynamic(this, &UInteractableComponent::OnDetectionBeginOverlap);
    DetectionRange->OnComponentEndOverlap.AddDynamic(this, &UInteractableComponent::OnDetectionEndOverlap);
}
```

##### 2. OnDetectionBeginOverlap()
```cpp
void UInteractableComponent::OnDetectionBeginOverlap(...)
{
    ACharacter* Character = Cast<ACharacter>(OtherActor);
    if (Character && Character->IsPlayerControlled())
    {
        // InteractionSystem 찾기 및 등록
        UInteractionSystem* InteractionSystem = Character->FindComponentByClass<UInteractionSystem>();
        if (InteractionSystem)
        {
            InteractionSystem->RegisterInteractable(this);
        }
    }
}
```

##### 3. PickUp() / Drop() - 네트워크 동기화
```cpp
void UInteractableComponent::PickUp()
{
    GetOwner()->SetOwner(HoldingOwner);
    Server_PickUp();  // RPC 호출
}

void UInteractableComponent::Server_PickUp_Implementation()
{
    if (bIsPickedUp) return;
    
    UPrimitiveComponent* PrimComp = GetOwnerPrimitiveComponent();
    
    // 원래 상태 저장
    bOriginalSimulatePhysics = PrimComp->IsSimulatingPhysics();
    OriginalCollisionType = PrimComp->GetCollisionEnabled();
    
    // 물리 비활성화
    PrimComp->SetSimulatePhysics(false);
    
    // 서버에서 Attach
    if (HoldingOwner->HasAuthority())
    {
        OnRep_HoldingOwner();
    }
    
    bIsPickedUp = true;
}

void UInteractableComponent::OnRep_HoldingOwner()
{
    if (HoldingOwner)
    {
        APlayerActor* MyPlayer = Cast<APlayerActor>(HoldingOwner);
        GetOwner()->AttachToComponent(MyPlayer->HoldPosition, ...);
    }
    else
    {
        GetOwner()->DetachFromActor(...);
    }
}
```

##### 4. TriggerInteraction() - 델리게이트 기반
```cpp
void UInteractableComponent::TriggerInteraction(AActor* Interactor)
{
    if (!bCanInteract || !Interactor) return;
    
    // 델리게이트 브로드캐스트
    OnInteractionTriggered.Broadcast(Interactor);
}
```

---

## 동작 메커니즘

### 전체 흐름도

```
[1단계: 초기화]
Interactable Actor Spawn
  └─> UInteractableComponent::BeginPlay()
       └─> DetectionRange (BoxComponent) 자동 생성
            └─> Overlap 콜백 바인딩

[2단계: 근접 감지]
Player enters DetectionRange
  └─> OnDetectionBeginOverlap() 호출
       └─> UInteractionSystem::RegisterInteractable()
            └─> NearbyInteractables[] 배열에 추가

[3단계: 타겟팅 (매 프레임)]
UInteractionSystem::TickComponent()
  └─> DetectInteractableTarget()
       ├─> NearbyInteractables 비어있음? → nullptr 반환
       └─> PerformCenterLineTrace()
            ├─> 화면 중앙에서 Ray 발사
            ├─> Hit된 Actor에서 InteractableComponent 찾기
            └─> NearbyInteractables에 포함됨? → CurrentTarget 설정

[4단계: 상호작용 실행]
Player Input (E 키)
  └─> UInteractionSystem::TryInteract()
       └─> CurrentTarget->InteractionType 확인
            ├─> PickUp: TryPickUp()
            │    └─> HoldingInteractable = CurrentTarget
            │         └─> PickUp() → Server_PickUp()
            │              └─> Attach to HoldPosition
            └─> Button: TriggerInteraction()
                 └─> OnInteractionTriggered.Broadcast()

[5단계: 범위 이탈]
Player leaves DetectionRange
  └─> OnDetectionEndOverlap()
       └─> UInteractionSystem::UnregisterInteractable()
            ├─> NearbyInteractables[]에서 제거
            └─> CurrentTarget == this? → CurrentTarget = nullptr
```

### 주요 시나리오별 상세 동작

#### 시나리오 1: 물건 들기 (PickUp)

```
[준비 단계]
1. Player가 luggage 근처로 접근
2. luggage의 DetectionRange Overlap 발생
3. luggage의 InteractableComponent가 UInteractionSystem에 등록됨
   → NearbyInteractables[luggage]

[타겟팅]
4. 매 프레임 화면 중앙에서 LineTrace 실행
5. luggage가 Hit되면 CurrentTarget = luggage

[상호작용]
6. Player가 E 키 입력
7. TryInteract() → TryPickUp() 호출
8. HoldingInteractable = luggage (중요: PickUp 호출 전)
9. luggage->PickUp() 호출
   a. GetOwner()->SetOwner(Player)
   b. Server_PickUp() RPC 호출
   
[서버 실행]
10. Server_PickUp_Implementation()
    a. 물리 상태 저장 (bOriginalSimulatePhysics, OriginalCollisionType)
    b. SetSimulatePhysics(false)
    c. OnRep_HoldingOwner() → AttachToComponent(HoldPosition)
    d. bIsPickedUp = true

[결과]
11. luggage가 Player의 HoldPosition에 부착됨
12. DetachFromActor로 인해 Overlap 해제 → UnregisterInteractable 호출됨
    (하지만 HoldingInteractable에는 여전히 저장되어 있음)
```

#### 시나리오 2: 버튼 누르기 (Button)

```
[준비 단계]
1. Player가 PedestalSwitch 근처로 접근
2. DetectionRange Overlap → 등록

[타겟팅]
3. 화면 중앙 LineTrace → CurrentTarget = PedestalSwitch

[상호작용]
4. E 키 입력 → TryInteract()
5. InteractionType이 Button이므로 TriggerInteraction() 호출
6. OnInteractionTriggered.Broadcast(Player)

[델리게이트 처리]
7. PedestalSwitch::OnInteractionTriggered() 콜백 실행
   a. AnimBlueprint->ChangeState(true) - 버튼 눌림 애니메이션
   b. Timer 설정 (RecoveryDelay)
   c. OnActivate() 호출 (Blueprint 이벤트)

[타이머 종료]
8. RecoveryButton() 호출
   → AnimBlueprint->ChangeState(false) - 버튼 복원
```

---

## Interactable 객체 구현

### 1. luggage (PickUp 타입)

**파일**: `Source/Onepiece/Interactable/luggage.h/cpp`

#### 특징
- **타입**: EInteractionType::PickUp
- **물리**: 시뮬레이션 활성화 (무게 5kg)
- **네트워크**: Replication 지원

#### 구현

```cpp
Aluggage::Aluggage()
{
    // Mesh 설정
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    SetRootComponent(Mesh);
    
    // InteractableComponent 생성
    InteractableComp = CreateDefaultSubobject<UInteractableComponent>(TEXT("Interactable"));
    InteractableComp->InteractionType = EInteractionType::PickUp;
    InteractableComp->InteractionPrompt = TEXT("Press E to Grab");
    
    // 물리 설정
    Mesh->SetSimulatePhysics(true);
    Mesh->SetEnableGravity(true);
    Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    Mesh->SetCollisionProfileName(TEXT("PhysicsActor"));
    Mesh->SetMassOverrideInKg(NAME_None, 5.f, true);
    
    // Replication
    bReplicates = true;
    SetReplicateMovement(true);
}
```

#### 사용 방법
1. 레벨에 배치
2. 자동으로 DetectionRange 생성됨
3. Player가 접근하여 E 키로 들기/놓기

---

### 2. PedestalSwitch (Button 타입)

**파일**: `Source/Onepiece/Interactable/APedestalSwitch.h/cpp`

#### 특징
- **타입**: EInteractionType::Button
- **동작**: 버튼 누르기 → 애니메이션 → 자동 복원
- **확장성**: OnActivate() Blueprint 이벤트로 동작 커스터마이징

#### 구현

```cpp
APedestalSwitch::APedestalSwitch()
{
    SwitchBody = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SwitchBody"));
    
    InteractableComp = CreateDefaultSubobject<UInteractableComponent>(TEXT("Interactable"));
    InteractableComp->InteractionType = EInteractionType::Button;
    InteractableComp->InteractionPrompt = TEXT("Press E to Activate");
}

void APedestalSwitch::BeginPlay()
{
    Super::BeginPlay();
    
    InitSwitch();
    
    // 델리게이트 바인딩
    InteractableComp->OnInteractionTriggered.AddDynamic(this, &APedestalSwitch::OnInteractionTriggered);
}

void APedestalSwitch::OnInteractionTriggered(AActor* Interactor)
{
    if (AnimBlueprint)
    {
        // 버튼 눌림
        AnimBlueprint->ChangeState(true);
        
        // 자동 복원 타이머
        GetWorld()->GetTimerManager().SetTimer(
            RecoveryTimerHandle,
            this,
            &APedestalSwitch::RecoveryButton,
            RecoveryDelay,
            false
        );
    }
    
    OnActivate();  // Blueprint 이벤트
}
```

#### 확장 예시
Blueprint에서 `OnActivate` 이벤트를 구현하여 동작 정의:
```
OnActivate 이벤트
  └─> 문 열기
  └─> 퍼즐 진행
  └─> 사운드/VFX 재생
```

---

### 3. WeightSwitch (압력판 타입)

**파일**: `Source/Onepiece/Interactable/AWeightSwitch.h/cpp`

#### 특징
- **감지 방식**: Overlap (InteractableComponent가 아닌 자체 BoxComponent 사용)
- **동작**: 물체가 올라가면 활성화, 내려가면 비활성화
- **브로드캐스트**: UBroadcastManager를 통한 전역 이벤트 발송

#### 구현

```cpp
AWeightSwitch::AWeightSwitch()
{
    SwitchBody = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SwitchBody"));
    
    SwitchCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("SwitchCollision"));
    SwitchCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    SwitchCollision->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
}

void AWeightSwitch::BeginPlay()
{
    Super::BeginPlay();
    
    SwitchCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeightSwitch::OnBeginOverlap);
    SwitchCollision->OnComponentEndOverlap.AddDynamic(this, &AWeightSwitch::OnEndOverlap);
    
    UBroadcastManager::Get(GetWorld())->OnWeightSwitch.AddDynamic(this, &AWeightSwitch::OnWeightSwitch);
}

void AWeightSwitch::OnBeginOverlap(...)
{
    OverlappingActors.AddUnique(OtherActor);
    
    if (OverlappingActors.Num() == 1)
    {
        DetectTarget = true;
        ElapsedTime = 0.0;
    }
}

void AWeightSwitch::Tick(float DeltaTime)
{
    if (!DetectTarget) return;
    
    ElapsedTime += DeltaTime;
    
    if (ActivateTrigger())  // TriggerDelay 경과
    {
        UBroadcastManager::Get(GetWorld())->SendWeightSwitch(ButtonIndex, true);
    }
}

void AWeightSwitch::OnWeightSwitch(int InButtonIndex, bool InActive)
{
    if (InButtonIndex != ButtonIndex) return;
    
    SetActivate(InActive);  // 애니메이션 + 머티리얼 변경
    OnActivate(InActive);   // Blueprint 이벤트
}
```

#### 특징 정리
- **여러 물체 감지**: OverlappingActors 배열로 관리
- **딜레이 트리거**: TriggerDelay 시간 동안 물체가 올라가 있어야 활성화
- **시각적 피드백**: EmissiveMaterial로 색상 변경 (DeactivateColor ↔ ActivateColor)

---

### 4. Door (수신 전용 객체)

**파일**: `Source/Onepiece/Interactable/ADoor.h/cpp`

#### 특징
- **InteractableComponent 없음**: 직접 상호작용 불가
- **브로드캐스트 수신**: UBroadcastManager::OnDoorMessage 리스닝
- **퍼즐 메커니즘**: ReqCount만큼 활성화 신호를 받아야 열림

#### 구현 개념
```cpp
void ADoor::BeginPlay()
{
    Super::BeginPlay();
    
    // 브로드캐스트 리스닝
    UBroadcastManager::Get(GetWorld())->OnDoorMessage.AddDynamic(this, &ADoor::OnDoorMessage);
}

void ADoor::OnDoorMessage(int32 InDoorIndex, bool bInOpen)
{
    if (InDoorIndex != DoorIndex) return;
    
    if (bInOpen)
    {
        CurCount++;
        if (CurCount >= ReqCount)
        {
            OpenDoor();
        }
    }
    else
    {
        CurCount--;
        if (CurCount < ReqCount)
        {
            CloseDoor();
        }
    }
}
```

---

## 확장 가이드

### 새로운 상호작용 타입 추가

#### 1단계: EInteractionType에 타입 추가

`InteractableComponent.h`:
```cpp
UENUM(BlueprintType)
enum class EInteractionType : uint8
{
    None,
    PickUp,
    Button,
    Examine,  // 새로운 타입 추가
};
```

#### 2단계: UInteractionSystem에 처리 로직 추가

`UInteractionSystem.cpp`:
```cpp
void UInteractionSystem::TryInteract()
{
    if (!CurrentTarget || !CurrentTarget->bCanInteract)
        return;
    
    switch (CurrentTarget->InteractionType)
    {
        case EInteractionType::PickUp:
            TryPickUp();
            break;
        
        case EInteractionType::Button:
            CurrentTarget->TriggerInteraction(OwnerPlayer);
            break;
        
        case EInteractionType::Examine:  // 새로운 처리
            TryExamine();
            break;
    }
}

void UInteractionSystem::TryExamine()
{
    if (!CurrentTarget) return;
    
    // 검사 UI 표시, 카메라 줌 등
    CurrentTarget->TriggerInteraction(OwnerPlayer);
}
```

#### 3단계: 새로운 Interactable Actor 생성

```cpp
UCLASS()
class ONEPIECE_API AExaminableObject : public AActor
{
    GENERATED_BODY()

public:
    AExaminableObject()
    {
        Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
        SetRootComponent(Mesh);
        
        InteractableComp = CreateDefaultSubobject<UInteractableComponent>(TEXT("Interactable"));
        InteractableComp->InteractionType = EInteractionType::Examine;
        InteractableComp->InteractionPrompt = TEXT("Press E to Examine");
    }
    
    void BeginPlay() override
    {
        Super::BeginPlay();
        
        // 델리게이트 바인딩
        InteractableComp->OnInteractionTriggered.AddDynamic(this, &AExaminableObject::OnExamine);
    }
    
    UFUNCTION()
    void OnExamine(AActor* Interactor)
    {
        // 검사 로직
        UE_LOG(LogTemp, Log, TEXT("Examining object"));
    }

private:
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* Mesh;
    
    UPROPERTY(VisibleAnywhere)
    UInteractableComponent* InteractableComp;
};
```

---

### 복잡한 상호작용 체인 구성

#### 예시: 퍼즐 문 시스템

```cpp
[시나리오]
3개의 PedestalSwitch를 모두 눌러야 문이 열림

[구성]
PedestalSwitch A (ButtonIndex = 0)
  └─> OnActivate() → SendDoorMessage(0, true)

PedestalSwitch B (ButtonIndex = 1)
  └─> OnActivate() → SendDoorMessage(0, true)

PedestalSwitch C (ButtonIndex = 2)
  └─> OnActivate() → SendDoorMessage(0, true)

Door (DoorIndex = 0, ReqCount = 3)
  └─> OnDoorMessage(0, true) → CurCount++
       └─> if (CurCount >= 3) → OpenDoor()
```

#### Blueprint 구현 예시

**PedestalSwitch_BP**:
```
OnActivate 이벤트
  └─> Get BroadcastManager
       └─> SendDoorMessage
            - DoorIndex: 0
            - bOpen: true
```

**Door_BP**:
```
OnDoorMessage 델리게이트
  └─> if (InDoorIndex == DoorIndex)
       └─> CurCount++
            └─> if (CurCount >= ReqCount)
                 └─> OpenDoor (Timeline Animation)
```

---

## Best Practices

### 1. 네트워크 동기화

#### PickUp 타입 객체
```cpp
// Actor 클래스
bReplicates = true;
SetReplicateMovement(true);

// InteractableComponent
SetIsReplicatedByDefault(true);

UPROPERTY(ReplicatedUsing=OnRep_HoldingOwner)
AActor* HoldingOwner;

// GetLifetimeReplicatedProps 구현
DOREPLIFETIME(UInteractableComponent, HoldingOwner);
```

#### RPC 호출
```cpp
// Client → Server
void PickUp()
{
    GetOwner()->SetOwner(HoldingOwner);
    Server_PickUp();  // RPC
}

UFUNCTION(Server, Reliable)
void Server_PickUp();

void Server_PickUp_Implementation()
{
    // 서버에서만 실행되는 로직
    if (HoldingOwner->HasAuthority())
    {
        OnRep_HoldingOwner();
    }
}
```

### 2. 유효성 검사

#### UInteractionSystem에서
```cpp
void UInteractionSystem::TryPickUp()
{
    // 유효성 검사 강화
    if (!IsValid(CurrentTarget))
    {
        CurrentTarget = nullptr;
        return;
    }
    
    AActor* TargetOwner = CurrentTarget->GetOwner();
    if (!TargetOwner || !IsValid(TargetOwner))
    {
        return;
    }
    
    // 로직 실행
    ...
}
```

#### UInteractableComponent에서
```cpp
void UInteractableComponent::RegisterInteractable(UInteractableComponent* Interactable)
{
    if (!Interactable || !IsValid(Interactable))
    {
        return;
    }
    
    if (NearbyInteractables.Contains(Interactable))
        return;
    
    NearbyInteractables.Add(Interactable);
}
```

### 3. 중요한 순서 보장

#### PickUp 시 HoldingInteractable 설정 타이밍
```cpp
// ❌ 잘못된 구현
void TryPickUp()
{
    CurrentTarget->PickUp();
    HoldingInteractable = CurrentTarget;  // 너무 늦음!
}

// ✅ 올바른 구현
void TryPickUp()
{
    HoldingInteractable = CurrentTarget;  // PickUp 호출 **전**
    HoldingInteractable->PickUp();
}
```

**이유**: `PickUp()` 과정에서 `DetachFromActor`가 호출되면 Overlap이 해제되고,  
`OnDetectionEndOverlap` → `UnregisterInteractable` → `CurrentTarget = nullptr`이 됨.

### 4. DetectionRange 자동 생성

#### BeginPlay에서 생성
```cpp
void UInteractableComponent::BeginPlay()
{
    Super::BeginPlay();
    
    AActor* Owner = GetOwner();
    if (!Owner) return;
    
    // DetectionRange가 없으면 자동 생성
    if (!DetectionRange)
    {
        DetectionRange = NewObject<UBoxComponent>(Owner, ...);
        DetectionRange->RegisterComponent();
        DetectionRange->AttachToComponent(Owner->GetRootComponent(), ...);
        
        // 크기 설정
        FVector BoxExtent(DetectionDistance, DetectionDistance, DetectionDistance);
        DetectionRange->SetBoxExtent(BoxExtent);
        
        // Collision 설정
        DetectionRange->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        DetectionRange->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
        
        // 콜백 바인딩
        DetectionRange->OnComponentBeginOverlap.AddDynamic(...);
        DetectionRange->OnComponentEndOverlap.AddDynamic(...);
    }
}
```

### 5. 디버그 시각화

#### UInteractionSystem
```cpp
void UInteractionSystem::TickComponent(...)
{
    CurrentTarget = DetectInteractableTarget();
    
    if (bShowDebugInfo && CurrentTarget)
    {
        CurrentTarget->ShowDebugInfo(OwnerPlayer);
    }
}

bool UInteractionSystem::PerformCenterLineTrace(FHitResult& OutHit)
{
    bool bHit = GetWorld()->LineTraceSingleByChannel(...);
    
    if (bShowDebugInfo)
    {
        DrawDebugLine(
            GetWorld(), TraceStart, TraceEnd,
            bHit ? FColor::Green : FColor::Red,
            false, 0.0f, 0, 0.5f
        );
    }
    
    return bHit;
}
```

#### UInteractableComponent
```cpp
void UInteractableComponent::ShowDebugInfo(AActor* ViewerActor)
{
    FVector DebugLocation = GetOwner()->GetActorLocation() + FVector(0, 0, 100);
    
    DrawDebugString(
        GetWorld(),
        DebugLocation,
        InteractionPrompt,
        nullptr,
        bIsPickedUp ? FColor::Yellow : FColor::Green,
        0.0f,
        true,
        1.2f
    );
}
```

### 6. 델리게이트 패턴 활용

#### 선언
```cpp
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionTriggered, AActor*, Interactor);

UPROPERTY(BlueprintAssignable)
FOnInteractionTriggered OnInteractionTriggered;
```

#### 바인딩
```cpp
void APedestalSwitch::BeginPlay()
{
    Super::BeginPlay();
    
    InteractableComp->OnInteractionTriggered.AddDynamic(
        this, 
        &APedestalSwitch::OnInteractionTriggered
    );
}
```

#### 브로드캐스트
```cpp
void UInteractableComponent::TriggerInteraction(AActor* Interactor)
{
    OnInteractionTriggered.Broadcast(Interactor);
}
```

---

## 사용 사례 요약

### 사용 사례 1: 물건 집어서 퍼즐 풀기

```
[시나리오]
luggage를 집어서 WeightSwitch 위에 올려놓으면 문이 열림

[흐름]
1. Player가 luggage를 E 키로 집음 (PickUp)
2. Player가 WeightSwitch 위로 이동
3. E 키로 luggage를 놓음 (Drop)
4. luggage가 WeightSwitch와 Overlap
   → WeightSwitch 활성화
   → SendWeightSwitch(ButtonIndex, true)
5. Door가 OnDoorMessage 수신
   → 문 열림
```

### 사용 사례 2: 연속 버튼 퍼즐

```
[시나리오]
3개의 PedestalSwitch를 순서대로 눌러야 문이 열림

[구현]
각 PedestalSwitch의 OnActivate()에서:
  - 올바른 순서면 다음 단계 활성화
  - 잘못된 순서면 리셋

Blueprint 로직으로 순서 검증 구현
```

### 사용 사례 3: 시간 제한 퍼즐

```
[시나리오]
WeightSwitch를 TriggerDelay 시간 동안 눌러야 활성화

[구현]
AWeightSwitch::Tick()에서:
  - DetectTarget == true일 때만 ElapsedTime 증가
  - TriggerDelay 경과 시 활성화
  - 물체가 내려가면 리셋
```

---

## 문제 해결 가이드

### 문제 1: CurrentTarget이 null이 되는 경우

**원인**: PickUp() 호출 중 DetachFromActor로 Overlap 해제

**해결**:
```cpp
// HoldingInteractable에 먼저 저장
HoldingInteractable = CurrentTarget;
HoldingInteractable->PickUp();
```

### 문제 2: 멀티플레이어에서 물건이 동기화되지 않음

**원인**: Replication 설정 누락

**해결**:
```cpp
// Actor
bReplicates = true;
SetReplicateMovement(true);

// Component
SetIsReplicatedByDefault(true);
DOREPLIFETIME(UInteractableComponent, HoldingOwner);
```

### 문제 3: DetectionRange가 생성되지 않음

**원인**: Owner가 null이거나 BeginPlay 타이밍 문제

**해결**:
```cpp
void UInteractableComponent::BeginPlay()
{
    Super::BeginPlay();
    
    AActor* Owner = GetOwner();
    if (!Owner)
    {
        UE_LOG(LogTemp, Error, TEXT("Owner is null!"));
        return;
    }
    
    // DetectionRange 생성 로직
    ...
}
```

---

## 성능 고려사항

### 1. LineTrace 최적화
- **현재**: 매 프레임 실행
- **최적화**: NearbyInteractables가 비어있을 때는 스킵 (이미 구현됨)

### 2. NearbyInteractables 배열 관리
- **중복 방지**: `Contains()` 체크
- **자동 정리**: Overlap 해제 시 제거

### 3. 디버그 표시
- **에디터 전용**: Shipping 빌드에서는 자동으로 제거됨
- **조건부 실행**: `bShowDebugInfo` 플래그로 제어

---

## 결론

이 Interaction System은 **Overlap + LineTrace** 이중 감지 방식을 통해  
정밀하고 확장 가능한 상호작용을 제공합니다.

### 핵심 장점
1. **정확한 타겟팅**: 화면 중앙 LineTrace로 의도한 객체만 선택
2. **성능 효율**: Overlap으로 후보군 필터링 → LineTrace는 최소한만 실행
3. **확장성**: 새로운 InteractionType 추가 용이
4. **네트워크 지원**: Replication 기반 멀티플레이어 대응
5. **재사용성**: UInteractableComponent를 붙이기만 하면 자동으로 동작

### 확장 가능성
- 새로운 상호작용 타입 (Examine, Dialogue, Craft 등)
- 복잡한 퍼즐 메커니즘
- 조건부 상호작용 (아이템 보유, 퀘스트 상태 등)
- 애니메이션/사운드 통합

---

## 참고 파일 위치

```
Source/Onepiece/
├─ Character/
│  ├─ Public/UInteractionSystem.h
│  └─ Private/UInteractionSystem.cpp
│
├─ Interactable/
│  ├─ Public/
│  │  ├─ InteractableComponent.h
│  │  ├─ luggage.h
│  │  ├─ APedestalSwitch.h
│  │  ├─ AWeightSwitch.h
│  │  └─ ADoor.h
│  │
│  └─ Private/
│     ├─ InteractableComponent.cpp
│     ├─ luggage.cpp
│     ├─ APedestalSwitch.cpp
│     ├─ AWeightSwitch.cpp
│     └─ ADoor.cpp
```

---

**문서 버전**: 1.0  
**최종 수정**: 2025-11-23  
**작성자**: Claude Code Agent
