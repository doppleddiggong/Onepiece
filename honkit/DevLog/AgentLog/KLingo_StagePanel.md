# KLingo Step1(Read) - Quest System Implementation Spec

## 프로젝트 정보 (Project Information)

### 개요 (Overview)
당신은 Unreal Engine 5.4 + C++ 기반 멀티플레이 게임 **Onepiece** 프로젝트의 아키텍트 겸 시니어 엔지니어입니다.

**목표**: Step1(Read) 퀘스트 시스템 + UI 위젯 + 판정 로직을 UE C++ 코드로 구현합니다.

### 프로젝트 환경 및 제약 (Environment & Constraints)

#### 기술 스택 (Tech Stack)
- **엔진**: Unreal Engine 5.4
- **언어**: C++17
- **네트워크**: Unreal 멀티플레이 (Dedicated / Listen 모두 지원)
- **UI**: UMG (C++ 기반 UUserWidget 확장)
- **모듈**: `ONEPIECE_API` 매크로 사용
- **로그**: `LogOnepiece` 카테고리 사용

#### 네트워크 권한 분리 (Network Authority)
- **GameMode**: 서버 전용 (Authority), 게임 룰 및 스테이지 진행 관리
- **GameState**: 복제 데이터 (Replicated), 모든 클라이언트가 공유하는 게임 상태
- **PlayerState**: 플레이어별 데이터 (Replicated), 각 플레이어의 선택 상태
- **Widget**: 클라이언트 전용, UI 표시 및 입력 처리

#### RPC 규칙 (RPC Rules)
- 서버에서만 변경 가능한 값: `Server RPC` → `GameState/PlayerState Replicated 변수`
- UI 입력: `클라이언트` → `Server RPC` → `GameState/PlayerState 반영` → `OnRep` → `위젯 업데이트`

---

## 기존 코드베이스 (Existing Codebase)

### 이미 존재하는 클래스 (Existing Classes)

#### 1. ALingoGameState
**위치**: `Source/Onepiece/Game/Public/ALingoGameState.h`

**주요 멤버**:
```cpp
// 타이머 관련
UPROPERTY(Replicated)
float RemainMissionTime;

UPROPERTY(Replicated)
bool bIsTimerActive;

// 시나리오 데이터
UPROPERTY(Replicated)
int32 ScenarioIndex;

UPROPERTY(Replicated)
int32 StageIndex;

UPROPERTY(Replicated)
int32 ScenarioLevel;

UPROPERTY(Replicated)
FResponseScenario CurScenarioData;
```

**주요 메서드**:
- `void SetStageData(int InStageIndex, const FResponseScenario& InResponseData)`
- `void StartMissionTimer(float TimeLimit)`
- `void StopMissionTimer()`
- `float GetRemainMissionTime()`

#### 2. ALingoPlayerState
**위치**: `Source/Onepiece/Game/Public/ALingoPlayerState.h`

**현재 상태**: 매우 기본적인 구조 (Token, UserName만 존재)

**필요한 확장**:
- Step1(Read)에서 플레이어의 선택 상태 (Symbol, Color)
- 싱글/멀티 플레이 시 역할 (Role) 구분
- 정답/오답 판정 결과

#### 3. ALingoGameMode
**위치**: `Source/Onepiece/Game/Public/ALingoGameMode.h`

**현재 상태**: 거의 비어있음

**필요한 확장**:
- Step1(Read) 시작 로직
- 캐리어 선택 처리 (정답/오답 판정)
- 타이머 패널티 적용

#### 4. APlayerActor
**위치**: `Source/Onepiece/Character/Public/APlayerActor.h`

**주요 컴포넌트**:
- `UInteractionSystem* InteractionSystem`: 캐리어와의 상호작용에 활용
- `UMainWidget* MainWidget`: 메인 UI 위젯

#### 5. UMainWidget
**위치**: `Source/Onepiece/UI/Public/UMainWidget.h`

**주요 컴포넌트**:
- `UPlayTimer* PlayTimer`: 타이머 표시 위젯
- `UStateWidget* StateWidget`: 플레이어 상태 표시 위젯
- `ALingoGameState* CachedGameState`: GameState 참조

---

## 데이터 구조 (Data Structures)

### 기존 데이터 구조 (Existing Structures)
**위치**: `Source/Onepiece/Network/Public/NetworkData.h`

#### 1. FWordData
```cpp
USTRUCT(BlueprintType)
struct FWordData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Word")
    FString Kor;  // 한국어

    UPROPERTY(BlueprintReadWrite, Category = "Word")
    FString Eng;  // 영어

    UPROPERTY(BlueprintReadWrite, Category = "Word")
    FString Pronunciation;  // 발음
};
```

#### 2. FScenarioTargetData
```cpp
USTRUCT(BlueprintType)
struct FScenarioTargetData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Scenario")
    FString symbol;  // 심볼 (문제1)

    UPROPERTY(BlueprintReadWrite, Category = "Scenario")
    FString color;   // 색상 (문제2)
};
```

#### 3. FResponseScenario
```cpp
USTRUCT(BlueprintType)
struct FResponseScenario
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Scenario")
    int32 index;

    UPROPERTY(BlueprintReadWrite, Category = "Scenario")
    int32 dificulity;

    UPROPERTY(BlueprintReadWrite, Category = "Scenario")
    TArray<FScenarioTargetData> target_data;  // 선택지 풀

    UPROPERTY(BlueprintReadWrite, Category = "Scenario")
    int32 correct_answer_index;

    UPROPERTY(BlueprintReadWrite, Category = "Scenario")
    FWordData word_data1;  // 문제1 정답

    UPROPERTY(BlueprintReadWrite, Category = "Scenario")
    FWordData word_data2;  // 문제2 정답

    UPROPERTY(BlueprintReadWrite, Category = "Scenario")
    FWordData full_data;   // 전체 정답 (문제1 + 문제2)
};
```

### 추가 필요 데이터 구조 (New Structures Needed)

#### 1. EReadQuestRole (플레이어 역할)
```cpp
UENUM(BlueprintType)
enum class EReadQuestRole : uint8
{
    Both           UMETA(DisplayName = "Both"),          // 싱글: 문제1, 2 모두 조작
    OnlyQuestion1  UMETA(DisplayName = "OnlyQuestion1"), // 멀티: 문제1만 조작
    OnlyQuestion2  UMETA(DisplayName = "OnlyQuestion2")  // 멀티: 문제2만 조작
};
```

#### 2. FReadQuestResult (플레이어 기록)
```cpp
USTRUCT(BlueprintType)
struct FReadQuestResult
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Quest")
    bool bSuccess = false;  // 성공 여부

    UPROPERTY(BlueprintReadWrite, Category = "Quest")
    float RemainTime = 0.f;  // 남은 시간

    UPROPERTY(BlueprintReadWrite, Category = "Quest")
    int32 AttemptCount = 0;  // 시도 횟수

    UPROPERTY(BlueprintReadWrite, Category = "Quest")
    FString SelectedSymbol;  // 선택한 심볼

    UPROPERTY(BlueprintReadWrite, Category = "Quest")
    FString SelectedColor;   // 선택한 색상
};
```

---

## 게임 시나리오 (Game Scenario)

### Step1(Read) 퀘스트 흐름 (Quest Flow)

#### 1. 퀘스트 시작 (Quest Start)
1. 플레이어는 "ReadQuest 미션 가이드 팝업"을 봅니다.
2. 팝업에는 다음 정보가 표시됩니다:
   - 문제1 선택 영역 (심볼 리스트)
   - 문제2 선택 영역 (색상 리스트)
   - 남은 시간

#### 2. 문제 선택 (Answer Selection)
**문제1 (심볼 선택)**:
- `FScenarioTargetData` 배열에서 `symbol` 필드가 존재하는 항목만 필터링
- `symbol` 문자열 기준으로 중복 제거
- 세로 스크롤 뷰(ScrollBox)에 버튼/엔트리로 노출
- 플레이어가 하나 선택하면:
  - 로컬 UI 하이라이트
  - 서버 RPC 호출 → PlayerState에 `SelectedSymbol` 저장
  - Replicated 변수 동기화

**문제2 (색상 선택)**:
- `FScenarioTargetData` 배열에서 `color` 필드가 존재하는 항목만 필터링
- `color` 문자열 기준으로 중복 제거
- 세로 스크롤 뷰에 버튼/엔트리로 노출
- 플레이어가 하나 선택하면:
  - 로컬 UI 하이라이트
  - 서버 RPC 호출 → PlayerState에 `SelectedColor` 저장
  - Replicated 변수 동기화

#### 3. 캐리어 선택 (Carrier Interaction)
- 플레이어가 맵에 배치된 캐리어(수하물 - `Aluggage`)를 선택합니다.
- `APlayerActor`의 `InteractionSystem`을 활용하여 상호작용합니다.
- 캐리어 선택 시 서버 RPC 호출 → GameMode에서 정답 판정

#### 4. 정답 판정 (Answer Validation)
**정답 조건**:
- 플레이어가 선택한 `SelectedSymbol` == 캐리어의 `Symbol`
- 플레이어가 선택한 `SelectedColor` == 캐리어의 `Color`
- 두 조건 모두 만족 시 정답

**오답 조건**:
- 하나라도 다르면 오답

---

## 싱글 vs 멀티 플레이 규칙 (Single vs Multiplayer Rules)

### 싱글 플레이 (Single Player)
- **플레이어1**: 문제1과 문제2를 모두 조작 가능
- **Role**: `EReadQuestRole::Both`

### 멀티 플레이 (Multiplayer)
- **플레이어1**: `word_data1 (문제1)` 만 조작 가능
  - **Role**: `EReadQuestRole::OnlyQuestion1`
- **플레이어2**: `word_data2 (문제2)` 만 조작 가능
  - **Role**: `EReadQuestRole::OnlyQuestion2`

### 구현 방법 (Implementation)
- `ALingoPlayerState`에 `EReadQuestRole` 필드 추가
- 위젯에서 Role에 따라 버튼 활성/비활성 제어
- 서버 RPC에서 Role 검증 (권한이 없는 선택 차단)

---

## 정답 처리 로직 (Success Logic)

### 서버 처리 (Server-side)
1. 남은 시간을 `ALingoGameState::RemainMissionTime`에서 가져옵니다.
2. `FReadQuestResult` 구조체에 다음 정보를 저장합니다:
   - `bSuccess = true`
   - `RemainTime = 현재 남은 시간`
   - `AttemptCount = 시도 횟수`
   - `SelectedSymbol = 선택한 심볼`
   - `SelectedColor = 선택한 색상`
3. GameState에 성공 플래그를 Replicated 합니다.
4. `UBroadcastManager`를 통해 성공 이벤트를 브로드캐스트합니다.

### 클라이언트 처리 (Client-side)
1. OnRep 콜백에서 성공 플래그를 감지합니다.
2. 위젯에 "정답입니다" 팝업을 표시합니다.
3. 다음 Step 버튼을 활성화합니다.

---

## 오답 처리 로직 (Failure Logic)

### 서버 처리 (Server-side)

#### 1. 타이머 패널티
```cpp
// 남은 시간에서 30초 감소
float Penalty = 30.f;
float NewTime = FMath::Max(0.f, RemainMissionTime - Penalty);
GameState->RemainMissionTime = NewTime;
```

#### 2. 틀린 항목 판정
```cpp
bool bSymbolCorrect = (PlayerState->SelectedSymbol == Carrier->Symbol);
bool bColorCorrect = (PlayerState->SelectedColor == Carrier->Color);

if (!bSymbolCorrect)
{
    // 심볼이 틀렸으므로 선택 해제
    PlayerState->SelectedSymbol = TEXT("");
    PlayerState->bSymbolWrong = true;
}

if (!bColorCorrect)
{
    // 색상이 틀렸으므로 선택 해제
    PlayerState->SelectedColor = TEXT("");
    PlayerState->bColorWrong = true;
}
```

#### 3. 시도 횟수 증가
```cpp
PlayerState->AttemptCount++;
```

### 클라이언트 처리 (Client-side)

#### 1. OnRep 콜백
```cpp
// PlayerState의 OnRep_SelectedSymbol, OnRep_SelectedColor 등에서 UI 업데이트
UFUNCTION()
void OnRep_SelectedSymbol();

UFUNCTION()
void OnRep_SelectedColor();

UFUNCTION()
void OnRep_SymbolWrong();

UFUNCTION()
void OnRep_ColorWrong();
```

#### 2. UI 반영
- 틀린 항목:
  - 이전에 선택되어 있던 엔트리에 "오답" 스타일 적용 (빨간색, X 표시 등)
  - 선택 상태 해제
- 맞은 항목:
  - 기존 선택 상태 유지
- 플레이어는 다시 틀렸던 항목만 선택할 수 있게 됩니다.

---

## 필수 설계 요소 (Required Design Elements)

### 1. 클래스 확장 (Class Extensions)

#### ALingoGameState 확장
**새로 추가할 멤버**:
```cpp
// Step1(Read) 기록
UPROPERTY(Replicated, BlueprintReadOnly, Category = "Quest")
FReadQuestResult QuestResult;

// 성공/실패 플래그
UPROPERTY(ReplicatedUsing = OnRep_QuestSuccess, BlueprintReadOnly, Category = "Quest")
bool bQuestSuccess = false;

UFUNCTION()
void OnRep_QuestSuccess();
```

#### ALingoPlayerState 확장
**새로 추가할 멤버**:
```cpp
// 플레이어 역할
UPROPERTY(Replicated, BlueprintReadOnly, Category = "Quest")
EReadQuestRole QuestRole = EReadQuestRole::Both;

// 선택 상태
UPROPERTY(ReplicatedUsing = OnRep_SelectedSymbol, BlueprintReadOnly, Category = "Quest")
FString SelectedSymbol;

UPROPERTY(ReplicatedUsing = OnRep_SelectedColor, BlueprintReadOnly, Category = "Quest")
FString SelectedColor;

// 오답 플래그
UPROPERTY(ReplicatedUsing = OnRep_SymbolWrong, BlueprintReadOnly, Category = "Quest")
bool bSymbolWrong = false;

UPROPERTY(ReplicatedUsing = OnRep_ColorWrong, BlueprintReadOnly, Category = "Quest")
bool bColorWrong = false;

// 시도 횟수
UPROPERTY(Replicated, BlueprintReadOnly, Category = "Quest")
int32 AttemptCount = 0;

// Server RPC
UFUNCTION(Server, Reliable, WithValidation)
void ServerSetSelectedSymbol(const FString& Symbol);

UFUNCTION(Server, Reliable, WithValidation)
void ServerSetSelectedColor(const FString& Color);

// OnRep 콜백
UFUNCTION()
void OnRep_SelectedSymbol();

UFUNCTION()
void OnRep_SelectedColor();

UFUNCTION()
void OnRep_SymbolWrong();

UFUNCTION()
void OnRep_ColorWrong();
```

#### ALingoGameMode 확장
**새로 추가할 메서드**:
```cpp
// Step1(Read) 시작
UFUNCTION(BlueprintCallable, Category = "Quest")
void StartReadQuest();

// 캐리어 선택 처리
UFUNCTION()
void HandleCarrierSelection(APlayerState* Player, Aluggage* Carrier);

// 정답 판정
UFUNCTION()
bool ValidateAnswer(ALingoPlayerState* Player, Aluggage* Carrier);

// 정답 처리
UFUNCTION()
void HandleCorrectAnswer(ALingoPlayerState* Player);

// 오답 처리
UFUNCTION()
void HandleWrongAnswer(ALingoPlayerState* Player, bool bSymbolCorrect, bool bColorCorrect);
```

### 2. 기존 클래스 확장 (Existing Class Extension)

#### Aluggage (Luggage 액터 확장)
**위치**: `Source/Onepiece/Interactable/Public/luggage.h`

**현재 구현**:
```cpp
UCLASS()
class ONEPIECE_API Aluggage : public AActor
{
    GENERATED_BODY()

public:
    Aluggage();

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    TObjectPtr<class UStaticMeshComponent> Mesh;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<class UInteractableComponent> InteractableComp;
};
```

**추가 필요 멤버**:
```cpp
/// @brief 캐리어의 심볼 (문제1 정답)
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
FString Symbol;

/// @brief 캐리어의 색상 (문제2 정답)
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
FString Color;

/// @brief 플레이어가 캐리어를 선택했을 때 호출됩니다.
UFUNCTION(BlueprintCallable, Category = "Interaction")
void OnInteract(AActor* Interactor);

/// @brief 서버에 캐리어 선택을 알립니다.
UFUNCTION(Server, Reliable, WithValidation)
void ServerNotifySelection(APlayerState* Player);
```

#### UReadQuestWidget (Read 퀘스트 위젯)
**위치**: `Source/Onepiece/UI/Public/UReadQuestWidget.h`

**주요 멤버**:
```cpp
UCLASS()
class ONEPIECE_API UReadQuestWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UReadQuestWidget(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
    /// @brief 문제1 선택 스크롤 박스 (BindWidget)
    UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
    class UScrollBox* SymbolScrollBox;

    /// @brief 문제2 선택 스크롤 박스 (BindWidget)
    UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
    class UScrollBox* ColorScrollBox;

    /// @brief 미션 설명 텍스트 (BindWidget)
    UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
    class UTextBlock* MissionDescriptionText;

    /// @brief 타이머 텍스트 (BindWidget)
    UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
    class UTextBlock* TimerText;

protected:
    /// @brief GameState 참조 캐싱
    UPROPERTY()
    TObjectPtr<class ALingoGameState> CachedGameState;

    /// @brief PlayerState 참조 캐싱
    UPROPERTY()
    TObjectPtr<class ALingoPlayerState> CachedPlayerState;

    /// @brief 선택지 엔트리 위젯 클래스
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UReadQuestEntryWidget> EntryWidgetClass;

    /// @brief 현재 선택된 심볼 엔트리
    UPROPERTY()
    TObjectPtr<class UReadQuestEntryWidget> SelectedSymbolEntry;

    /// @brief 현재 선택된 색상 엔트리
    UPROPERTY()
    TObjectPtr<class UReadQuestEntryWidget> SelectedColorEntry;

public:
    /// @brief 퀘스트 위젯을 초기화합니다.
    UFUNCTION(BlueprintCallable, Category = "Quest")
    void InitializeQuest();

    /// @brief 심볼 리스트를 빌드합니다.
    UFUNCTION(BlueprintCallable, Category = "Quest")
    void BuildSymbolList();

    /// @brief 색상 리스트를 빌드합니다.
    UFUNCTION(BlueprintCallable, Category = "Quest")
    void BuildColorList();

    /// @brief 심볼 선택 처리
    UFUNCTION()
    void OnSymbolSelected(const FString& Symbol, UReadQuestEntryWidget* EntryWidget);

    /// @brief 색상 선택 처리
    UFUNCTION()
    void OnColorSelected(const FString& Color, UReadQuestEntryWidget* EntryWidget);

    /// @brief 타이머 업데이트
    UFUNCTION()
    void UpdateTimer();

    /// @brief PlayerState 업데이트 처리
    UFUNCTION()
    void OnPlayerStateUpdated();
};
```

#### UReadQuestEntryWidget (선택지 엔트리 위젯)
**위치**: `Source/Onepiece/UI/Public/UReadQuestEntryWidget.h`

**주요 멤버**:
```cpp
UCLASS()
class ONEPIECE_API UReadQuestEntryWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    /// @brief 선택 버튼 (BindWidget)
    UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
    class UButton* SelectButton;

    /// @brief 선택지 텍스트 (BindWidget)
    UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
    class UTextBlock* ChoiceText;

    /// @brief 상태 이미지 (BindWidget) - 정답/오답 표시
    UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
    class UImage* StateImage;

protected:
    /// @brief 선택지 값
    UPROPERTY(BlueprintReadOnly, Category = "Quest")
    FString ChoiceValue;

    /// @brief 선택 상태
    UPROPERTY(BlueprintReadOnly, Category = "Quest")
    bool bIsSelected = false;

    /// @brief 오답 상태
    UPROPERTY(BlueprintReadOnly, Category = "Quest")
    bool bIsWrong = false;

public:
    /// @brief 엔트리를 초기화합니다.
    UFUNCTION(BlueprintCallable, Category = "Quest")
    void InitializeEntry(const FString& Value, bool bEnabled);

    /// @brief 선택 상태를 설정합니다.
    UFUNCTION(BlueprintCallable, Category = "Quest")
    void SetSelected(bool bSelected);

    /// @brief 오답 상태를 설정합니다.
    UFUNCTION(BlueprintCallable, Category = "Quest")
    void SetWrong(bool bWrong);

    /// @brief 버튼 클릭 콜백
    UFUNCTION()
    void OnButtonClicked();

    /// @brief 선택 델리게이트
    DECLARE_MULTICAST_DELEGATE_TwoParams(FOnEntrySelected, const FString&, UReadQuestEntryWidget*);
    FOnEntrySelected OnEntrySelected;
};
```

---

## 예시 흐름 (Example Flow)

### 싱글 플레이 흐름 (Single Player Flow)

#### 1. Step1 시작
```
1. GameMode::StartReadQuest() 호출
   ↓
2. GameState에 FResponseScenario 세팅
   ↓
3. PlayerState에 Role = EReadQuestRole::Both 설정
   ↓
4. GameState::StartMissionTimer() 호출
   ↓
5. UBroadcastManager를 통해 "QuatStarted" 이벤트 브로드캐스트
   ↓
6. 클라이언트: ReadQuest 팝업 표시
   ↓
7. ReadQuestWidget::InitializeQuest() 호출
   ↓
8. BuildSymbolList(), BuildColorList() 호출
```

#### 2. 문제 선택
```
1. 플레이어가 심볼 버튼 클릭
   ↓
2. ReadQuestWidget::OnSymbolSelected() 호출
   ↓
3. 로컬 UI 하이라이트 업데이트
   ↓
4. PlayerState::ServerSetSelectedSymbol(Symbol) RPC 호출
   ↓
5. 서버: PlayerState::SelectedSymbol = Symbol
   ↓
6. Replicated 변수 동기화
   ↓
7. 클라이언트: PlayerState::OnRep_SelectedSymbol() 콜백
   ↓
8. 위젯 업데이트

(색상 선택도 동일한 흐름)
```

#### 3. 캐리어 선택
```
1. 플레이어가 InteractionSystem으로 캐리어(Aluggage) 상호작용
   ↓
2. Aluggage::OnInteract(Player) 호출
   ↓
3. Aluggage::ServerNotifySelection(PlayerState) RPC 호출
   ↓
4. 서버: GameMode::HandleCarrierSelection(PlayerState, Carrier) 호출
   ↓
5. GameMode::ValidateAnswer() 호출
   ↓
6. 정답/오답 분기
```

#### 4-A. 정답 처리
```
1. GameMode::HandleCorrectAnswer(PlayerState) 호출
   ↓
2. GameState::QuestResult 업데이트
   ↓
3. GameState::bQuestSuccess = true
   ↓
4. GameState::StopMissionTimer() 호출
   ↓
5. UBroadcastManager를 통해 "QuestSuccess" 이벤트 브로드캐스트
   ↓
6. 클라이언트: GameState::OnRep_QuestSuccess() 콜백
   ↓
7. 위젯에 "정답입니다" 팝업 표시
   ↓
8. 다음 Step 버튼 활성화
```

#### 4-B. 오답 처리
```
1. GameMode::HandleWrongAnswer(PlayerState, bSymbolCorrect, bColorCorrect) 호출
   ↓
2. GameState::RemainMissionTime -= 30.f (패널티)
   ↓
3. 틀린 항목 판정:
   - bSymbolCorrect == false → PlayerState::SelectedSymbol = "", bSymbolWrong = true
   - bColorCorrect == false → PlayerState::SelectedColor = "", bColorWrong = true
   ↓
4. PlayerState::AttemptCount++
   ↓
5. Replicated 변수 동기화
   ↓
6. 클라이언트: OnRep 콜백
   ↓
7. 위젯 업데이트:
   - 틀린 항목: 오답 스타일 적용, 선택 해제
   - 맞은 항목: 선택 상태 유지
   ↓
8. 플레이어는 다시 틀렸던 항목만 선택 가능
```

### 멀티 플레이 흐름 (Multiplayer Flow)

#### 1. Step1 시작
```
1. GameMode::StartReadQuest() 호출
   ↓
2. GameState에 FResponseScenario 세팅
   ↓
3. PlayerState 역할 할당:
   - Player1: Role = EReadQuestRole::OnlyQuestion1
   - Player2: Role = EReadQuestRole::OnlyQuestion2
   ↓
4. 나머지는 싱글 플레이와 동일
```

#### 2. 문제 선택
```
1. Player1이 심볼 버튼 클릭
   ↓
2. ReadQuestWidget::OnSymbolSelected() 호출
   ↓
3. PlayerState::ServerSetSelectedSymbol(Symbol) RPC 호출
   ↓
4. 서버: Role 검증
   - Player1.Role == OnlyQuestion1 → 허용
   - Player2.Role == OnlyQuestion2 → 차단
   ↓
5. 나머지는 싱글 플레이와 동일

(Player2는 색상만 선택 가능)
```

#### 3. 캐리어 선택
```
1. Player1 또는 Player2가 캐리어 선택
   ↓
2. 서버: 두 플레이어의 선택 상태를 모두 확인
   - Player1.SelectedSymbol
   - Player2.SelectedColor
   ↓
3. 정답 판정:
   - 두 조건 모두 만족 → 정답
   - 하나라도 틀림 → 오답
   ↓
4. 나머지는 싱글 플레이와 동일
```

---

## 스타일 가이드 (Style Guide)

### 코딩 컨벤션 (Coding Conventions)

#### 1. 언리얼 엔진 규칙 준수
- **클래스 접두사**: 클래스 타입에 따라 적절한 접두사 사용
  - `A`: Actor 클래스 (예: `ACarrierActor`, `ALingoGameMode`)
  - `U`: UObject 클래스 (예: `UReadQuestWidget`, `ULingoGameHelper`)
  - `F`: 구조체 (예: `FReadQuestResult`, `FWordData`)
  - `E`: Enum (예: `EReadQuestRole`)
  - `I`: Interface (예: `IControllable`)

#### 2. 네이밍 규칙
- **변수**: PascalCase, 접두사 사용
  - `bool` → `bIsSelected`, `bQuestSuccess`
  - `int32` → `AttemptCount`, `ScenarioIndex`
  - `float` → `RemainTime`, `Penalty`
  - `TObjectPtr` → `CachedGameState`, `InteractionSystem`
- **함수**: PascalCase, 동사로 시작
  - `InitializeQuest()`, `BuildSymbolList()`, `OnSymbolSelected()`
- **RPC 함수**: `Server`, `Client`, `Multicast` 접두사
  - `ServerSetSelectedSymbol()`, `ClientShowPopup()`, `MulticastBroadcastResult()`

#### 3. UPROPERTY 메타 지정자
```cpp
// Replicated 변수
UPROPERTY(Replicated, BlueprintReadOnly, Category = "Quest")
float RemainTime;

// ReplicatedUsing 변수
UPROPERTY(ReplicatedUsing = OnRep_QuestSuccess, BlueprintReadOnly, Category = "Quest")
bool bQuestSuccess;

// BindWidget
UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
class UScrollBox* SymbolScrollBox;

// EditAnywhere (디자이너가 설정 가능)
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Carrier")
FString Symbol;
```

#### 4. UFUNCTION 메타 지정자
```cpp
// Server RPC
UFUNCTION(Server, Reliable, WithValidation)
void ServerSetSelectedSymbol(const FString& Symbol);

// Client RPC
UFUNCTION(Client, Reliable)
void ClientShowPopup(const FString& Message);

// OnRep 콜백
UFUNCTION()
void OnRep_QuestSuccess();

// BlueprintCallable
UFUNCTION(BlueprintCallable, Category = "Quest")
void InitializeQuest();
```

### Doxygen 주석 스타일

#### 1. 파일 헤더
```cpp
// Copyright (c) 2025 Doppleddiggong. All rights reserved.

/// @file luggage.h
/// @brief 수하물(Luggage) 액터를 정의합니다.
```

#### 2. 클래스 주석
```cpp
/// @brief 상호작용 가능한 수하물 액터
/// @details 플레이어가 선택할 수 있는 수하물 오브젝트입니다.
///          Read 퀘스트에서는 Symbol과 Color 정보를 추가로 가지며, 정답 판정에 사용됩니다.
UCLASS()
class ONEPIECE_API Aluggage : public AActor
{
    GENERATED_BODY()
};
```

#### 3. 함수 주석
```cpp
/// @brief 캐리어를 선택했을 때 호출됩니다.
/// @param Interactor [in] 상호작용을 시도한 액터 (플레이어)
/// @return 없음
UFUNCTION(BlueprintCallable, Category = "Interaction")
void OnInteract(AActor* Interactor);
```

#### 4. 변수 주석
```cpp
/// @brief 캐리어의 심볼 (문제1 정답)
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Carrier")
FString Symbol;
```

### 모듈 및 로그

#### 1. 모듈 매크로
```cpp
// 모든 public 클래스에 ONEPIECE_API 매크로 사용
class ONEPIECE_API Aluggage : public AActor
```

#### 2. 로그 카테고리
```cpp
// GameLogging.h에 정의된 로그 매크로 사용
PRINTLOG(TEXT("[ReadQuest] Symbol selected: %s"), *Symbol);
```

### 블루프린트 호환성
- 가능한 모든 함수에 `BlueprintCallable` 또는 `BlueprintPure` 지정
- 구조체는 `USTRUCT(BlueprintType)` 사용
- Enum은 `UENUM(BlueprintType)` 사용

---

## 구현 체크리스트 (Implementation Checklist)

### Phase 1: 데이터 구조 및 GameState/PlayerState 확장
- [ ] `EReadQuestRole` Enum 정의
- [ ] `FReadQuestResult` 구조체 정의
- [ ] `ALingoGameState` 확장
  - [ ] `FReadQuestResult QuestResult` 추가
  - [ ] `bool bQuestSuccess` 추가
  - [ ] `OnRep_QuestSuccess()` 구현
- [ ] `ALingoPlayerState` 확장
  - [ ] `EReadQuestRole QuestRole` 추가
  - [ ] `FString SelectedSymbol` 추가
  - [ ] `FString SelectedColor` 추가
  - [ ] `bool bSymbolWrong` 추가
  - [ ] `bool bColorWrong` 추가
  - [ ] `int32 AttemptCount` 추가
  - [ ] `ServerSetSelectedSymbol()` RPC 구현
  - [ ] `ServerSetSelectedColor()` RPC 구현
  - [ ] OnRep 콜백들 구현

### Phase 2: GameMode 로직 구현
- [ ] `ALingoGameMode` 확장
  - [ ] `StartReadQuest()` 구현
  - [ ] `HandleCarrierSelection()` 구현
  - [ ] `ValidateAnswer()` 구현
  - [ ] `HandleCorrectAnswer()` 구현
  - [ ] `HandleWrongAnswer()` 구현

### Phase 3: Luggage 액터 확장
- [ ] `Aluggage` 클래스 확장
  - [ ] `Symbol`, `Color` 프로퍼티 추가 (Quest 데이터)
  - [ ] `OnInteract()` 메서드 추가
  - [ ] `ServerNotifySelection()` RPC 구현
  - [ ] 기존 `InteractableComp`와 연동

### Phase 4: UI 위젯 구현
- [ ] `UReadQuestWidget` 클래스 생성
  - [ ] BindWidget 프로퍼티들 추가
  - [ ] `InitializeQuest()` 구현
  - [ ] `BuildSymbolList()` 구현
  - [ ] `BuildColorList()` 구현
  - [ ] `OnSymbolSelected()` 구현
  - [ ] `OnColorSelected()` 구현
  - [ ] `UpdateTimer()` 구현
  - [ ] `OnPlayerStateUpdated()` 구현
- [ ] `UReadQuestEntryWidget` 클래스 생성
  - [ ] BindWidget 프로퍼티들 추가
  - [ ] `InitializeEntry()` 구현
  - [ ] `SetSelected()` 구현
  - [ ] `SetWrong()` 구현
  - [ ] `OnButtonClicked()` 구현

### Phase 5: 블루프린트 작업
- [ ] ReadQuestWidget UMG 블루프린트 생성
  - [ ] 레이아웃 구성 (ScrollBox, TextBlock 등)
- [ ] ReadQuestEntryWidget UMG 블루프린트 생성
  - [ ] 버튼, 텍스트, 이미지 구성
- [ ] Aluggage 블루프린트 생성 (BP_QuestLuggage 등)
  - [ ] 기존 메시 활용
  - [ ] Symbol, Color 프로퍼티 설정 가능하도록 구성

### Phase 6: 테스트 및 디버깅
- [ ] 싱글 플레이 테스트
  - [ ] 정답 케이스
  - [ ] 오답 케이스 (심볼만 틀림)
  - [ ] 오답 케이스 (색상만 틀림)
  - [ ] 오답 케이스 (둘 다 틀림)
  - [ ] 타이머 패널티 확인
- [ ] 멀티 플레이 테스트
  - [ ] Player1이 문제1만 조작 가능한지 확인
  - [ ] Player2가 문제2만 조작 가능한지 확인
  - [ ] 두 플레이어가 동시에 선택했을 때 정답 판정
  - [ ] 오답 시 각 플레이어의 UI 업데이트 확인

---

## 참고 문서 (References)

### 프로젝트 문서
- `AgentRule/Project/Onepiece/AGENT_GUIDE.md`: Onepiece 프로젝트 공통 가이드
- `AgentRule/Project/ue_coding_conventions.md`: 언리얼 엔진 코딩 컨벤션
- `AgentRule/Project/Onepiece/DOXYGEN_SETUP.md`: Doxygen 문서화 가이드

### 언리얼 엔진 문서
- [Unreal Engine Multiplayer Programming](https://docs.unrealengine.com/en-US/ProgrammingAndScripting/Networking/)
- [Unreal Engine Replicated Properties](https://docs.unrealengine.com/en-US/ProgrammingAndScripting/Networking/Actors/Properties/)
- [Unreal Engine RPC](https://docs.unrealengine.com/en-US/ProgrammingAndScripting/Networking/Actors/RPCs/)
- [Unreal Engine UMG](https://docs.unrealengine.com/en-US/InteractiveExperiences/UMG/)

---

## 최종 노트 (Final Notes)

### 설계 원칙
1. **책임 분리**: GameMode는 게임 로직, GameState는 공유 상태, PlayerState는 플레이어 상태, Widget은 UI만 담당
2. **네트워크 안정성**: 모든 중요한 상태는 Replicated 변수로 관리, RPC는 최소화
3. **확장성**: 다른 Step (Listen, Write, Speak)을 구현할 때 재사용 가능한 구조
4. **디버깅 용이성**: 적절한 로그 메시지와 디버그 정보 출력

### 주의사항
1. **RPC Validation**: 모든 Server RPC에 `WithValidation` 추가하고 `_Validate()` 함수 구현
2. **Role 검증**: 플레이어가 권한이 없는 선택을 하지 못하도록 서버에서 검증
3. **타이머 동기화**: 타이머는 서버에서만 업데이트, 클라이언트는 Replicated 변수 참조
4. **UI 업데이트**: OnRep 콜백에서만 UI 업데이트 수행 (직접 수정 금지)

### 다음 단계
이 문서를 바탕으로 단계별로 구현을 진행하세요. 각 Phase를 완료한 후 체크리스트를 확인하고, 다음 Phase로 넘어가세요.

구현 중 질문이나 문제가 발생하면 이 문서와 프로젝트 가이드를 참조하세요.
