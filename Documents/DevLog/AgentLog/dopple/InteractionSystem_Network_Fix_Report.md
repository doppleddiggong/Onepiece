# Interaction System 네트워크 문제 해결 보고서

**작성자**: dopple
**작성일**: 2025-11-25
**관련 시스템**: UInteractionSystem, UInteractableComponent, ALuggage
**문제 유형**: 네트워크 동기화 이슈

---

## 📋 Executive Summary

Guest 플레이어가 ALuggage 객체를 Pickup/Drop 할 수 없는 문제를 해결했습니다. 문제의 근본 원인은 네트워크 환경에서 `DeprojectScreenPositionToWorld()` 함수가 불안정하게 작동하여 LineTrace가 실패한 것이었습니다.

**해결 방법**: 카메라 Transform을 직접 사용하는 방식으로 변경하여 Host/Guest 모두에서 안정적으로 작동하도록 수정했습니다.

---

## 🔍 문제 상황

### 증상
- **Host 플레이어**: ALuggage 객체를 정상적으로 Pickup/Drop 가능
- **Guest 플레이어**: ALuggage 객체와 상호작용 불가능
  - `CurrentTarget`이 nullptr로 설정됨
  - ShowDebugInfo는 표시되지만 실제 Pickup 시도 시 실패

### 재현 조건
- Listen Server 환경
- Guest 클라이언트가 Luggage 객체에 접근하여 상호작용 시도

---

## 🔎 원인 분석

### 1단계: 초기 가설 검증

**가설 1**: 네트워크 복제 문제
- `UInteractableComponent`의 `HoldingOwner` 변수는 올바르게 복제되도록 설정됨
- `ALuggage`도 `bReplicates = true` 설정 확인
- ❌ 복제 설정은 정상

**가설 2**: DetectionRange 컴포넌트 복제 문제
- 초기 코드: `BeginPlay()`에서 `NewObject<UBoxComponent>()` 사용
- 런타임 생성 컴포넌트는 자동 복제되지 않음
- ⚠️ 부분적으로 문제 발견

### 2단계: DetectionRange 수정 시도

**시도 1**: 생성자에서 `CreateDefaultSubobject` 사용
```cpp
UInteractableComponent::UInteractableComponent()
{
    DetectionRange = CreateDefaultSubobject<UBoxComponent>(TEXT("DetectionRange"));
}
```

**결과**: Host도 반응 안 함
**원인**: ActorComponent의 생성자에서는 Owner에 접근 불가
- `GetOwner()` 호출 시 nullptr 반환
- `RegisterComponent()` 및 `AttachToComponent()` 누락

**시도 2**: BeginPlay에서 RegisterComponent + AttachToComponent 추가
```cpp
void UInteractableComponent::BeginPlay()
{
    if (DetectionRange)
    {
        DetectionRange->RegisterComponent();
        DetectionRange->AttachToComponent(Owner->GetRootComponent(), ...);
    }
}
```

**결과**: DetectionRange는 정상 작동하지만 Guest는 여전히 상호작용 불가

### 3단계: 실제 원인 발견

**핵심 발견**: `DetectInteractableTarget()` 분석
- Tick에서 호출 시: ✅ CurrentTarget 정상 설정 (ShowDebugInfo 표시됨)
- TryPickUp에서 호출 시: ❌ `PerformCenterLineTrace()` 실패

**문제 코드** (UInteractionSystem.cpp:228):
```cpp
bool UInteractionSystem::PerformCenterLineTrace(FHitResult& OutHit)
{
    APlayerController* PC = Cast<APlayerController>(OwnerPlayer->GetController());

    int32 ViewportSizeX, ViewportSizeY;
    PC->GetViewportSize(ViewportSizeX, ViewportSizeY);  // ⚠️ Guest에서 (0, 0) 반환

    FVector WorldLocation, WorldDirection;
    if (!PC->DeprojectScreenPositionToWorld(ScreenX, ScreenY, WorldLocation, WorldDirection))
        return false;  // ⚠️ ViewportSize가 0이면 실패!
}
```

**근본 원인**:
- Listen Server 환경에서 Guest 클라이언트의 `GetViewportSize()`가 불안정
- ViewportSize가 (0, 0) 또는 유효하지 않은 값 반환
- `DeprojectScreenPositionToWorld()` 함수 내부적으로 ViewportSize 사용
- ViewportSize가 0이면 Deproject 실패 → LineTrace 실패 → CurrentTarget nullptr

---

## ✅ 해결 방법

### 최종 해결책: 카메라 Transform 직접 사용

**수정된 코드** (UInteractionSystem.cpp):

```cpp
bool UInteractionSystem::PerformCenterLineTrace(FHitResult& OutHit)
{
    if (!OwnerPlayer)
        return false;

    APlayerController* PC = Cast<APlayerController>(OwnerPlayer->GetController());
    if (!PC)
        return false;

    // ✅ 카메라 매니저에서 직접 위치/방향 가져오기
    APlayerCameraManager* CameraManager = PC->PlayerCameraManager;
    if (!CameraManager)
        return false;

    FVector CameraLocation = CameraManager->GetCameraLocation();
    FVector CameraForward = CameraManager->GetCameraRotation().Vector();

    // 레이 트레이스 시작/끝 지점
    FVector TraceStart = CameraLocation;
    FVector TraceEnd = TraceStart + (CameraForward * InteractionDistance);

    // Ray trace 실행
    bool bHit = GetWorld()->LineTraceSingleByChannel(
        OutHit, TraceStart, TraceEnd, ECC_Visibility
    );

    // 디버그 라인
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

### 변경 사항

**Before**:
- `GetViewportSize()` → 화면 중앙 계산 → `DeprojectScreenPositionToWorld()` 사용
- 네트워크 환경에서 불안정

**After**:
- `PlayerCameraManager`에서 직접 위치/회전 정보 사용
- ViewportSize에 의존하지 않음
- 각 클라이언트의 로컬 카메라 정보 사용

---

## 🎯 기술적 이점

### 1. 네트워크 독립성
- 각 클라이언트가 자신의 로컬 카메라 정보 사용
- Server/Client 구분 없이 동일한 로직 작동

### 2. 안정성 향상
- ViewportSize 유효성 검증 불필요
- `DeprojectScreenPositionToWorld()`의 내부 복잡성 우회
- 네트워크 지연/타이밍 이슈에 강함

### 3. 코드 간결성
- 화면 좌표 계산 단계 제거
- 카메라 Transform만으로 Ray 계산 가능
- 디버깅 용이

### 4. 성능
- Deproject 연산 제거로 약간의 성능 향상
- 직접 계산이 더 효율적

---

## 📝 추가 수정 사항

### DetectionRange 생성 방식 개선

**기존 문제**:
```cpp
// BeginPlay에서 NewObject 사용 - 복제 안 됨!
DetectionRange = NewObject<UBoxComponent>(Owner, ...);
```

**개선된 방식**:
```cpp
// 생성자에서 CreateDefaultSubobject - 자동 복제됨
UInteractableComponent::UInteractableComponent()
{
    DetectionRange = CreateDefaultSubobject<UBoxComponent>(TEXT("DetectionRange"));
    DetectionRange->SetBoxExtent(FVector(150.0f));
    DetectionRange->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    DetectionRange->SetCollisionResponseToAllChannels(ECR_Ignore);
    DetectionRange->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void UInteractableComponent::BeginPlay()
{
    if (DetectionRange)
    {
        // World에 등록
        if (!DetectionRange->IsRegistered())
            DetectionRange->RegisterComponent();

        // Owner에 부착
        if (!DetectionRange->IsAttachedTo(Owner->GetRootComponent()))
            DetectionRange->AttachToComponent(Owner->GetRootComponent(), ...);

        // Overlap 콜백 바인딩
        DetectionRange->OnComponentBeginOverlap.AddDynamic(...);
        DetectionRange->OnComponentEndOverlap.AddDynamic(...);
    }
}
```

**개선 효과**:
- ✅ DetectionRange가 모든 클라이언트에 복제됨
- ✅ Overlap 이벤트가 Host/Guest 모두에서 작동
- ✅ NearbyInteractables 배열이 올바르게 관리됨

---

## 🧪 테스트 결과

### 테스트 환경
- **네트워크 모드**: Listen Server
- **플레이어 수**: Host 1명 + Guest 1명
- **테스트 객체**: ALuggage

### 테스트 케이스

| 케이스 | Host | Guest | 결과 |
|--------|------|-------|------|
| Luggage Pickup | ✅ | ✅ | Pass |
| Luggage Drop | ✅ | ✅ | Pass |
| DetectionRange Overlap | ✅ | ✅ | Pass |
| LineTrace Detection | ✅ | ✅ | Pass |
| ShowDebugInfo | ✅ | ✅ | Pass |

### 검증 항목
- [x] Guest가 Luggage를 집을 수 있음
- [x] Guest가 Luggage를 놓을 수 있음
- [x] Host와 Guest 간 상태 동기화 정상
- [x] 물리 시뮬레이션 정상 작동
- [x] Collision 설정 복원 정상

---

## 📚 학습 포인트

### 1. Unreal Engine 네트워크 아키텍처

**ViewportSize의 네트워크 제약**:
- `GetViewportSize()`는 로컬 플레이어의 화면 정보를 반환
- Listen Server에서 원격 클라이언트의 ViewportSize는 부정확할 수 있음
- 네트워크 기반 게임에서는 화면 좌표 의존적인 로직 지양 필요

**권장 패턴**:
- 화면 좌표 대신 월드 좌표/Transform 사용
- 각 클라이언트가 로컬 정보만 사용하도록 설계
- Server RPC는 월드 좌표 기반으로 전달

### 2. 컴포넌트 생성과 복제

**생성 시점에 따른 복제 동작**:
- **생성자 (`CreateDefaultSubobject`)**: 자동 복제됨
- **BeginPlay (`NewObject`)**: 복제 안 됨 (Server-only)
- **런타임 동적 생성**: 별도 복제 로직 필요

**ActorComponent vs Actor**:
- ActorComponent 생성자에서는 `GetOwner()` 사용 불가
- Owner는 Component가 Actor에 추가된 후에 설정됨
- `RegisterComponent()` + `AttachToComponent()`는 BeginPlay에서 수행

### 3. 디버깅 전략

**문제 해결 과정**:
1. 네트워크 복제 설정 확인
2. 컴포넌트 생성/등록 확인
3. 실행 시점별 동작 비교 (Tick vs Input)
4. 로그 분석으로 정확한 실패 지점 파악

**유용한 로그 포인트**:
- Overlap 이벤트 발생 여부
- LineTrace 성공/실패
- ViewportSize 값
- Controller/Camera 유효성

---

## 🔧 관련 파일

### 수정된 파일
- `Source/Onepiece/Character/Private/UInteractionSystem.cpp`
  - `PerformCenterLineTrace()` 함수 수정
- `Source/Onepiece/Interactable/Private/InteractableComponent.cpp`
  - 생성자 및 BeginPlay 수정
- `Source/Onepiece/Interactable/Public/InteractableComponent.h`
  - DetectionRange 선언 유지

### 영향받는 시스템
- UInteractionSystem
- UInteractableComponent
- ALuggage
- APlayerActor (간접적)

---

## ⚠️ 주의사항

### 향후 유사 이슈 방지

1. **화면 좌표 사용 지양**
   - `DeprojectScreenPositionToWorld()` 사용 시 네트워크 환경 고려
   - 가능하면 카메라 Transform 직접 사용

2. **컴포넌트 동적 생성 주의**
   - 런타임 생성 컴포넌트는 복제 설정 필수
   - 생성자에서 생성 가능한지 먼저 검토

3. **네트워크 테스트 필수**
   - Host/Guest 양쪽에서 모두 테스트
   - Listen Server와 Dedicated Server 환경 차이 고려

### 알려진 제약사항

- 현재 구현은 PlayerCameraManager 의존
- VR/멀티 뷰포트 환경에서는 추가 고려 필요
- Camera가 없는 Spectator 모드는 미지원

---

## 🎓 참고 자료

### Unreal Engine Documentation
- [Components and Collision](https://docs.unrealengine.com/en-US/InteractiveExperiences/Networking/Actors/Components/)
- [Replication](https://docs.unrealengine.com/en-US/InteractiveExperiences/Networking/Actors/Properties/)
- [PlayerCameraManager](https://docs.unrealengine.com/en-US/API/Runtime/Engine/Camera/APlayerCameraManager/)

### 프로젝트 문서
- `AgentRule/Project/Onepiece/AGENT_GUIDE.md`
- `AgentRule/Project/ue_coding_conventions.md`

---

## 📞 후속 조치

### 완료된 작업
- [x] Guest 상호작용 문제 해결
- [x] DetectionRange 복제 개선
- [x] LineTrace 로직 안정화
- [x] 네트워크 환경 테스트

### 권장 추가 작업
- [ ] Dedicated Server 환경 테스트
- [ ] 3명 이상 멀티플레이 테스트
- [ ] 다른 Interactable 객체 타입 검증
- [ ] 성능 프로파일링 (많은 Interactable 존재 시)

### 개선 고려사항
- [ ] Interaction System 문서화
- [ ] Unit Test 작성
- [ ] Network Profiler 분석
- [ ] Blueprint 노출 함수 추가 (디자이너 편의성)

---

**문서 종료**

작성자: dopple
최종 수정: 2025-11-25
문서 버전: 1.0
