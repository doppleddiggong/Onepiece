### **KLingo Speak Quest 기능 구현 요청**

**프로젝트 개요:**
Onepiece 프로젝트 내에서 플레이어가 NPC와 상호작용하여 음성으로 질문에 답하는 'SpeakQuest' 콘텐츠를 구현합니다. 이 기능의 핵심 액터는 `ANPCExaminer`이며, 모든 로직은 `Documents/DevLog/AgentLog/KLingo_SpeakScenario.md`에 명시된 서버 권한(Server-Authoritative) 및 턴 기반(Turn-Based) 멀티플레이 아키텍처를 따라야 합니다.

**프로젝트 코딩 컨벤션:**
- 모든 코드는 `AgentRule/Project/Onepiece/CODING_CONVENTIONS.md` 준수
- 파일 헤더: 저작권 표시 필수
- 변수명: camelCase (bool은 bPascalCase)
- Doxygen 주석: `/// @brief`, `/// @param` 스타일 사용
- 포인터: `TObjectPtr<>` 사용
- 네트워크 로깅: `NETWORK_LOG` 매크로 사용
- RPC 명명: `ServerRPC_`, `ClientRPC_` 접두사 사용

**기존 시스템 활용:**
- `APlayerActor`: InteractionSystem, VoiceConversationSystem 내장
- `APlayerControl`: Enhanced Input System 사용
- `ALingoPlayerState`: 퀘스트 데이터 저장
- `UKLingoNetworkSystem`: HTTP/WebSocket 통신
- `ANPCExaminer`: InteractComponent 기반 상호작용

**요구사항:**
아래 설명된 기능들을 C++ 코드로 구현해 주세요. 기존 시스템을 최대한 활용하고 중복 구현을 피해주세요.

---

### **1. `ANPCExaminer` 액터 구현**

`ACharacter`를 상속받는 `ANPCExaminer` 클래스에 다음 기능들을 구현합니다.

#### **1.1. 주요 프로퍼티**

```cpp
// ANPCExaminer.h
// Copyright (c) 2025 Doppleddiggong. All rights reserved.

protected:
    /// @brief 현재 상호작용 또는 심사 중인 플레이어 (서버에서만 유효)
    UPROPERTY()
    TObjectPtr<APlayerActor> targetPlayer;

    /// @brief 심사관이 현재 다른 플레이어와 상호작용 중인지 여부 (모든 클라이언트에 복제)
    UPROPERTY(Transient, ReplicatedUsing = OnRep_bIsBusy)
    bool bIsBusy;

    /// @brief 현재 심사 중인 플레이어의 이름 (UI 표기용, 모든 클라이언트에 복제)
    UPROPERTY(Transient, Replicated)
    FString busyPlayerName;

    /// @brief 플레이어를 밀어내는 힘이 적용될 영역을 나타내는 Sphere Collision
    /// @note 이미 PlayerDetectSphereComp가 존재하므로 이를 활용하거나 별도 생성
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SpeakQuest")
    TObjectPtr<USphereComponent> exclusionSphere;

    /// @brief InteractionSystem 연동을 위한 상호작용 컴포넌트
    /// @note KLingo_SpeakScenario.md의 InteractComponent 기반 상호작용 패턴 준수
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
    TObjectPtr<class UInteractComponent> interactComponent;
```

#### **1.2. 상호작용 및 상태 관리**

**중요**: `KLingo_SpeakScenario.md`의 InteractComponent 기반 아키텍처를 준수합니다.

1.  **`OnPlayerInteract(APlayerActor* InteractingPlayer)` 함수**
    *   `interactComponent->OnInteracted` 델리게이트에 바인딩되는 함수입니다.
    *   플레이어의 `InteractionSystem`이 이 NPC의 `InteractComponent`를 감지하면 자동 호출됩니다.
    *   `bIsBusy`가 `true`이면:
        *   `InteractingPlayer`의 컨트롤러(`APlayerControl`)에게 `ClientRPC_ShowToast`를 호출하여 "현재 {busyPlayerName} 유저의 심사 진행중입니다" 메시지를 띄웁니다.
    *   `bIsBusy`가 `false`이면:
        *   연결된 `SpeakStage->RequestJoinConversation(InteractingPlayer)`를 호출하여 플레이어를 대화 큐에 추가합니다.
        *   `SpeakStageActor`가 턴 기반 순서를 관리하므로 NPC는 직접 `bIsBusy`를 설정하지 않습니다.

2.  **`OnRep_bIsBusy()` 함수**
    *   `bIsBusy` 상태가 변경될 때 모든 클라이언트에서 호출됩니다.
    *   `true`일 경우: NPC 외형에 "심사 중"임을 나타내는 시각적 표시 (예: 머티리얼 변경, 이펙트 활성화, 머리 위 위젯 표시)를 활성화합니다.
    *   `false`일 경우: 시각적 표시를 비활성화합니다.
    *   NPC의 상태(예: Idle, Busy)에 따른 애니메이션 전환 로직을 여기에 추가합니다.

#### **1.3. NPC 시선 처리 및 디버깅 (Tick)**

*   `Tick` 함수 내에서 현재 발화자를 바라보도록 구현합니다.
    *   `SpeakStage`의 `GetCurrentSpeaker()`를 통해 현재 발화 중인 플레이어를 가져옵니다.
    *   현재 발화자가 유효할 때:
        *   NPC의 머리가 발화자를 부드럽게 바라보도록 `FindLookAtRotation`과 `RInterpTo`를 사용해 `ControlRotation`을 갱신합니다.
        *   `#if ENABLE_DRAW_DEBUG` 전처리기 내부에서, NPC의 머리 소켓에서 발화자의 머리 위치까지 녹색 `DrawDebugLine`을 그려 시각적으로 누구를 보고 있는지 표시합니다.

#### **1.4. 다른 플레이어 밀어내기 (서버 로직)**

*   `Tick` 함수 또는 0.5초 주기의 타이머에서 서버(`HasAuthority()`)에서만 실행합니다.
    *   `SpeakStage`의 `GetCurrentSpeaker()`를 통해 현재 발화자를 가져옵니다.
    *   현재 발화자가 있을 때:
        *   `exclusionSphere` 내에 현재 발화자가 아닌 다른 플레이어가 있는지 감지합니다.
        *   감지된 다른 플레이어가 있다면, `LaunchCharacter` 함수 등을 사용해 영역 바깥으로 부드럽게 밀어냅니다.

---

### **2. `ALingoPlayerState` 데이터 확장**

**중요**: 기존 `ALingoPlayerState`는 ReadQuest 데이터를 관리하므로, SpeakQuest 데이터를 별도로 추가합니다.

```cpp
// ALingoPlayerState.h
// Copyright (c) 2025 Doppleddiggong. All rights reserved.

/// @brief API로부터 받아온 SpeakQuest 질문/답변 데이터 구조체
USTRUCT(BlueprintType)
struct FSpeakStageQuestion
{
    GENERATED_BODY()

    /// @brief 질문 텍스트
    UPROPERTY(BlueprintReadOnly)
    FString questionText;

    /// @brief 질문 음성 파일 URL (TTS 생성 또는 사전 녹음)
    UPROPERTY(BlueprintReadOnly)
    FString questionVoiceURL;

    /// @brief 허용 가능한 답변 목록 (채점용)
    UPROPERTY(BlueprintReadOnly)
    TArray<FString> acceptableAnswers;
};

// ALingoPlayerState 클래스에 추가:

public:
    //--------------------------------------------------------------//
    // Speak Quest Functions
    //--------------------------------------------------------------//

    /// @brief SpeakQuest 질문 목록
    /// @note SpeakStageActor에서 관리하는 방식도 고려 가능
    UPROPERTY(Transient, Replicated, BlueprintReadOnly, Category = "SpeakQuest")
    TArray<FSpeakStageQuestion> speakQuestData;

    /// @brief 현재 진행 중인 질문 인덱스
    UPROPERTY(Transient, Replicated, BlueprintReadOnly, Category = "SpeakQuest")
    int32 currentSpeakQuestStep;
```

**대안**: `ASpeakStageActor`에서 플레이어별 진행 상태를 `TMap<APlayerActor*, int32>` 형태로 관리하는 것도 가능합니다.

---

### **3. SpeakQuest 진행 로직**

**중요**: `UKLingoNetworkSystem`을 활용하여 HTTP 통신을 수행합니다.

#### **3.1. `ServerRPC_BeginSpeakQuest(APlayerActor* Player)`**
*   `ASpeakStageActor` 또는 `ANPCExaminer`에 구현된 서버 전용 함수입니다.
*   **네트워크 시스템 활용**:
    ```cpp
    UKLingoNetworkSystem* NetworkSystem = UKLingoNetworkSystem::Get(GetWorld());
    if (!NetworkSystem) return;

    NETWORK_LOG(ENetworkLogType::GET, TEXT("[SpeakQuest] Requesting question data"));

    // HTTP 요청 콜백
    auto OnResponse = [this, Player](bool bSuccess, const FResponseSpeakQuest& Response)
    {
        if (bSuccess)
        {
            // PlayerState에 데이터 저장
            if (ALingoPlayerState* PS = Player->GetPlayerState<ALingoPlayerState>())
            {
                PS->speakQuestData = Response.questions;
                PS->currentSpeakQuestStep = 0;
            }

            // PlayerController에 첫 질문 전달
            if (APlayerControl* PC = Cast<APlayerControl>(Player->GetController()))
            {
                PC->ClientRPC_AskQuestion(Response.questions[0]);
            }
        }
        else
        {
            NETWORK_LOG(ENetworkLogType::ERROR, TEXT("[SpeakQuest] Failed to load questions"));
        }
    };

    NetworkSystem->RequestSpeakQuestData(OnResponse);
    ```

#### **3.2. `ClientRPC_AskQuestion(const FSpeakStageQuestion& Question)`**
*   `APlayerControl`에 구현된 클라이언트 RPC입니다.
*   **기존 시스템 활용**:
    ```cpp
    /// @brief 클라이언트에 질문을 전달하고 음성 재생
    /// @param Question [in] 질문 데이터
    UFUNCTION(Client, Reliable)
    void ClientRPC_AskQuestion(const FSpeakStageQuestion& Question);

    void APlayerControl::ClientRPC_AskQuestion_Implementation(const FSpeakStageQuestion& Question)
    {
        // UI 업데이트 (MainWidget 활용)
        APlayerActor* Player = Cast<APlayerActor>(GetPawn());
        if (!Player) return;

        UMainWidget* MainWidget = Player->GetMainWidget();
        if (MainWidget)
        {
            MainWidget->ShowSpeakQuestion(Question.questionText);
        }

        // VoiceConversationSystem 통한 음성 재생
        if (Player->VoiceConversationSystem)
        {
            Player->VoiceConversationSystem->PlayQuestionAudio(Question.questionVoiceURL);
        }
    }
    ```

---

### **4. 플레이어 컨트롤러 (`APlayerControl`) 로직**

**중요**: 기존 `IA_Record` InputAction을 활용합니다.

#### **4.1. 퀘스트 중 입력 처리**

1.  **질문 다시 듣기 (새 InputAction 추가 권장: `IA_Replay`)**
    *   새 InputAction `IA_Replay`를 추가하거나 UI 버튼을 통해 요청합니다.
    *   `ServerRPC_RequestReplayQuestion()` RPC를 `ASpeakStageActor`에 호출합니다.
    *   서버는 현재 플레이어의 `currentSpeakQuestStep`에 맞는 질문 데이터를 다시 `ClientRPC_AskQuestion`으로 보내줍니다.

2.  **음성 녹음 및 전송 (기존 `IA_Record` 활용)**
    *   **이미 구현된 `OnRecordPressed`/`OnRecordReleased` 핸들러를 확장**합니다.
    *   **누를 때**:
        *   `VoiceConversationSystem->StartRecording()` 호출
        *   UI에 "녹음 중" 표시
    *   **뗄 때**:
        *   `VoiceConversationSystem->StopRecording()` 호출
        *   녹음된 오디오 데이터를 가져와 `ServerRPC_SubmitAnswer(const TArray<uint8>& VoiceData)` RPC 호출

#### **4.2. 답변 제출 및 결과 처리**

**중요**: `UKLingoNetworkSystem`을 활용하여 STT 및 채점 API 통신을 수행합니다.

1.  **`ServerRPC_SubmitAnswer(const TArray<uint8>& VoiceData)`**
    *   `APlayerControl` 또는 `ASpeakStageActor`에서 호출하는 서버 RPC입니다.
    *   **네트워크 시스템 활용**:
    ```cpp
    UKLingoNetworkSystem* NetworkSystem = UKLingoNetworkSystem::Get(GetWorld());
    if (!NetworkSystem) return;

    NETWORK_LOG(ENetworkLogType::POST, TEXT("[SpeakQuest] Submitting answer"));

    auto OnResponse = [this, Player](bool bSuccess, const FResponseEvaluation& Response)
    {
        if (bSuccess)
        {
            // 채점 결과를 클라이언트에 전달
            if (APlayerControl* PC = Cast<APlayerControl>(Player->GetController()))
            {
                PC->ClientRPC_ShowAnswerFeedback(Response.bIsCorrect, Response.feedbackMessage);
            }
        }
        else
        {
            NETWORK_LOG(ENetworkLogType::ERROR, TEXT("[SpeakQuest] Evaluation failed"));
        }
    };

    NetworkSystem->RequestSpeakEvaluation(VoiceData, OnResponse);
    ```

2.  **`ClientRPC_ShowAnswerFeedback(bool bIsCorrect, const FString& FeedbackMessage)`**
    *   `APlayerControl`에 구현된 클라이언트 RPC입니다.
    *   **기존 토스트 시스템 활용**:
    ```cpp
    void APlayerControl::ClientRPC_ShowAnswerFeedback_Implementation(bool bIsCorrect, const FString& FeedbackMessage)
    {
        // 기존 Client_ToastMessage 활용
        Client_ToastMessage(FeedbackMessage);
    }
    ```

3.  **다음 단계로 넘어가기**
    *   피드백 확인 후, 플레이어가 "다음" 버튼을 클릭하거나 특정 키를 누르면 `ASpeakStageActor->NotifyAnswerComplete(Player)` RPC를 호출합니다.
    *   `KLingo_SpeakScenario.md`의 턴 기반 아키텍처에 따라 `SpeakStageActor`가 단계 진행을 관리합니다.

#### **4.3. 퀘스트 종료**

**중요**: `KLingo_SpeakScenario.md`의 턴 기반 아키텍처에 따라 `ASpeakStageActor`가 퀘스트 종료를 관리합니다.

*   **`ASpeakStageActor::AdvanceToNextPlayer()`**:
    *   현재 플레이어의 모든 단계가 완료되면 자동 호출됩니다.
    *   `PlayerQueue`에서 현재 플레이어를 제거하고 다음 플레이어로 전환합니다.
    *   현재 플레이어의 컨트롤러에게 `ClientRPC_ShowResultScreen()` RPC를 호출합니다.

*   **`APlayerControl::ClientRPC_ShowResultScreen()`**:
    *   최종 결과 UI를 화면에 표시합니다.
    *   **기존 MainWidget 활용**:
    ```cpp
    void APlayerControl::ClientRPC_ShowResultScreen_Implementation()
    {
        APlayerActor* Player = Cast<APlayerActor>(GetPawn());
        if (!Player) return;

        UMainWidget* MainWidget = Player->GetMainWidget();
        if (MainWidget)
        {
            MainWidget->ShowSpeakQuestResult();
        }
    }
    ```

---

## **추가 권장 사항**

### **A. 네트워크 응답 구조체 정의**

`NetworkData.h`에 다음 구조체를 추가하세요:

```cpp
/// @brief SpeakQuest 질문 데이터 API 응답
USTRUCT(BlueprintType)
struct FResponseSpeakQuest
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    TArray<FSpeakStageQuestion> questions;

    void SetFromHttpResponse(const FString& JsonString);
    void PrintData() const;
};

/// @brief 음성 평가 결과 API 응답
USTRUCT(BlueprintType)
struct FResponseEvaluation
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    bool bIsCorrect;

    UPROPERTY(BlueprintReadOnly)
    FString feedbackMessage;

    UPROPERTY(BlueprintReadOnly)
    float score;

    void SetFromHttpResponse(const FString& JsonString);
    void PrintData() const;
};
```

### **B. UKLingoNetworkSystem 확장**

```cpp
// UKLingoNetworkSystem.h

/// @brief SpeakQuest 질문 데이터 요청
/// @param Callback [in] 응답 콜백
void RequestSpeakQuestData(TFunction<void(bool, const FResponseSpeakQuest&)> Callback);

/// @brief 음성 답변 평가 요청
/// @param VoiceData [in] 녹음된 음성 데이터
/// @param Callback [in] 응답 콜백
void RequestSpeakEvaluation(const TArray<uint8>& VoiceData, TFunction<void(bool, const FResponseEvaluation&)> Callback);
```

### **C. VoiceConversationSystem 확장**

```cpp
// UVoiceConversationSystem.h

/// @brief 질문 음성 재생
/// @param AudioURL [in] 음성 파일 URL
void PlayQuestionAudio(const FString& AudioURL);

/// @brief 음성 녹음 시작
void StartRecording();

/// @brief 음성 녹음 중지 및 데이터 반환
/// @return 녹음된 오디오 데이터
TArray<uint8> StopRecording();
```

---

## **최종 체크리스트**

### **필수 확인 사항**

- [ ] 모든 파일 헤더에 저작권 표시 추가
- [ ] 변수명 camelCase 컨벤션 준수 (bool은 bPascalCase)
- [ ] `TObjectPtr<>` 사용으로 포인터 선언
- [ ] Doxygen 주석 스타일(`/// @brief`, `/// @param`) 적용
- [ ] RPC 함수명 `ServerRPC_`, `ClientRPC_` 접두사 사용
- [ ] `NETWORK_LOG` 매크로로 네트워크 로깅
- [ ] `KLingo_SpeakScenario.md` 아키텍처 준수

### **기존 시스템 통합**

- [ ] `APlayerActor::InteractionSystem` 활용
- [ ] `APlayerActor::VoiceConversationSystem` 활용
- [ ] `APlayerControl::IA_Record` InputAction 활용
- [ ] `APlayerControl::Client_ToastMessage` 활용
- [ ] `UMainWidget` UI 시스템 활용
- [ ] `UKLingoNetworkSystem` HTTP/WebSocket 통신 활용
- [ ] `ANPCExaminer::InteractComponent` 상호작용 활용

### **새로 구현할 컴포넌트**

- [ ] `ASpeakStageActor` (턴 기반 대화 관리자)
- [ ] `FSpeakStageQuestion` 구조체
- [ ] `FResponseSpeakQuest` 응답 구조체
- [ ] `FResponseEvaluation` 응답 구조체
- [ ] `UKLingoNetworkSystem::RequestSpeakQuestData()`
- [ ] `UKLingoNetworkSystem::RequestSpeakEvaluation()`
- [ ] `UVoiceConversationSystem::PlayQuestionAudio()`
- [ ] `UVoiceConversationSystem::StartRecording()`
- [ ] `UVoiceConversationSystem::StopRecording()`

---

## **구현 우선순위**

### **Phase 1: 기반 구조 (High Priority)**
1. `ASpeakStageActor` 클래스 생성 및 턴 기반 아키텍처 구현
2. `FSpeakStageQuestion` 구조체 정의
3. `ANPCExaminer` InteractComponent 연동
4. `ALingoPlayerState` SpeakQuest 데이터 추가

### **Phase 2: 네트워크 통합 (High Priority)**
1. `FResponseSpeakQuest` 응답 구조체 추가
2. `FResponseEvaluation` 응답 구조체 추가
3. `UKLingoNetworkSystem` API 메서드 확장
4. 네트워크 로깅 통합

### **Phase 3: 음성 시스템 (Medium Priority)**
1. `VoiceConversationSystem` 확장
2. `APlayerControl` 녹음 입력 핸들러 확장
3. 음성 재생 및 STT 통합

### **Phase 4: UI 및 피드백 (Medium Priority)**
1. `UMainWidget` SpeakQuest UI 추가
2. 진행률 표시 위젯
3. 토스트 메시지 통합

### **Phase 5: 테스트 및 최적화 (Low Priority)**
1. 단일/멀티플레이어 테스트
2. 턴 전환 검증
3. 네트워크 지연 시뮬레이션
4. 디버그 시각화

---

## **주요 변경 사항 요약**

| 항목 | 변경 전 (프롬프트 원본) | 변경 후 (프로젝트 적합) |
|------|----------------------|---------------------|
| **PlayerController** | `AKPlayerController` | `APlayerControl` |
| **PlayerState** | `KPlayerState` | `ALingoPlayerState` |
| **PlayerCharacter** | `APlayerCharacter` | `APlayerActor` |
| **구조체 명명** | `FSpeakQuestQuestion` | `FSpeakStageQuestion` |
| **RPC 명명** | `Client_ShowToast` | `ClientRPC_ShowToast` |
| **상호작용** | 커스텀 `OnInteract` RPC | `InteractComponent` 델리게이트 |
| **음성 시스템** | 새로운 VoiceCapture | 기존 `VoiceConversationSystem` |
| **네트워크** | `FHttpModule` 직접 사용 | `UKLingoNetworkSystem` 활용 |
| **로깅** | 일반 로깅 | `NETWORK_LOG` 매크로 |
| **변수명** | PascalCase | camelCase (bool 제외) |

---

## **참고 문서**

- **아키텍처**: `Documents/DevLog/AgentLog/KLingo_SpeakScenario.md`
- **코딩 컨벤션**: `AgentRule/Project/Onepiece/CODING_CONVENTIONS.md`
- **UE 기본 규칙**: `AgentRule/Project/ue_coding_conventions.md`
- **프로젝트 개요**: `AgentRule/Project/Onepiece/PROJECT_OVERVIEW.md`

---

**문서 버전**: v2.0 (프로젝트 적합성 개선)
**마지막 수정**: 2025-12-11
**수정자**: Claude Agent
