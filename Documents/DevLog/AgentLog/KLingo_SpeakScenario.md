# Onepiece - KLingo Speak Stage System 아키텍처 가이드

> **작성일**: 2025-12-01
> **대상**: 순차적 음성 대화 기반 학습 시스템 설계
> **프로젝트**: Onepiece (Unreal Engine 5.6)
> **시스템**: KLingo Speak Stage (입국 심사 시뮬레이션)

---

## 📋 목차

1. [아키텍처 개요](#1-아키텍처-개요)
2. [핵심 컴포넌트 역할](#2-핵심-컴포넌트-역할)
3. [NPC 생성 전략](#3-npc-생성-전략)
4. [SpeakStageSystem 초기화](#4-speakstagesystem-초기화)
5. [턴 기반 대화 시스템](#5-턴-기반-대화-시스템)
6. [네트워크 복제 흐름](#6-네트워크-복제-흐름)
7. [상호작용 흐름도](#7-상호작용-흐름도)
8. [확장성과 장점](#8-확장성과-장점)
9. [구현 체크리스트](#9-구현-체크리스트)

---

## 1. 아키텍처 개요

### 핵심 원칙 (한 문장 요약)

```
GameMode가 NPC와 SpeakStageActor를 생성 → SpeakStageActor가 턴 기반 대화를 관리
→ 한 번에 한 플레이어만 발화 → 모든 단계 완료 후 다음 플레이어로 진행
```

이 구조는 **순차적 멀티플레이 환경**에서 안정적이며, **턴 기반 학습 시스템의 표준 패턴**입니다.

### 설계 철학

- **순차적 처리**: 한 번에 한 플레이어만 대화 진행
- **서버 권한**: Server Authority로 발화 권한 관리
- **상태 공유**: SpeakStageActor가 모든 플레이어에게 상태 복제
- **인터랙션 시스템**: InteractComponent 기반 표준 상호작용

### 턴 기반 대화의 핵심

1. **Current Speaker**: 현재 발화 권한을 가진 플레이어 추적
2. **Step Progress**: 플레이어별 진행 단계 관리
3. **Server Permission**: 클라이언트는 서버 허락을 받아 발화
4. **Sequential Flow**: 한 플레이어의 모든 질문 완료 후 다음 플레이어로

---

## 2. 핵심 컴포넌트 역할

| 컴포넌트 | 역할 | 책임 범위 |
|---------|------|----------|
| **ALingoGameMode** | 게임 초기화 | BeginPlay에서 NPC 및 SpeakStageActor 생성 |
| **ASpeakStageActor** | 대화 시스템 관리자 | CurrentStepIndex 관리, 턴 제어, 상태 복제 |
| **ANPCExaminer** | 심사관 NPC | InteractComponent 소유, 질문 출력 |
| **APlayerActor** | 플레이어 캐릭터 | InteractionSystem으로 NPC와 상호작용 |
| **APlayerControl** | 입력 처리 | 음성 입력 권한 요청 및 발화 |
| **UI** | 시각적 피드백 | 현재 발화자, 진행률 표시 |

### 데이터 흐름

```
[GameMode]
  ├─ Spawns [NPC]
  └─ Creates [SpeakStageActor] ──> Manages Turn & Step
       │
       ├─ CurrentSpeaker (Replicated)
       ├─ CurrentStepIndex (Replicated)
       └─ Player Queue
```

---

## 3. NPC 생성 전략

### 3.1 생성 타이밍: `GameMode::BeginPlay()`

KLingo는 고정된 입국 심사 시나리오이므로 **BeginPlay에서 직접 생성**합니다.

**이유**:
- 모든 Subsystem이 완전히 초기화된 상태
- World와 GameState가 준비 완료
- SpeakStageActor 생성 후 즉시 NPC와 연결 가능

### 3.2 GameMode에서 NPC 및 SpeakStageActor 생성

```cpp
// LingoGameMode.h
UCLASS()
class ONEPIECE_API ALingoGameMode : public AGameModeBase
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;

    /** 생성할 NPC 클래스 */
    UPROPERTY(EditDefaultsOnly, Category = "KLingo")
    TSubclassOf<ANPCExaminer> ExaminerClass;

    /** 생성할 SpeakStageActor 클래스 */
    UPROPERTY(EditDefaultsOnly, Category = "KLingo")
    TSubclassOf<ASpeakStageActor> SpeakStageClass;

    /** 생성된 심사관 NPC */
    UPROPERTY()
    TObjectPtr<ANPCExaminer> ExaminerNPC;

    /** 생성된 SpeakStage 시스템 */
    UPROPERTY()
    TObjectPtr<ASpeakStageActor> SpeakStage;

    /** NPC 생성 위치 */
    UPROPERTY(EditDefaultsOnly, Category = "KLingo")
    FTransform ExaminerSpawnTransform;
};
```

```cpp
// LingoGameMode.cpp
void ALingoGameMode::BeginPlay()
{
    Super::BeginPlay();

    if (!HasAuthority()) return; // 서버에서만 실행

    UWorld* World = GetWorld();
    if (!World)
    {
        PRINTLOG(Error, TEXT("World is nullptr"));
        return;
    }

    // 1. SpeakStageActor 생성 (먼저 생성해야 NPC와 연결 가능)
    if (SpeakStageClass)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        SpawnParams.Name = FName(TEXT("SpeakStageActor"));

        SpeakStage = World->SpawnActor<ASpeakStageActor>(
            SpeakStageClass,
            FVector::ZeroVector,
            FRotator::ZeroRotator,
            SpawnParams
        );

        if (SpeakStage)
        {
            // 테스트 데이터 생성 (서버에서 직접 생성)
            SpeakStage->CreateTestScenarioData();
            PRINTLOG(Log, TEXT("SpeakStageActor Created"));
        }
        else
        {
            PRINTLOG(Error, TEXT("Failed to spawn SpeakStageActor"));
            return;
        }
    }

    // 2. NPC 생성
    if (ExaminerClass)
    {
        FActorSpawnParameters NPCSpawnParams;
        NPCSpawnParams.Owner = this;
        NPCSpawnParams.SpawnCollisionHandlingOverride =
            ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

        ExaminerNPC = World->SpawnActor<ANPCExaminer>(
            ExaminerClass,
            ExaminerSpawnTransform,
            NPCSpawnParams
        );

        if (ExaminerNPC)
        {
            // SpeakStage 연결
            ExaminerNPC->SetSpeakStage(SpeakStage);
            PRINTLOG(Log, TEXT("Examiner NPC Created: %s"), *ExaminerNPC->GetName());
        }
        else
        {
            PRINTLOG(Error, TEXT("Failed to spawn Examiner NPC"));
        }
    }
}
```

### 3.3 NPC에 InteractComponent 추가

```cpp
// NPCExaminer.h
UCLASS()
class ONEPIECE_API ANPCExaminer : public ACharacter
{
    GENERATED_BODY()

public:
    ANPCExaminer();

protected:
    /** 상호작용 컴포넌트 (플레이어가 이것을 감지) */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
    TObjectPtr<UInteractComponent> InteractComponent;

    /** 연결된 SpeakStage */
    UPROPERTY()
    TObjectPtr<ASpeakStageActor> SpeakStage;

public:
    /** SpeakStage 설정 (GameMode에서 호출) */
    void SetSpeakStage(ASpeakStageActor* InSpeakStage);

    /** 플레이어가 상호작용했을 때 (InteractComponent에서 호출) */
    UFUNCTION()
    void OnPlayerInteract(APlayerActor* Player);
};
```

```cpp
// NPCExaminer.cpp
ANPCExaminer::ANPCExaminer()
{
    // InteractComponent 생성
    InteractComponent = CreateDefaultSubobject<UInteractComponent>(TEXT("InteractComponent"));
    InteractComponent->SetupAttachment(RootComponent);

    // 상호작용 이벤트 바인딩
    InteractComponent->OnInteracted.AddDynamic(this, &ANPCExaminer::OnPlayerInteract);
}

void ANPCExaminer::SetSpeakStage(ASpeakStageActor* InSpeakStage)
{
    SpeakStage = InSpeakStage;
    PRINTLOG(Log, TEXT("[%s] SpeakStage Connected"), *GetName());
}

void ANPCExaminer::OnPlayerInteract(APlayerActor* Player)
{
    if (!SpeakStage || !Player)
    {
        PRINTLOG(Warning, TEXT("SpeakStage or Player is nullptr"));
        return;
    }

    // SpeakStage에 플레이어 참여 요청
    SpeakStage->RequestJoinConversation(Player);
}

---

## 4. SpeakStageSystem 초기화

### 4.1 초기화 순서

```cpp
GameMode::BeginPlay()
 ├─ [1] Spawn SpeakStageActor
 ├─ [2] CreateTestScenarioData() // 서버에서 직접 생성
 ├─ [3] Spawn NPC
 └─ [4] NPC와 SpeakStage 연결
```

### 4.2 SpeakStageActor 구조

```cpp
// SpeakStageActor.h
UCLASS()
class ONEPIECE_API ASpeakStageActor : public AActor
{
    GENERATED_BODY()

public:
    ASpeakStageActor();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
    //----------------------------------------------------------
    // Replicated Properties
    //----------------------------------------------------------

    /** 현재 대화 중인 플레이어 */
    UPROPERTY(ReplicatedUsing = OnRep_CurrentSpeaker)
    TObjectPtr<APlayerActor> CurrentSpeaker;

    /** 현재 진행 단계 (질문 인덱스) */
    UPROPERTY(ReplicatedUsing = OnRep_CurrentStepIndex)
    int32 CurrentStepIndex;

    /** 대기 중인 플레이어 큐 */
    UPROPERTY(Replicated)
    TArray<TObjectPtr<APlayerActor>> PlayerQueue;

    //----------------------------------------------------------
    // Scenario Data (Server Only)
    //----------------------------------------------------------

    /** 시나리오 질문 목록 */
    UPROPERTY()
    TArray<FString> Questions;

    //----------------------------------------------------------
    // RepNotify Functions
    //----------------------------------------------------------

    UFUNCTION()
    void OnRep_CurrentSpeaker();

    UFUNCTION()
    void OnRep_CurrentStepIndex();

public:
    //----------------------------------------------------------
    // Public Interface
    //----------------------------------------------------------

    /** 테스트 시나리오 데이터 생성 (서버에서만) */
    void CreateTestScenarioData();

    /** 플레이어가 대화 참여 요청 */
    UFUNCTION(Server, Reliable)
    void RequestJoinConversation(APlayerActor* Player);

    /** 플레이어가 발화 권한 요청 */
    UFUNCTION(Server, Reliable, WithValidation)
    void RequestSpeak(APlayerActor* Player);

    /** 플레이어 답변 완료 (다음 단계로) */
    UFUNCTION(Server, Reliable)
    void NotifyAnswerComplete(APlayerActor* Player);

private:
    /** 다음 발화자로 전환 */
    void AdvanceToNextPlayer();

    /** 다음 질문으로 진행 */
    void AdvanceStep();
};
```

### 4.3 테스트 데이터 생성 (BeginPlay에서 호출)

```cpp
// SpeakStageActor.cpp
void ASpeakStageActor::CreateTestScenarioData()
{
    if (!HasAuthority()) return;

    // 테스트용 질문 데이터 생성
    Questions.Empty();
    Questions.Add(TEXT("What is your name?"));
    Questions.Add(TEXT("Where are you from?"));
    Questions.Add(TEXT("What is the purpose of your visit?"));
    Questions.Add(TEXT("How long will you stay?"));
    Questions.Add(TEXT("Where will you be staying?"));

    CurrentStepIndex = 0;
    CurrentSpeaker = nullptr;

    PRINTLOG(Log, TEXT("Test Scenario Data Created: %d questions"), Questions.Num());
}
```

### 4.4 Replication 설정

```cpp
// SpeakStageActor.cpp
void ASpeakStageActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ASpeakStageActor, CurrentSpeaker);
    DOREPLIFETIME(ASpeakStageActor, CurrentStepIndex);
    DOREPLIFETIME(ASpeakStageActor, PlayerQueue);
}

void ASpeakStageActor::OnRep_CurrentSpeaker()
{
    // 클라이언트에서 UI 갱신
    if (CurrentSpeaker)
    {
        PRINTLOG(Log, TEXT("Current Speaker Changed: %s"), *CurrentSpeaker->GetName());
        // UI 업데이트: 현재 발화자 표시
    }
    else
    {
        PRINTLOG(Log, TEXT("No Current Speaker"));
    }
}

void ASpeakStageActor::OnRep_CurrentStepIndex()
{
    // 클라이언트에서 질문 갱신
    PRINTLOG(Log, TEXT("Step Changed: %d"), CurrentStepIndex);
    // UI 업데이트: 진행률 표시
}

---

## 5. 턴 기반 대화 시스템

### 5.1 핵심 규칙

**한 번에 한 플레이어만 발화 가능**

1. CurrentSpeaker가 설정된 플레이어만 발화 권한 보유
2. 다른 플레이어는 대기 큐(PlayerQueue)에서 대기
3. 현재 플레이어의 모든 질문(Step) 완료 후 다음 플레이어로
4. 서버가 모든 턴 전환을 관리

### 5.2 플레이어 참여 흐름

```cpp
// SpeakStageActor.cpp
void ASpeakStageActor::RequestJoinConversation_Implementation(APlayerActor* Player)
{
    if (!HasAuthority() || !Player) return;

    // 이미 큐에 있는지 확인
    if (PlayerQueue.Contains(Player))
    {
        PRINTLOG(Warning, TEXT("Player already in queue: %s"), *Player->GetName());
        return;
    }

    // 큐에 추가
    PlayerQueue.Add(Player);
    PRINTLOG(Log, TEXT("Player joined queue: %s (Position: %d)"),
             *Player->GetName(), PlayerQueue.Num());

    // 현재 발화자가 없으면 첫 번째 플레이어를 발화자로 설정
    if (!CurrentSpeaker && PlayerQueue.Num() > 0)
    {
        CurrentSpeaker = PlayerQueue[0];
        CurrentStepIndex = 0;
        PRINTLOG(Log, TEXT("First speaker assigned: %s"), *CurrentSpeaker->GetName());
    }
}
```

### 5.3 발화 권한 요청 (클라이언트 → 서버)

```cpp
// SpeakStageActor.cpp
void ASpeakStageActor::RequestSpeak_Implementation(APlayerActor* Player)
{
    if (!HasAuthority() || !Player) return;

    // 현재 발화자가 아니면 거부
    if (CurrentSpeaker != Player)
    {
        PRINTLOG(Warning, TEXT("Not your turn: %s"), *Player->GetName());
        // 클라이언트에 거부 알림 (RPC)
        return;
    }

    // 발화 권한 승인
    PRINTLOG(Log, TEXT("Speak permission granted: %s (Step: %d)"),
             *Player->GetName(), CurrentStepIndex);

    // 클라이언트에 승인 알림 (RPC)
    // Player->ClientCanSpeak();
}

bool ASpeakStageActor::RequestSpeak_Validate(APlayerActor* Player)
{
    return Player != nullptr;
}
```

### 5.4 답변 완료 → 다음 단계/플레이어

```cpp
// SpeakStageActor.cpp
void ASpeakStageActor::NotifyAnswerComplete_Implementation(APlayerActor* Player)
{
    if (!HasAuthority() || !Player) return;

    if (CurrentSpeaker != Player)
    {
        PRINTLOG(Warning, TEXT("Invalid speaker: %s"), *Player->GetName());
        return;
    }

    PRINTLOG(Log, TEXT("Answer complete: %s (Step: %d)"),
             *Player->GetName(), CurrentStepIndex);

    // 다음 단계로 진행
    AdvanceStep();
}

void ASpeakStageActor::AdvanceStep()
{
    if (!HasAuthority()) return;

    CurrentStepIndex++;

    // 모든 질문 완료?
    if (CurrentStepIndex >= Questions.Num())
    {
        PRINTLOG(Log, TEXT("All steps completed for: %s"), *CurrentSpeaker->GetName());
        // 다음 플레이어로 전환
        AdvanceToNextPlayer();
    }
    else
    {
        PRINTLOG(Log, TEXT("Advanced to step: %d"), CurrentStepIndex);
        // RepNotify로 모든 클라이언트에 전파됨
    }
}

void ASpeakStageActor::AdvanceToNextPlayer()
{
    if (!HasAuthority()) return;

    if (!CurrentSpeaker) return;

    // 현재 플레이어를 큐에서 제거
    PlayerQueue.Remove(CurrentSpeaker);
    PRINTLOG(Log, TEXT("Player completed: %s"), *CurrentSpeaker->GetName());

    // 다음 플레이어 설정
    if (PlayerQueue.Num() > 0)
    {
        CurrentSpeaker = PlayerQueue[0];
        CurrentStepIndex = 0; // 처음부터 시작
        PRINTLOG(Log, TEXT("Next speaker: %s"), *CurrentSpeaker->GetName());
    }
    else
    {
        CurrentSpeaker = nullptr;
        PRINTLOG(Log, TEXT("All players completed"));
        // 시나리오 완료 처리
    }
}
```

### 5.5 클라이언트에서 발화 요청 예시

```cpp
// LingoPlayerController.cpp
void ALingoPlayerController::OnVoiceInputReady()
{
    // SpeakStage 찾기
    ASpeakStageActor* SpeakStage = FindSpeakStageActor();
    if (!SpeakStage) return;

    APlayerActor* MyPlayer = Cast<APlayerActor>(GetPawn());
    if (!MyPlayer) return;

    // 서버에 발화 권한 요청
    SpeakStage->RequestSpeak(MyPlayer);
}
```

---

## 6. 네트워크 복제 흐름

### 6.1 Server Authority 원칙

**핵심**: 서버만 상태 변경, 클라이언트는 RepNotify로만 동기화

```
[서버]
 ├─ CurrentSpeaker 변경
 ├─ CurrentStepIndex 변경
 └─ PlayerQueue 변경

      ↓ (Replication)

[모든 클라이언트]
 ├─ OnRep_CurrentSpeaker() 호출
 ├─ OnRep_CurrentStepIndex() 호출
 └─ UI 자동 갱신
```

### 6.2 데이터 공유 방식

SpeakStageActor가 **GameMode 소유**이므로:

```cpp
// GameMode가 SpeakStage를 생성하고 소유
// 모든 클라이언트가 같은 SpeakStageActor를 참조

[Server: GameMode]
  └─ SpeakStageActor (Replicated Actor)
       │
       ├─ CurrentSpeaker (Replicated)
       ├─ CurrentStepIndex (Replicated)
       └─ PlayerQueue (Replicated)
             ↓
       [All Clients]
       자동으로 동일한 상태 유지
```

**RPC 데이터 꼬임 방지**:
- 모든 상태 변경은 서버에서만 발생
- 클라이언트는 RPC로 요청만 전송
- 상태는 Replication으로만 클라이언트에 전달

### 6.3 Replication 설정 요약

```cpp
// SpeakStageActor.h
UCLASS()
class ONEPIECE_API ASpeakStageActor : public AActor
{
    GENERATED_BODY()

public:
    ASpeakStageActor()
    {
        // 반드시 Replication 활성화
        bReplicates = true;
        bAlwaysRelevant = true; // 모든 클라이언트에 항상 전송
    }

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
    {
        Super::GetLifetimeReplicatedProps(OutLifetimeProps);

        DOREPLIFETIME(ASpeakStageActor, CurrentSpeaker);
        DOREPLIFETIME(ASpeakStageActor, CurrentStepIndex);
        DOREPLIFETIME(ASpeakStageActor, PlayerQueue);
    }

protected:
    UPROPERTY(ReplicatedUsing = OnRep_CurrentSpeaker)
    TObjectPtr<APlayerActor> CurrentSpeaker;

    UPROPERTY(ReplicatedUsing = OnRep_CurrentStepIndex)
    int32 CurrentStepIndex;

    UPROPERTY(Replicated)
    TArray<TObjectPtr<APlayerActor>> PlayerQueue;
};
```

### 6.4 클라이언트 → 서버 RPC 패턴

```cpp
// 패턴 1: Server RPC (검증 필요)
UFUNCTION(Server, Reliable, WithValidation)
void RequestSpeak(APlayerActor* Player);

bool RequestSpeak_Validate(APlayerActor* Player)
{
    return Player != nullptr;
}

void RequestSpeak_Implementation(APlayerActor* Player)
{
    // 서버에서만 실행
    if (CurrentSpeaker == Player)
    {
        // 승인 처리
    }
}

// 패턴 2: Server RPC (검증 없음)
UFUNCTION(Server, Reliable)
void RequestJoinConversation(APlayerActor* Player);

void RequestJoinConversation_Implementation(APlayerActor* Player)
{
    // 서버에서만 실행
    PlayerQueue.Add(Player);
}
```

### 6.5 데이터 무결성 보장

| 상황 | 처리 방식 |
|------|-----------|
| 클라이언트 A가 발화 요청 | Server RPC로 서버에 요청 전송 |
| 서버가 CurrentSpeaker 확인 | A가 맞으면 승인, 아니면 거부 |
| 서버가 Step 진행 | CurrentStepIndex++ (서버에서만) |
| 모든 클라이언트 동기화 | OnRep_CurrentStepIndex() 자동 호출 |
| 다음 플레이어로 전환 | CurrentSpeaker 변경 (서버에서만) |
| 모든 클라이언트 UI 갱신 | OnRep_CurrentSpeaker() 자동 호출 |

**중요**: 클라이언트는 절대 직접 변경하지 않음 → RPC 꼬임 방지

---

## 7. 상호작용 흐름도

### 7.1 전체 흐름 다이어그램

```
1. 플레이어가 NPC와 상호작용
   [PlayerActor]
     → Uses InteractionSystem
       → [NPC InteractComponent] Detected
         → [NPC] OnPlayerInteract()
           → [SpeakStage] RequestJoinConversation(Player)

2. 서버가 플레이어를 큐에 추가
   [Server: SpeakStage]
     → PlayerQueue.Add(Player)
     → If (No CurrentSpeaker):
         → CurrentSpeaker = Player
         → CurrentStepIndex = 0
     → Replicate to All Clients

3. 현재 발화자 클라이언트에서 음성 입력 시작
   [Client: CurrentSpeaker]
     → User Speaks
     → Voice Input Detected
     → [PlayerController] Request Speak Permission
       → [SpeakStage] RequestSpeak(Player) [RPC]

4. 서버가 발화 권한 검증
   [Server: SpeakStage]
     → If (CurrentSpeaker == Player):
         → Grant Permission
         → Client Can Speak Now
     → Else:
         → Deny (Not Your Turn)

5. 음성 인식 및 답변 처리
   [Client: CurrentSpeaker]
     → STT Processing
     → Answer Validated
     → [SpeakStage] NotifyAnswerComplete(Player) [RPC]

6. 서버가 다음 단계 진행
   [Server: SpeakStage]
     → CurrentStepIndex++
     → If (All Steps Done):
         → AdvanceToNextPlayer()
         → CurrentSpeaker = NextPlayer
         → CurrentStepIndex = 0
     → Else:
         → Continue Same Player
     → Replicate to All Clients

7. 모든 클라이언트 UI 갱신
   [All Clients]
     → OnRep_CurrentSpeaker()
       → Update UI: Current Speaker Name
     → OnRep_CurrentStepIndex()
       → Update UI: Progress Bar
       → Display Next Question
```

### 7.2 InteractionSystem 기반 상호작용

```cpp
// PlayerActor에서 Interaction 사용
void APlayerActor::TryInteract()
{
    // InteractionSystem이 주변 InteractComponent 스캔
    UInteractComponent* NearestInteract = InteractionSystem->FindNearestInteract();

    if (NearestInteract)
    {
        // InteractComponent에 상호작용 신호 전송
        NearestInteract->OnInteracted.Broadcast(this);
    }
}

// NPCExaminer의 InteractComponent가 이벤트 수신
void ANPCExaminer::OnPlayerInteract(APlayerActor* Player)
{
    // SpeakStage에 플레이어 참여 요청
    SpeakStage->RequestJoinConversation(Player);
}
```

### 7.3 순차적 턴 진행 예시

**시나리오**: Player A, B, C가 차례로 참여

```
Step 1: Player A 참여
  → PlayerQueue = [A]
  → CurrentSpeaker = A
  → CurrentStepIndex = 0

Step 2: Player B 참여
  → PlayerQueue = [A, B]
  → CurrentSpeaker = A (변경 없음)

Step 3: Player C 참여
  → PlayerQueue = [A, B, C]
  → CurrentSpeaker = A (변경 없음)

Step 4: Player A가 5개 질문 모두 완료
  → A 제거, PlayerQueue = [B, C]
  → CurrentSpeaker = B
  → CurrentStepIndex = 0 (B는 처음부터)

Step 5: Player B가 5개 질문 모두 완료
  → B 제거, PlayerQueue = [C]
  → CurrentSpeaker = C
  → CurrentStepIndex = 0

Step 6: Player C가 5개 질문 모두 완료
  → C 제거, PlayerQueue = []
  → CurrentSpeaker = nullptr
  → All Players Completed!
```

### 7.4 발화 권한 체크 흐름

```cpp
// 클라이언트에서 발화 시도
void ALingoPlayerController::OnVoiceInputDetected()
{
    APlayerActor* MyPlayer = Cast<APlayerActor>(GetPawn());
    ASpeakStageActor* Stage = GetSpeakStage();

    // 1. 내 턴인지 로컬 체크 (UI 피드백용)
    if (Stage->GetCurrentSpeaker() != MyPlayer)
    {
        // UI: "Not your turn" 메시지 표시
        return;
    }

    // 2. 서버에 발화 권한 요청
    Stage->RequestSpeak(MyPlayer);
}

// 서버에서 검증
void ASpeakStageActor::RequestSpeak_Implementation(APlayerActor* Player)
{
    if (CurrentSpeaker != Player)
    {
        PRINTLOG(Warning, TEXT("Rejected: Not your turn"));
        // ClientRPC로 거부 알림 (선택사항)
        return;
    }

    // 승인
    PRINTLOG(Log, TEXT("Speak permission granted"));
    // ClientRPC로 승인 알림 (선택사항)
}
```

---

## 8. 확장성과 장점

### 8.1 아키텍처의 강점

| 측면 | 설명 |
|-----|------|
| **턴 기반 제어** | 한 번에 한 플레이어만 발화하여 음성 인식 충돌 방지 |
| **멀티플레이 안정성** | Server Authority + Replication으로 RPC 꼬임 방지 |
| **공유 데이터 관리** | SpeakStageActor가 모든 상태를 중앙 관리 |
| **InteractionSystem 통합** | 표준 상호작용 시스템으로 확장 용이 |
| **테스트 데이터 생성** | 서버에서 직접 생성하여 빠른 프로토타입 가능 |

### 8.2 왜 이 구조가 필요한가?

#### 문제: 동시 발화 충돌

```
❌ 잘못된 구조:
  Player A, B, C가 동시에 발화
    → 서버에 동시에 STT 결과 전송
    → CurrentStepIndex 충돌
    → 누구의 답변이 먼저 처리될지 불명확
```

#### 해결: 턴 기반 순차 처리

```
✅ 올바른 구조:
  CurrentSpeaker = A
    → A만 발화 가능
    → A의 모든 Step 완료
    → CurrentSpeaker = B
    → B만 발화 가능
```

### 8.3 확장 가능성

#### 1. 새로운 시나리오 타입 추가

```cpp
// 공항 체크인, 병원 접수 등 추가 가능
enum class ESpeakStageType : uint8
{
    Immigration,    // 입국 심사
    Airport,        // 공항 체크인
    Hospital,       // 병원 접수
    Restaurant      // 레스토랑 주문
};

// SpeakStageActor에 타입 추가
UPROPERTY(EditDefaultsOnly)
ESpeakStageType StageType;
```

#### 2. 난이도별 질문 세트

```cpp
// SpeakStageActor.cpp
void ASpeakStageActor::CreateTestScenarioData()
{
    switch (DifficultyLevel)
    {
    case EDifficulty::Easy:
        Questions.Add(TEXT("What is your name?"));
        Questions.Add(TEXT("Where are you from?"));
        break;

    case EDifficulty::Hard:
        Questions.Add(TEXT("Can you explain the purpose of your extended stay?"));
        Questions.Add(TEXT("What documentation supports your visit?"));
        break;
    }
}
```

#### 3. 다중 NPC 지원

```cpp
// GameMode에서 여러 심사대 생성
for (int32 i = 0; i < NumExaminationBooths; i++)
{
    ASpeakStageActor* Booth = SpawnSpeakStage();
    ANPCExaminer* Examiner = SpawnExaminer();
    Examiner->SetSpeakStage(Booth);
}

// 플레이어는 가장 가까운 심사대 선택
```

---

## 9. 구현 체크리스트

### Phase 1: 기본 구조 (GameMode & Actor)

- [ ] `ALingoGameMode` 클래스 생성
- [ ] `ASpeakStageActor` 클래스 생성
- [ ] `ANPCExaminer` 클래스 생성
- [ ] GameMode에서 BeginPlay 구현
- [ ] SpeakStageActor 생성 및 초기화
- [ ] NPC 생성 및 SpeakStage 연결

### Phase 2: SpeakStageActor 핵심 로직

- [ ] Replication 설정 (`bReplicates = true`)
- [ ] `CurrentSpeaker` Replicated 변수 추가
- [ ] `CurrentStepIndex` Replicated 변수 추가
- [ ] `PlayerQueue` Replicated 변수 추가
- [ ] `OnRep_CurrentSpeaker()` 구현
- [ ] `OnRep_CurrentStepIndex()` 구현
- [ ] `CreateTestScenarioData()` 구현

### Phase 3: RPC 구현

- [ ] `RequestJoinConversation` Server RPC
- [ ] `RequestSpeak` Server RPC (WithValidation)
- [ ] `NotifyAnswerComplete` Server RPC
- [ ] `AdvanceStep()` 로직 구현
- [ ] `AdvanceToNextPlayer()` 로직 구현

### Phase 4: NPC 상호작용

- [ ] NPC에 `UInteractComponent` 추가
- [ ] `OnPlayerInteract()` 이벤트 바인딩
- [ ] NPC가 현재 질문 표시 로직
- [ ] NPC 애니메이션/사운드 연동

### Phase 5: PlayerController 통합

- [ ] PlayerController에 SpeakStage 참조 추가
- [ ] 음성 입력 감지 시 `RequestSpeak()` 호출
- [ ] STT 완료 시 `NotifyAnswerComplete()` 호출
- [ ] 발화 권한 거부 시 UI 피드백

### Phase 6: UI 구현

- [ ] 현재 발화자 이름 표시 위젯
- [ ] 대기 큐 표시 (몇 번째 대기 중)
- [ ] 진행률 바 (CurrentStepIndex / TotalSteps)
- [ ] 현재 질문 표시
- [ ] "Not Your Turn" 메시지 표시

### Phase 7: 테스트 & 디버깅

- [ ] 단일 플레이어 테스트
- [ ] 멀티플레이어 테스트 (2~3명)
- [ ] 턴 전환 검증
- [ ] 네트워크 지연 시뮬레이션 테스트
- [ ] PRINTLOG로 디버그 출력 확인

---

## 참고 자료

- [UE Network Replication](https://docs.unrealengine.com/5.4/en-US/networking-and-multiplayer-in-unreal-engine/)
- [Server RPC Best Practices](https://docs.unrealengine.com/5.4/en-US/rpcs-in-unreal-engine/)
- [Actor Replication](https://docs.unrealengine.com/5.4/en-US/actor-replication-in-unreal-engine/)
- [Turn-Based Multiplayer Patterns](https://www.unrealengine.com/en-US/blog/multiplayer-programming-patterns)

---

**마지막 업데이트**: 2025-12-01
**작성자**: Claude Agent
**프로젝트**: Onepiece - KLingo Speak Stage
**버전**: v2.0 (Turn-Based Architecture)