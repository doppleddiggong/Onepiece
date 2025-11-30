# Luggage 상호작용 RPC 정리 가이드 (2025-11-30, KST)

## 1. 개요

### 1.1 목적
본 문서는 물리 기반 오브젝트인 **Luggage**가 포함된 세 가지 상호작용 시스템(Pickup, Conveyor Belt, Hook)에 대해:
1. 현재 RPC 구현의 문제점을 진단
2. 멀티플레이어 환경에서의 보안 및 일관성 리스크 분석
3. 단계별 개선 방법을 제시

### 1.2 범위
- **InteractableComponent**: 픽업/드롭 상호작용
- **ConveyorBelt**: 컨베이어벨트 이동 처리
- **UHookSystem**: 그래플링 훅 시스템

### 1.3 배경
- 클라이언트가 RPC 호출 시 물리 상태와 소유권을 함께 전송하는 패턴이 혼재
- 서버 권한 검증과 재현성이 부족한 구간 발견
- 상태 플래그(`bIsPickedUp`) 미복제로 인한 시스템 간 충돌 가능성

---

## 2. 문제 진단 및 코드 분석

### 2.1 InteractableComponent (Pickup/Drop)

#### 2.1.1 핵심 문제점

**문제 1: 클라이언트 제공 소유자를 무검증으로 신뢰**
```cpp
// Source/Onepiece/Interactable/Private/InteractableComponent.cpp:156-165
void UInteractableComponent::PickUp(AActor* NewHoldingOwner)
{
    if (!NewHoldingOwner)
    {
        PRINTLOG( TEXT("InteractableComponent::PickUp - NewHoldingOwner is null"));
        return;
    }

    Server_PickUp(NewHoldingOwner);  // 클라이언트가 제공한 NewHoldingOwner를 그대로 전달
}
```

**위험:**
- 클라이언트가 임의의 액터를 `NewHoldingOwner`로 전달 가능
- 다른 플레이어의 캐릭터를 지정하여 소유권 탈취 가능
- 거리 검증 없이 멀리 있는 오브젝트도 픽업 가능

**문제 2: bIsPickedUp 상태 미복제**
```cpp
// Source/Onepiece/Interactable/Public/InteractableComponent.h:144
bool bIsPickedUp = false;  // Replicated 없음!

// Source/Onepiece/Interactable/Private/InteractableComponent.cpp:132-137
void UInteractableComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UInteractableComponent, HoldingOwner);  // HoldingOwner만 복제
}
```

**위험:**
- `HoldingOwner`는 복제되지만 `bIsPickedUp`은 복제되지 않음
- ConveyorBelt가 `IsPickedUp()` 체크로 이동 여부 결정 → 클라이언트마다 다른 상태 가능
- 클라이언트에서 luggage가 컨베이어에서 움직이거나 멈추는 것이 불일치

**문제 3: 거리 검증 부재**
```cpp
// Source/Onepiece/Interactable/Private/InteractableComponent.cpp:167-202
void UInteractableComponent::Server_PickUp_Implementation(AActor* NewHoldingOwner)
{
    if (bIsPickedUp) return;  // 중복 픽업만 체크

    HoldingOwner = NewHoldingOwner;  // 거리 검증 없음!
    // ... 나머지 로직
}
```

**위험:**
- DetectionRange overlap 이벤트는 클라이언트에서만 발생
- 서버 RPC에서는 재검증하지 않아 멀리 떨어진 오브젝트도 픽업 가능

#### 2.1.2 긍정적 요소
- `HoldingOwner` 복제 및 `OnRep_HoldingOwner`로 비주얼 동기화
- 기본적인 중복 픽업 방지 (`bIsPickedUp` 체크)
- 물리 상태 저장/복원 메커니즘 존재

---

### 2.2 ConveyorBelt

#### 2.2.1 핵심 문제점

**문제 1: 무분별한 방향 전환 권한**
```cpp
// Source/Onepiece/Prop/Private/ConveyorBelt.cpp:62-76
void AConveyorBelt::ChangeConveyorMovement()
{
    ServerRPC_ChangeConveyorMovement();
}

void AConveyorBelt::ServerRPC_ChangeConveyorMovement_Implementation()
{
    MulticastRPC_ChangeConveyorMovement();
}

void AConveyorBelt::MulticastRPC_ChangeConveyorMovement_Implementation()
{
    bIsForward = !bIsForward;  // 아무 클라이언트나 방향 전환 가능!
}
```

**위험:**
- 누구나 언제든지 컨베이어 방향 전환 가능
- 쿨다운, 권한 체크, 트리거 조건 없음
- 게임 로직 파괴 가능 (예: 특정 스위치로만 전환되어야 할 경우)

**문제 2: 물리 속도 덮어쓰기 방식**
```cpp
// Source/Onepiece/Prop/Private/ConveyorBelt.cpp:122-133
if (primComp && primComp->IsSimulatingPhysics())
{
    float speed = MoveSpeed;
    FVector targetVelocity = moveDirection * speed;

    FVector currentVelocity = primComp->GetPhysicsLinearVelocity();
    FVector newVelocity = targetVelocity;
    newVelocity.Z = currentVelocity.Z;  // Z축 속도만 유지

    primComp->SetPhysicsLinearVelocity(newVelocity, false);  // 매 프레임 덮어씀
}
```

**위험:**
- 매 프레임 속도를 강제로 설정하여 물리 시뮬레이션 부자연스러움
- 네트워크 소유권이 클라이언트일 때 서버/클라이언트 간 물리 충돌 가능
- 위치 스냅 및 떨림 발생 가능

**문제 3: Tick이 모든 머신에서 실행**
```cpp
// Source/Onepiece/Prop/Private/ConveyorBelt.cpp:54-60
void AConveyorBelt::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    MoveOverlappedSkeletals(MoveSpeed * DeltaTime);
    MoveOverlappedStatics(MoveSpeed * DeltaTime);  // 서버/클라이언트 구분 없음
}
```

**위험:**
- 클라이언트에서도 물리 이동 시도 → 서버 물리 복제와 충돌
- Authority 체크 없어 예측 불일치 발생 가능

#### 2.2.2 긍정적 요소
- `IsPickedUp()` 체크로 픽업 중 컨베이어 이동 방지 (ConveyorBelt.cpp:112-115)
- 물리 객체 감지 및 분리 처리

---

### 2.3 UHookSystem

#### 2.3.1 핵심 문제점

**문제 1: 클라이언트 LineTrace 결과를 서버가 재검증 없이 신뢰**
```cpp
// Source/Onepiece/Character/Private/UHookSystem.cpp:116-156
void UHookSystem::TryHook()
{
    // ... 클라이언트에서 LineTrace 수행
    FHitResult HitResult;
    if (!PerformCenterLineTrace(HitResult))
    {
        return;
    }

    // ... 클라이언트 측 검증만 수행

    // Server에 Hit 결과를 그대로 전달
    ServerTryHook(HitResult);  // 클라이언트 HitResult를 그대로 전송!
}

// Source/Onepiece/Character/Private/UHookSystem.cpp:158-181
void UHookSystem::ServerTryHook_Implementation(const FHitResult& HitResult)
{
    // 서버에서 재검증
    AActor* HitActor = HitResult.GetActor();  // 클라이언트가 제공한 결과만 확인
    if (!HitActor) return;

    UHookComponent* HookComp = HitActor->FindComponentByClass<UHookComponent>();
    if (!HookComp || !HookComp->bIsHookable) return;

    // 클라이언트가 제공한 HitResult를 그대로 사용!
    StartHook(HitResult);
}
```

**위험:**
- 클라이언트가 조작한 HitResult 전송 가능 (벽 너머 오브젝트, 거리 밖 오브젝트)
- 서버에서 재추적하지 않아 치트 가능
- 시야 밖/최대 거리 밖 오브젝트를 훅으로 끌어올 수 있음

**문제 2: 클라이언트 Hit 위치 기반 물리/충돌 설정**
```cpp
// Source/Onepiece/Character/Private/UHookSystem.cpp:183-230
void UHookSystem::StartHook(const FHitResult& Hit)
{
    // ...

    // 발사 위치 및 방향 설정
    HookProjectileLocation = OwnerPlayer->GetActorLocation();
    HookLaunchDirection = (Hit.Location - HookProjectileLocation).GetSafeNormal();  // 클라이언트 Hit.Location 사용

    // 물리 및 충돌 끄기
    UPrimitiveComponent* RootPrimitive = Cast<UPrimitiveComponent>(HookedTarget->GetRootComponent());
    if (RootPrimitive)
    {
        OriginalCollisionEnabled = RootPrimitive->GetCollisionEnabled();
        RootPrimitive->SetSimulatePhysics(false);
        RootPrimitive->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
}
```

**위험:**
- 클라이언트가 제공한 `Hit.Location`으로 발사 방향 결정
- 실제 서버 상의 오브젝트 위치와 다를 수 있음
- 물리 상태 즉시 변경으로 다른 클라이언트가 순간이동 보일 수 있음

**문제 3: 중복 훅 방지 메커니즘 부족**
```cpp
// Source/Onepiece/Character/Private/UHookSystem.cpp:124-129
if (HookState != EHookState::Idle)
{
    PRINTLOG(TEXT("UHookSystem: Already in hook state!"));
    return;
}
```

**위험:**
- 자신의 `HookState`만 체크
- 다른 플레이어가 동일한 Luggage를 동시에 훅하려 할 때 방지 장치 없음
- Luggage가 여러 플레이어에게 동시에 끌려가는 상황 가능

#### 2.3.2 긍정적 요소
- `HookState`, `HookedTarget`, `HookProjectileLocation`, `HookLaunchDirection` 모두 복제
- RepNotify로 비주얼 동기화 (`OnRep_HookState`, `OnRep_HookProjectileLocation`)
- 서버 권한에서만 위치 업데이트 (UpdateHookPulling:476-501)
- 물리 상태 저장/복원 메커니즘
- 속도 초기화 후 물리 복원 (ReleaseHook:251-258)

---

## 3. 개선 가이드

### 3.1 공통 RPC 원칙

#### 원칙 1: 서버 재검증 (Never Trust Client)
```cpp
// ❌ 잘못된 예시
void ServerRPC_DoSomething_Implementation(const FHitResult& ClientHit)
{
    AActor* HitActor = ClientHit.GetActor();  // 클라이언트 제공 데이터 신뢰
    DoSomething(HitActor);
}

// ✅ 올바른 예시
void ServerRPC_DoSomething_Implementation(const FHitResult& ClientHint)
{
    // 서버에서 재검증
    FHitResult ServerHit;
    if (!PerformServerSideLineTrace(ServerHit))
        return;

    // 서버 기준으로 거리, 시야, 권한 검증
    if (!ValidateInteraction(ServerHit))
        return;

    DoSomething(ServerHit.GetActor());
}
```

#### 원칙 2: 권한 분리 (Server Authority)
- **입력**: 클라이언트 담당
- **상태 변환**: 서버 단독 권한
- **물리 제어**: 서버 단독 권한
- **시각 효과**: Multicast로 전파

#### 원칙 3: 상태 플래그 복제 (State Replication)
```cpp
// ❌ 잘못된 예시
UPROPERTY()
bool bIsPickedUp = false;  // 복제 안됨

// ✅ 올바른 예시
UPROPERTY(ReplicatedUsing = OnRep_IsPickedUp)
bool bIsPickedUp = false;

UFUNCTION()
void OnRep_IsPickedUp()
{
    // 클라이언트에서 상태 변화 반영
    UpdateVisuals();
    UpdatePhysics();
}
```

#### 원칙 4: Multicast 최소화
- 물리/이동 업데이트는 Multicast 금지
- 서버가 상태를 변경하고 복제로 전파
- Multicast는 사운드/파티클 등 시각 효과만 사용

---

### 3.2 InteractableComponent 개선 방안

#### 3.2.1 단계별 구현 가이드

**Step 1: bIsPickedUp 복제 추가**

```cpp
// InteractableComponent.h
UPROPERTY(ReplicatedUsing = OnRep_IsPickedUp, BlueprintReadOnly, Category = "Interaction")
bool bIsPickedUp = false;

UFUNCTION()
void OnRep_IsPickedUp();
```

```cpp
// InteractableComponent.cpp

void UInteractableComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UInteractableComponent, HoldingOwner);
    DOREPLIFETIME(UInteractableComponent, bIsPickedUp);  // 추가!
}

void UInteractableComponent::OnRep_IsPickedUp()
{
    Aluggage* luggage = Cast<Aluggage>(GetOwner());
    if (!luggage)
        return;

    if (bIsPickedUp)
    {
        // 픽업 상태 - Outline/Widget 끄기
        luggage->OutlineOff();
        HideInteractWidget();
    }
    else
    {
        // 드롭 상태 - Outline/Widget 켜기
        luggage->OutlineOn();
        // ShowInteractWidget();  // 필요 시
    }
}
```

**Step 2: 서버 권한 검증 강화**

```cpp
// InteractableComponent.h
UFUNCTION(Server, Reliable, WithValidation)
void Server_PickUp(class AActor* NewHoldingOwner);

UFUNCTION(Server, Reliable, WithValidation)
void Server_Drop();
```

```cpp
// InteractableComponent.cpp

bool UInteractableComponent::Server_PickUp_Validate(AActor* NewHoldingOwner)
{
    // 1. NewHoldingOwner가 유효한지
    if (!NewHoldingOwner)
        return false;

    // 2. NewHoldingOwner가 실제 PlayerController에 의해 제어되는지
    APlayerActor* PlayerActor = Cast<APlayerActor>(NewHoldingOwner);
    if (!PlayerActor || !PlayerActor->GetController())
        return false;

    // 3. 이미 픽업된 상태인지
    if (bIsPickedUp)
        return false;

    // 4. HoldingOwner가 이미 있는지
    if (HoldingOwner && HoldingOwner != NewHoldingOwner)
        return false;

    // 5. 거리 검증 (서버 기준)
    AActor* Owner = GetOwner();
    if (!Owner)
        return false;

    float Distance = FVector::Dist(Owner->GetActorLocation(), NewHoldingOwner->GetActorLocation());
    if (Distance > DetectionDistance * 1.5f)  // 약간의 여유 (네트워크 지연 고려)
    {
        PRINTLOG(TEXT("Server_PickUp_Validate: Too far! Distance: %.2f, Max: %.2f"), Distance, DetectionDistance * 1.5f);
        return false;
    }

    return true;
}

void UInteractableComponent::Server_PickUp_Implementation(AActor* NewHoldingOwner)
{
    // Validation 통과 후 실행
    HoldingOwner = NewHoldingOwner;

    UPrimitiveComponent* PrimComp = GetOwnerPrimitiveComponent();
    if (!PrimComp)
        return;

    // 원래 상태 저장
    bOriginalSimulatePhysics = PrimComp->IsSimulatingPhysics();
    OriginalCollisionType = PrimComp->GetCollisionEnabled();

    // 물리 끄기
    PrimComp->SetSimulatePhysics(false);

    // Attach
    OnRep_HoldingOwner();

    // 상태 변경 (복제됨)
    bIsPickedUp = true;  // OnRep_IsPickedUp 자동 호출됨

    PRINTLOG(TEXT("InteractableComponent::Server_PickUp - %s picked up by %s"),
        *GetOwner()->GetName(), *NewHoldingOwner->GetName());
}

bool UInteractableComponent::Server_Drop_Validate()
{
    // 1. 실제로 픽업된 상태인지
    if (!bIsPickedUp)
        return false;

    // 2. HoldingOwner가 유효한지
    if (!HoldingOwner)
        return false;

    return true;
}

void UInteractableComponent::Server_Drop_Implementation()
{
    // Validation 통과 후 실행

    // Detach
    GetOwner()->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

    // 물리 복원
    UPrimitiveComponent* PrimComp = GetOwnerPrimitiveComponent();
    if (PrimComp)
    {
        PrimComp->SetSimulatePhysics(bOriginalSimulatePhysics);
        PrimComp->SetCollisionEnabled(OriginalCollisionType);
    }

    // 상태 변경 (복제됨)
    bIsPickedUp = false;  // OnRep_IsPickedUp 자동 호출됨
    HoldingOwner = nullptr;  // OnRep_HoldingOwner 자동 호출됨

    PRINTLOG(TEXT("InteractableComponent::Server_Drop - %s dropped"), *GetOwner()->GetName());
}
```

**Step 3: 클라이언트 호출부 단순화**

```cpp
// InteractableComponent.cpp

void UInteractableComponent::PickUp(AActor* NewHoldingOwner)
{
    // 클라이언트 측 기본 검증 (빠른 피드백)
    if (!NewHoldingOwner)
    {
        PRINTLOG(TEXT("PickUp - NewHoldingOwner is null"));
        return;
    }

    if (bIsPickedUp)
    {
        PRINTLOG(TEXT("PickUp - Already picked up"));
        return;
    }

    // 서버 RPC 호출 (서버에서 재검증)
    Server_PickUp(NewHoldingOwner);
}

void UInteractableComponent::Drop()
{
    // 클라이언트 측 기본 검증
    if (!bIsPickedUp)
    {
        PRINTLOG(TEXT("Drop - Not picked up"));
        return;
    }

    // 서버 RPC 호출
    Server_Drop();
}
```

#### 3.2.2 테스트 체크리스트

- [ ] 2인 멀티플레이어에서 동시에 같은 Luggage 픽업 시도 → 한 명만 성공
- [ ] 거리 밖에서 픽업 시도 → 실패
- [ ] 픽업 중 다른 플레이어가 같은 Luggage 픽업 시도 → 실패
- [ ] 픽업 후 ConveyorBelt 위에 올렸을 때 이동하지 않음
- [ ] 드롭 후 물리 정상 복원 (중력, 충돌)

---

### 3.3 ConveyorBelt 개선 방안

#### 3.3.1 단계별 구현 가이드

**Step 1: Tick을 서버 전용으로 변경**

```cpp
// ConveyorBelt.cpp

void AConveyorBelt::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 서버에서만 이동 처리
    if (!HasAuthority())
        return;

    MoveOverlappedSkeletals(MoveSpeed * DeltaTime);
    MoveOverlappedStatics(MoveSpeed * DeltaTime);
}
```

**Step 2: 방향 전환 권한 제한**

```cpp
// ConveyorBelt.h
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conveyor")
bool bAllowPlayerToggle = false;  // 플레이어가 방향 전환 가능한지

// 마지막 방향 전환 시각
float LastToggleTime = 0.0f;

// 방향 전환 쿨다운 (초)
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conveyor")
float ToggleCooldown = 2.0f;
```

```cpp
// ConveyorBelt.cpp

void AConveyorBelt::ChangeConveyorMovement()
{
    // 권한 체크는 서버에서
    if (!HasAuthority())
    {
        ServerRPC_ChangeConveyorMovement();
        return;
    }

    // 서버에서 직접 호출된 경우
    ServerRPC_ChangeConveyorMovement_Implementation();
}

bool AConveyorBelt::ServerRPC_ChangeConveyorMovement_Validate()
{
    // 플레이어가 토글 가능한지
    if (!bAllowPlayerToggle)
        return false;

    // 쿨다운 체크
    float CurrentTime = GetWorld()->GetTimeSeconds();
    if (CurrentTime - LastToggleTime < ToggleCooldown)
        return false;

    return true;
}

void AConveyorBelt::ServerRPC_ChangeConveyorMovement_Implementation()
{
    // 쿨다운 갱신
    LastToggleTime = GetWorld()->GetTimeSeconds();

    // Multicast로 모든 클라이언트에 전파
    MulticastRPC_ChangeConveyorMovement();
}

void AConveyorBelt::MulticastRPC_ChangeConveyorMovement_Implementation()
{
    bIsForward = !bIsForward;
    PRINT_STRING(TEXT("Conveyor direction changed: %d"), bIsForward);
}
```

**Step 3: 물리 이동을 Force 기반으로 변경**

```cpp
// ConveyorBelt.cpp

void AConveyorBelt::MoveOverlappedStatics(float deltaDistance)
{
    TArray<UPrimitiveComponent*> overlappedComponents;
    BeltBoxComp->GetOverlappingComponents(overlappedComponents);

    for (const auto& comp : overlappedComponents)
    {
        AActor* owner = comp->GetOwner();
        Aluggage* luggage = Cast<Aluggage>(owner);
        if (!luggage || !Cast<UStaticMeshComponent>(comp))
            continue;

        // Hook이나 PickUp 중이면 무시
        if (luggage->InteractableComp && luggage->InteractableComp->IsPickedUp())
            continue;

        FVector moveDirection = MoveDirArrowComp->GetForwardVector();
        moveDirection = bIsForward ? moveDirection : -moveDirection;

        UPrimitiveComponent* primComp = Cast<UPrimitiveComponent>(comp);
        if (primComp && primComp->IsSimulatingPhysics())
        {
            // ✅ Force 기반 이동 (속도 덮어쓰기 대신)
            float forceMagnitude = MoveSpeed * primComp->GetMass() * 10.0f;  // 질량 기반 힘
            FVector force = moveDirection * forceMagnitude;

            // Z축 힘은 추가하지 않음 (중력 유지)
            force.Z = 0.0f;

            primComp->AddForce(force, NAME_None, false);
        }
    }
}
```

**Step 4: bIsForward 복제 추가 (옵션)**

```cpp
// ConveyorBelt.h
UPROPERTY(ReplicatedUsing = OnRep_IsForward, BlueprintReadOnly, Category = "Conveyor")
bool bIsForward = true;

UFUNCTION()
void OnRep_IsForward();

virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
```

```cpp
// ConveyorBelt.cpp

void AConveyorBelt::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AConveyorBelt, bIsForward);
}

void AConveyorBelt::OnRep_IsForward()
{
    // 방향 변경 시각 효과 (화살표 회전, 사운드 등)
    PRINTLOG(TEXT("ConveyorBelt: Direction changed to %s"), bIsForward ? TEXT("Forward") : TEXT("Backward"));
}

// MulticastRPC 제거하고 복제로 대체
void AConveyorBelt::ServerRPC_ChangeConveyorMovement_Implementation()
{
    LastToggleTime = GetWorld()->GetTimeSeconds();
    bIsForward = !bIsForward;  // 복제됨 → OnRep_IsForward 자동 호출
}
```

#### 3.3.2 테스트 체크리스트

- [ ] 서버만 컨베이어 이동 처리
- [ ] 방향 전환 쿨다운 동작
- [ ] 픽업된 Luggage는 컨베이어에서 이동하지 않음
- [ ] 물리 이동이 부드러움 (스냅 없음)
- [ ] Force 기반 이동으로 다른 물리 오브젝트와 자연스럽게 상호작용

---

### 3.4 UHookSystem 개선 방안

#### 3.4.1 단계별 구현 가이드

**Step 1: 서버 라인트레이스 재실행**

```cpp
// UHookSystem.cpp

void UHookSystem::ServerTryHook_Implementation(const FHitResult& ClientHint)
{
    // 서버 권한 체크
    if (!OwnerPlayer || !OwnerPlayer->HasAuthority())
        return;

    // ✅ 서버에서 라인트레이스 재실행
    FHitResult ServerHit;
    if (!PerformServerLineTrace(ServerHit))
    {
        PRINTLOG(TEXT("ServerTryHook: Server-side line trace failed"));
        return;
    }

    // 유효성 검증
    AActor* HitActor = ServerHit.GetActor();
    if (!HitActor)
        return;

    UHookComponent* HookComp = HitActor->FindComponentByClass<UHookComponent>();
    if (!HookComp || !HookComp->bIsHookable)
        return;

    // 거리 검증
    float Distance = FVector::Dist(OwnerPlayer->GetActorLocation(), HitActor->GetActorLocation());
    if (Distance > MaxHookDistance)
    {
        PRINTLOG(TEXT("ServerTryHook: Target too far! Distance: %.2f"), Distance);
        return;
    }

    // 서버 HitResult 사용
    StartHook(ServerHit);
}

bool UHookSystem::PerformServerLineTrace(FHitResult& OutHit)
{
    if (!OwnerPlayer)
        return false;

    // 플레이어 컨트롤러 가져오기
    APlayerController* PC = Cast<APlayerController>(OwnerPlayer->GetController());
    if (!PC)
        return false;

    // ✅ 서버 기준 카메라 위치/방향 사용
    FVector CameraLocation;
    FRotator CameraRotation;
    PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

    FVector TraceStart = CameraLocation;
    FVector TraceEnd = TraceStart + (CameraRotation.Vector() * InteractionDistance);

    // LineTrace 실행
    bool bHit = GetWorld()->LineTraceSingleByChannel(
        OutHit, TraceStart, TraceEnd, ECC_Visibility
    );

    // 디버그 라인 (서버)
    if (bShowDebugInfo)
    {
        DrawDebugLine(
            GetWorld(), TraceStart, TraceEnd,
            bHit ? FColor::Green : FColor::Red,
            false, 1.0f, 0, 2.0f
        );
    }

    return bHit;
}
```

**Step 2: 중복 훅 방지 메커니즘**

```cpp
// UHookSystem.h

// Luggage에 추가 (또는 UHookComponent에 추가)
UPROPERTY(Replicated, BlueprintReadOnly, Category = "Hook")
bool bIsBeingHooked = false;

UPROPERTY(Replicated)
TObjectPtr<AActor> HookedBy = nullptr;  // 누가 훅하고 있는지
```

```cpp
// luggage.h (또는 UHookComponent.h)

UPROPERTY(ReplicatedUsing = OnRep_IsBeingHooked, BlueprintReadOnly, Category = "Hook")
bool bIsBeingHooked = false;

UPROPERTY(Replicated)
TObjectPtr<AActor> HookedBy = nullptr;

UFUNCTION()
void OnRep_IsBeingHooked();
```

```cpp
// luggage.cpp

void Aluggage::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(Aluggage, bIsBeingHooked);
    DOREPLIFETIME(Aluggage, HookedBy);
}

void Aluggage::OnRep_IsBeingHooked()
{
    // 훅 상태 시각 효과
    if (bIsBeingHooked)
    {
        OutlineOn();
    }
}
```

```cpp
// UHookSystem.cpp

void UHookSystem::ServerTryHook_Implementation(const FHitResult& ClientHint)
{
    // ... 기존 검증 ...

    // ✅ 중복 훅 방지
    Aluggage* Luggage = Cast<Aluggage>(HitActor);
    if (Luggage)
    {
        // 이미 다른 플레이어가 훅하고 있는지
        if (Luggage->bIsBeingHooked && Luggage->HookedBy != OwnerPlayer)
        {
            PRINTLOG(TEXT("ServerTryHook: Already hooked by %s"), *Luggage->HookedBy->GetName());
            return;
        }

        // 픽업된 상태인지
        if (Luggage->InteractableComp && Luggage->InteractableComp->IsPickedUp())
        {
            PRINTLOG(TEXT("ServerTryHook: Luggage is picked up"));
            return;
        }

        // 훅 플래그 설정
        Luggage->bIsBeingHooked = true;
        Luggage->HookedBy = OwnerPlayer;
    }

    StartHook(ServerHit);
}

void UHookSystem::ReleaseHook()
{
    // ... 기존 로직 ...

    // ✅ 훅 플래그 해제
    if (HookedTarget && IsValid(HookedTarget))
    {
        Aluggage* Luggage = Cast<Aluggage>(HookedTarget);
        if (Luggage)
        {
            Luggage->bIsBeingHooked = false;
            Luggage->HookedBy = nullptr;
        }

        // ... 물리 복원 ...
    }

    // ... 나머지 로직 ...
}
```

**Step 3: WeakObjectPtr로 안전성 강화 (옵션)**

```cpp
// UHookSystem.h

UPROPERTY(Replicated)
TWeakObjectPtr<AActor> HookedTarget;  // TObjectPtr → TWeakObjectPtr
```

```cpp
// UHookSystem.cpp

void UHookSystem::UpdateHookPulling(float DeltaTime)
{
    // ✅ WeakObjectPtr 안전성 체크
    if (!HookedTarget.IsValid())
    {
        PRINTLOG(TEXT("UHookSystem: Hooked target destroyed, releasing hook"));
        ReleaseHook();
        return;
    }

    AActor* Target = HookedTarget.Get();
    if (!Target)
    {
        ReleaseHook();
        return;
    }

    // ... 나머지 로직 ...
}
```

#### 3.4.2 테스트 체크리스트

- [ ] 서버에서 라인트레이스 재검증 동작
- [ ] 거리 밖 오브젝트 훅 시도 → 실패
- [ ] 2명이 동시에 같은 Luggage 훅 시도 → 한 명만 성공
- [ ] 픽업된 Luggage 훅 시도 → 실패
- [ ] 훅 중 타겟 파괴 시 안전하게 해제
- [ ] 물리 복원 후 속도 초기화 동작 (튕겨나가지 않음)

---

## 4. 통합 테스트 시나리오

### 4.1 시나리오 1: 픽업 → 컨베이어 → 드롭 체인

**목적**: 세 시스템 간 상태 플래그 공유 검증

**단계**:
1. Player A가 Luggage를 픽업
2. 컨베이어벨트 위에 올림
3. 컨베이어가 Luggage를 이동시키지 않는지 확인 (bIsPickedUp = true)
4. Player A가 Luggage 드롭
5. 컨베이어가 즉시 Luggage를 이동시키는지 확인 (bIsPickedUp = false)

**검증**:
- [ ] 픽업 중 컨베이어 이동 안 함
- [ ] 드롭 후 즉시 컨베이어 이동 시작
- [ ] 모든 클라이언트에서 동일한 동작

---

### 4.2 시나리오 2: 훅 → 픽업 경쟁

**목적**: 중복 소유권 방지 검증

**단계**:
1. Player A가 Luggage에 대해 훅 시작 (Launching 상태)
2. Player B가 동시에 Luggage 픽업 시도
3. 둘 중 하나만 성공해야 함

**검증**:
- [ ] 훅 중(bIsBeingHooked = true) 픽업 실패
- [ ] 픽업 중(bIsPickedUp = true) 훅 실패
- [ ] 서버 로그에 경쟁 상황 기록

---

### 4.3 시나리오 3: 컨베이어 → 훅 간섭

**목적**: 물리 제어 권한 이양 검증

**단계**:
1. Luggage가 컨베이어벨트 위에서 이동 중
2. Player A가 Luggage에 대해 훅 시도
3. 훅 성공 시 컨베이어 이동 중지
4. 훅 해제 후 컨베이어 이동 재개

**검증**:
- [ ] 훅 시작 시 물리/충돌 꺼짐 → 컨베이어 이동 안 함
- [ ] 훅 해제 시 물리 복원 → 컨베이어 이동 재개
- [ ] 위치 스냅/떨림 없음

---

### 4.4 시나리오 4: 거리/권한 치트 시도

**목적**: 서버 재검증 동작 확인

**테스트 케이스**:
1. 클라이언트에서 거리 밖 Luggage 픽업 시도 → 서버 Validation 실패
2. 클라이언트에서 다른 플레이어 소유 Luggage 픽업 시도 → 서버 Validation 실패
3. 클라이언트에서 시야 밖 Luggage 훅 시도 → 서버 라인트레이스 재검증 실패
4. 클라이언트에서 컨베이어 방향 무한 토글 시도 → 쿨다운으로 제한

**검증**:
- [ ] 모든 치트 시도가 서버에서 차단됨
- [ ] 서버 로그에 Validation 실패 기록
- [ ] 클라이언트에 정상적인 상태 복구

---

## 5. 개선 우선순위 및 일정 제안

### 5.1 우선순위

| 우선순위 | 시스템 | 작업 | 리스크 | 난이도 |
|---------|--------|------|--------|--------|
| **P0 (즉시)** | InteractableComponent | bIsPickedUp 복제 추가 | 높음 (시스템 간 충돌) | 낮음 |
| **P0 (즉시)** | InteractableComponent | Server_PickUp Validation | 높음 (보안) | 중간 |
| **P1 (1주 이내)** | UHookSystem | 서버 라인트레이스 재실행 | 높음 (치트) | 중간 |
| **P1 (1주 이내)** | UHookSystem | 중복 훅 방지 | 중간 (동시성) | 중간 |
| **P2 (2주 이내)** | ConveyorBelt | Tick 서버 전용화 | 중간 (물리 불일치) | 낮음 |
| **P2 (2주 이내)** | ConveyorBelt | Force 기반 이동 | 중간 (물리 품질) | 중간 |
| **P3 (필요 시)** | ConveyorBelt | 방향 전환 권한 제한 | 낮음 (게임 로직) | 낮음 |

### 5.2 단계별 작업 제안

**Phase 1 (1주차)**:
- [ ] InteractableComponent bIsPickedUp 복제 구현
- [ ] Server_PickUp/Drop Validation 추가
- [ ] 통합 테스트 시나리오 1, 2 수행

**Phase 2 (2주차)**:
- [ ] UHookSystem 서버 라인트레이스 재실행 구현
- [ ] 중복 훅 방지 메커니즘 (bIsBeingHooked 추가)
- [ ] 통합 테스트 시나리오 3, 4 수행

**Phase 3 (3주차)**:
- [ ] ConveyorBelt Tick 서버 전용화
- [ ] Force 기반 이동 구현
- [ ] 전체 시나리오 재검증

**Phase 4 (필요 시)**:
- [ ] 컨베이어 방향 전환 권한 제한
- [ ] WeakObjectPtr 적용 (안전성 강화)
- [ ] 성능 프로파일링

---

## 6. 참고 자료

### 6.1 언리얼 공식 문서
- [Replication](https://docs.unrealengine.com/5.3/en-US/replication-in-unreal-engine/)
- [RPC Validation](https://docs.unrealengine.com/5.3/en-US/rpcs/)
- [Physics Replication](https://docs.unrealengine.com/5.3/en-US/physics-replication-in-unreal-engine/)

### 6.2 베스트 프랙티스
- **Never Trust Client**: 모든 RPC는 서버에서 재검증
- **Single Source of Truth**: 상태는 서버에서만 변경, 복제로 전파
- **Predictive Movement**: 클라이언트는 예측만, 서버가 보정
- **RepNotify Over Multicast**: 상태 변화는 복제 사용, Multicast는 효과만

### 6.3 디버깅 팁
```cpp
// 네트워크 로그 활성화
LogNet, LogNetTraffic, LogRep

// 복제 디버그 명령어
net.PackageMap.DebugObject <ActorName>

// RPC 추적
LogNetPlayerMovement
```

---

## 7. 문서 히스토리

| 날짜 | 버전 | 작성자 | 변경 내용 |
|------|------|--------|----------|
| 2025-11-30 | 1.0 | KLingo | 초안 작성 (진단 리포트 기반) |
| 2025-11-30 | 2.0 | KLingo | 실제 코드 분석 및 구체적 개선 가이드 추가 |

---

## 8. 요약

### 8.1 핵심 문제
1. **InteractableComponent**: bIsPickedUp 미복제, 거리/권한 검증 부재
2. **ConveyorBelt**: 무분별한 방향 전환, 물리 속도 덮어쓰기
3. **UHookSystem**: 클라이언트 LineTrace 신뢰, 중복 훅 미방지

### 8.2 개선 방향
1. **상태 복제 강화**: bIsPickedUp, bIsBeingHooked 등 핵심 플래그 복제
2. **서버 재검증**: 모든 RPC에서 거리/시야/권한 재확인
3. **권한 분리**: 서버만 상태/물리 제어, 클라이언트는 입력만 전송

### 8.3 기대 효과
- 멀티플레이어 환경에서 일관된 게임 경험
- 치트 방지 및 보안 강화
- 물리 시뮬레이션 품질 향상
- 디버깅 및 유지보수 용이성 증가
