# KLingo API - Editor Utility Widget 테스트 도구 설계 문서

> **프로젝트**: Onepiece (POTENUP 최종 프로젝트)
> **목적**: 언리얼 에디터 내에서 KLingo API를 테스트할 수 있는 Editor Utility Widget 도구 생성

## 문서 목표

이 문서는 **Onepiece 프로젝트의 기존 UKLingoNetworkSystem을 활용**하여 에디터 내에서 KLingo API를 테스트할 수 있는 Editor Utility Widget 기반 테스트 도구를 설계합니다.

---

## 시스템 개요

### 핵심 기능

1. **API 목록 관리**: DataTable 기반 API 엔드포인트 관리
2. **API 선택**: 드롭다운에서 API 선택 시 자동으로 엔드포인트 채우기
3. **JSON Payload 입력**: 멀티라인 텍스트 박스에서 JSON 요청 본문 작성
4. **Mock Mode**: 실제 서버 대신 Mock JSON 파일 사용
5. **Send Request**: UKLingoNetworkSystem을 통한 실제 API 호출
6. **Response Display**: Pretty Print된 JSON 응답 출력

### 시스템 구성도

```
┌─────────────────────────────────────────────────────────┐
│  Editor Utility Widget (EUW_KLingoAPITester)            │
│  ┌───────────────────────────────────────────────────┐  │
│  │  [API 선택] [▼ /Login                          ]  │  │
│  │  [Endpoint]  /Login                              │  │
│  │  [Method]    POST                                │  │
│  │  [Mock Mode] ☐                                   │  │
│  │  ┌─────────────────────────────────────────────┐ │  │
│  │  │ JSON Payload (Input)                        │ │  │
│  │  │ {                                           │ │  │
│  │  │   "account": "player123"                    │ │  │
│  │  │ }                                           │ │  │
│  │  └─────────────────────────────────────────────┘ │  │
│  │  [Send Request]                                  │  │
│  │  ┌─────────────────────────────────────────────┐ │  │
│  │  │ Response (Output - Pretty Print)            │ │  │
│  │  │ {                                           │ │  │
│  │  │   "token": "abc123...",                     │ │  │
│  │  │   "playerInfo": {                           │ │  │
│  │  │     "nickname": "Player",                   │ │  │
│  │  │     "gold": 1000                            │ │  │
│  │  │   }                                         │ │  │
│  │  │ }                                           │ │  │
│  │  └─────────────────────────────────────────────┘ │  │
│  └───────────────────────────────────────────────────┘  │
│                          ↓                              │
│         UKLingoAPITestSubsystem (Editor Only)           │
│                          ↓                              │
│         UKLingoNetworkSystem (Runtime)                  │
│                          ↓                              │
│                    KLingo API Server                    │
└─────────────────────────────────────────────────────────┘
```

---

## 프로젝트 구조

```
Source/Onepiece/
├── Network/
│   ├── Public/
│   │   ├── KLingoAPITestEntry.h          # DataTable Row 구조체
│   │   └── UKLingoAPITestSubsystem.h     # 에디터 테스트 서브시스템
│   └── Private/
│       └── UKLingoAPITestSubsystem.cpp   # 테스트 로직 구현
├── Utils/                                 # (신규)
│   ├── Public/
│   │   └── JsonPrettyPrinter.h           # JSON Pretty Print 유틸리티
│   └── Private/
│       └── JsonPrettyPrinter.cpp
└── Onepiece.Build.cs

Content/
├── EditorUtilities/
│   └── EUW_KLingoAPITester               # Editor Utility Widget
├── Data/
│   ├── DT_KLingoAPIList                  # DataTable Asset
│   └── MockResponses/                    # Mock JSON 파일들
│       ├── Login.json
│       ├── CreateUser.json
│       └── ...
└── API/
    └── KLingoAPIList.csv                 # CSV 소스 파일
```

---

## 생성할 파일 목록

### C++ 파일

1. **KLingoAPITestEntry.h** - DataTable Row 구조체
2. **UKLingoAPITestSubsystem.h/cpp** - 에디터 테스트 서브시스템
3. **JsonPrettyPrinter.h/cpp** - JSON 포맷팅 유틸리티

### 데이터 파일

4. **KLingoAPIList.csv** - API 목록 CSV
5. **Mock JSON 파일들** - 각 API별 Mock 응답

### 에디터 에셋

6. **EUW_KLingoAPITester** - Editor Utility Widget (UMG)
7. **DT_KLingoAPIList** - DataTable Asset (CSV에서 생성)

---

## Onepiece 프로젝트 컨벤션

### 1. 파일 헤더
```cpp
// Copyright (c) 2025 Doppleddiggong. All rights reserved.
// Unauthorized copying, modification, or distribution of this file,
// via any medium is strictly prohibited. Proprietary and confidential.
```

### 2. 에디터 전용 코드
```cpp
#if WITH_EDITOR
// 에디터 전용 코드
#endif
```

### 3. 서브시스템 구조
- `UEditorSubsystem` 상속 (에디터 전용)
- `DEFINE_SUBSYSTEM_GETTER_INLINE` 매크로 사용 가능하나 에디터 전용이므로 조건부 사용

### 4. 로깅
```cpp
NETWORK_LOG(ENetworkLogType::GET, TEXT("[API Test] %s"), *Message);
```

---

## 1. KLingoAPITestEntry.h 설계

**목적**: DataTable Row로 사용할 API 정보 구조체

**파일 위치**: `Source/Onepiece/Network/Public/KLingoAPITestEntry.h`

**구조체 필드**:
```cpp
USTRUCT(BlueprintType)
struct FKLingoAPITestEntry : public FTableRowBase
{
    GENERATED_BODY()

    // API 이름 (UI 표시용)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "API")
    FString APIName;

    // 엔드포인트 경로
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "API")
    FString Endpoint;

    // HTTP 메서드 (GET, POST)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "API")
    FString Method;

    // 설명
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "API")
    FString Description;

    // 샘플 JSON Payload
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "API")
    FString SamplePayload;

    // Mock Response 파일 경로 (Content/ 기준 상대 경로)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "API")
    FString MockResponsePath;
};
```

---

## 2. UKLingoAPITestSubsystem 설계

**목적**: Editor Utility Widget에서 호출할 API 테스트 로직 제공

**파일 위치**:
- `Source/Onepiece/Network/Public/UKLingoAPITestSubsystem.h`
- `Source/Onepiece/Network/Private/UKLingoAPITestSubsystem.cpp`

**주요 기능**:
1. API 호출 (실제 서버 또는 Mock)
2. JSON Pretty Print
3. 응답 델리게이트 전달

**클래스 구조**:
```cpp
#if WITH_EDITOR

DECLARE_DELEGATE_TwoParams(FOnAPITestComplete, bool /*bSuccess*/, const FString& /*Response*/);

UCLASS()
class ONEPIECE_API UKLingoAPITestSubsystem : public UEditorSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    /// @brief API 테스트 요청을 전송합니다.
    /// @param Endpoint [in] API 엔드포인트입니다.
    /// @param Method [in] HTTP 메서드 (GET, POST)입니다.
    /// @param Payload [in] JSON 요청 본문입니다.
    /// @param bMockMode [in] Mock 모드 활성화 여부입니다.
    /// @param MockFilePath [in] Mock JSON 파일 경로입니다.
    /// @param OnComplete [in] 완료 시 호출될 델리게이트입니다.
    UFUNCTION(BlueprintCallable, Category = "KLingo API Test")
    void SendAPIRequest(
        const FString& Endpoint,
        const FString& Method,
        const FString& Payload,
        bool bMockMode,
        const FString& MockFilePath,
        FOnAPITestComplete OnComplete
    );

    /// @brief JSON 문자열을 Pretty Print 합니다.
    /// @param JsonString [in] JSON 문자열입니다.
    /// @return Pretty Print된 JSON 문자열입니다.
    UFUNCTION(BlueprintCallable, Category = "KLingo API Test")
    FString PrettyPrintJSON(const FString& JsonString);

private:
    /// @brief Mock JSON 파일을 읽어옵니다.
    FString LoadMockResponse(const FString& FilePath);
};

#endif // WITH_EDITOR
```

---

## 3. JsonPrettyPrinter 설계

**목적**: JSON 문자열을 보기 좋게 포맷팅

**파일 위치**:
- `Source/Onepiece/Utils/Public/JsonPrettyPrinter.h`
- `Source/Onepiece/Utils/Private/JsonPrettyPrinter.cpp`

**함수**:
```cpp
class ONEPIECE_API FJsonPrettyPrinter
{
public:
    /// @brief JSON 문자열을 Pretty Print 합니다.
    /// @param JsonString [in] 포맷팅할 JSON 문자열입니다.
    /// @return 포맷팅된 JSON 문자열입니다.
    static FString PrettyPrint(const FString& JsonString);
};
```

**구현 방법**:
1. `FJsonSerializer::Deserialize()`로 JSON 파싱
2. `TJsonWriterFactory<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>::Create()`로 Pretty Writer 생성
3. `FJsonSerializer::Serialize()`로 포맷팅된 문자열 생성

---

## 4. API 목록 CSV 설계

**파일 위치**: `Content/API/KLingoAPIList.csv`

**CSV 형식**:
```csv
Name,APIName,Endpoint,Method,Description,SamplePayload,MockResponsePath
Login,Login,/Login,POST,사용자 로그인,"{\"account\":\"player123\"}",Data/MockResponses/Login.json
CreateUser,CreateUser,/CreateUserInfo,POST,새 사용자 생성,"{\"character\":0,\"characterColor\":0}",Data/MockResponses/CreateUser.json
RequestInterview,RequestInterview,/RequestInterview,GET,인터뷰 요청,"",Data/MockResponses/RequestInterview.json
SubmitInterview,SubmitInterview,/SubmitInterviewAnswer,POST,인터뷰 제출,"{\"answers\":[\"answer1\",\"answer2\"]}",Data/MockResponses/SubmitInterview.json
StartGame,StartGame,/StartGame,POST,게임 시작,"{\"playerList\":[\"p1\",\"p2\"],\"nicknames\":[\"n1\",\"n2\"]}",Data/MockResponses/StartGame.json
GameLogin,GameLogin,/GameLogin,POST,게임 로그인,"{\"token\":\"abc123\"}",Data/MockResponses/GameLogin.json
QuestAnswer,QuestAnswer,/QuestAnswer,POST,퀘스트 답변,"{\"questStep\":0,\"questAnswer\":1,\"playTime\":10.5}",Data/MockResponses/QuestAnswer.json
QuestFail,QuestFail,/QuestFail,POST,퀘스트 실패,"{\"questStep\":0}",Data/MockResponses/QuestFail.json
QuestWrite,QuestWrite,/QuestWrite,POST,Write 퀘스트,"{\"imagePaths\":[],\"textData\":[]}",Data/MockResponses/QuestWrite.json
QuestSpeak,QuestSpeak,/QuestSpeak,POST,Speak 퀘스트,"{\"speakStep\":0,\"wavFilePath\":\"\"}",Data/MockResponses/QuestSpeak.json
GameResult,GameResult,/GameResult,GET,게임 결과 조회,"",Data/MockResponses/GameResult.json
```

---

## 5. Mock JSON 파일 예제

### Login.json
```json
{
  "token": "mock_auth_token_abc123",
  "playerInfo": {
    "userIndex": 1,
    "nickname": "MockPlayer",
    "gold": 1000,
    "character": 0,
    "characterColor": 0,
    "unlockEquip": "",
    "login": true
  }
}
```

### StartGame.json
```json
{
  "questRead": {
    "readIndex": 1,
    "difficulty": 1,
    "questTarget": [
      { "findSymbol": "A", "findColor": "Red" },
      { "findSymbol": "B", "findColor": "Blue" }
    ],
    "correctAnswerIndex": 0,
    "wordData1": { "kor": "사과", "eng": "Apple", "pronunciation": "æpl" },
    "wordData2": { "kor": "바나나", "eng": "Banana", "pronunciation": "bəˈnænə" },
    "fullData": "Mock Full Data"
  },
  "questListen": {
    "listenIndex": 1,
    "difficulty": 1,
    "questTarget": [
      { "findTarget1": "Listen1", "findTarget2": "Listen2" }
    ],
    "correctAnswerIndex": 0,
    "wordData1": { "kor": "듣기", "eng": "Listen", "pronunciation": "ˈlɪsən" },
    "wordData2": { "kor": "말하기", "eng": "Speak", "pronunciation": "spiːk" },
    "fullData": "Mock Listen Data"
  },
  "questWrite": {
    "writeIndex": 1,
    "difficulty": 1,
    "questTarget": [
      { "textureData": "MockTexture" }
    ]
  },
  "questSpeak": {
    "speakIndex": 1,
    "difficulty": 1,
    "answerPlayer": ["Player1", "Player2"],
    "questTarget": ["Target1", "Target2"]
  }
}
```

---

## 6. Editor Utility Widget 설계

### UMG 레이아웃

```
┌─────────────────────────────────────────────────────────┐
│  KLingo API Tester                                      │
├─────────────────────────────────────────────────────────┤
│  API Selection                                          │
│  ┌─────────────────────────────────────────────────┐   │
│  │ ComboBox: [▼ Login                            ] │   │
│  └─────────────────────────────────────────────────┘   │
│                                                         │
│  API Information                                        │
│  Endpoint: [/Login                                 ]   │
│  Method:   [POST                                   ]   │
│  ☐ Mock Mode                                           │
│                                                         │
│  Request Payload (JSON)                                 │
│  ┌─────────────────────────────────────────────────┐   │
│  │ {                                               │   │
│  │   "account": "player123"                        │   │
│  │ }                                               │   │
│  │                                                 │   │
│  │                                                 │   │
│  └─────────────────────────────────────────────────┘   │
│  [Send Request]                                         │
│                                                         │
│  Response (JSON)                                        │
│  ┌─────────────────────────────────────────────────┐   │
│  │ {                                               │   │
│  │   "token": "abc123...",                         │   │
│  │   "playerInfo": {                               │   │
│  │     "nickname": "Player",                       │   │
│  │     "gold": 1000                                │   │
│  │   }                                             │   │
│  │ }                                               │   │
│  │                                                 │   │
│  └─────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────┘
```

### 위젯 구성 요소

1. **ComboBox_APIList** (ComboBoxString)
   - API 목록 표시
   - OnSelectionChanged 이벤트 → API 정보 자동 채우기

2. **TextBox_Endpoint** (TextBox)
   - ReadOnly
   - API 선택 시 자동 채워짐

3. **TextBox_Method** (TextBox)
   - ReadOnly
   - API 선택 시 자동 채워짐

4. **CheckBox_MockMode** (CheckBox)
   - Mock 모드 활성화 여부

5. **MultiLineTextBox_Payload** (MultiLineEditableTextBox)
   - JSON 요청 본문 입력
   - API 선택 시 샘플 자동 채워짐

6. **Button_SendRequest** (Button)
   - OnClicked 이벤트 → API 요청 전송

7. **MultiLineTextBox_Response** (MultiLineEditableTextBox)
   - ReadOnly
   - Pretty Print된 응답 표시

---

## 7. Blueprint Graph 설계

### Event: OnSelectionChanged (ComboBox_APIList)

```
[ComboBox_APIList - OnSelectionChanged]
    ↓
[Get DataTable Row (DT_KLingoAPIList)]
    ↓ (FKLingoAPITestEntry)
[Set Text (TextBox_Endpoint)] ← Row.Endpoint
[Set Text (TextBox_Method)] ← Row.Method
[Set Text (MultiLineTextBox_Payload)] ← Row.SamplePayload
```

### Event: OnClicked (Button_SendRequest)

```
[Button_SendRequest - OnClicked]
    ↓
[Get EditorSubsystem (UKLingoAPITestSubsystem)]
    ↓
[Get Text (TextBox_Endpoint, TextBox_Method, MultiLineTextBox_Payload)]
[Get CheckBox State (CheckBox_MockMode)]
[Get Selected API Row] → MockResponsePath
    ↓
[SendAPIRequest]
    ├─ Endpoint
    ├─ Method
    ├─ Payload
    ├─ bMockMode
    └─ MockFilePath
    ↓ (OnComplete Delegate)
[PrettyPrintJSON] ← Response String
    ↓
[Set Text (MultiLineTextBox_Response)]
```

---

## 8. 구현 체크리스트

### C++ 파일
- [ ] KLingoAPITestEntry.h (DataTable Row 구조체)
- [ ] UKLingoAPITestSubsystem.h/cpp (에디터 서브시스템)
- [ ] JsonPrettyPrinter.h/cpp (JSON 포맷팅 유틸리티)

### 데이터 파일
- [ ] KLingoAPIList.csv (API 목록)
- [ ] Mock JSON 파일 11개 (각 API별)

### 에디터 에셋
- [ ] DT_KLingoAPIList (DataTable)
- [ ] EUW_KLingoAPITester (Editor Utility Widget)

### 빌드 설정
- [ ] Onepiece.Build.cs에 "EditorSubsystem" 모듈 추가 확인

---

## 9. 설치 및 사용 가이드

### Step 1: C++ 파일 생성
1. KLingoAPITestEntry.h 생성
2. UKLingoAPITestSubsystem.h/cpp 생성
3. JsonPrettyPrinter.h/cpp 생성
4. 프로젝트 빌드

### Step 2: CSV 및 Mock 파일 준비
1. `Content/API/KLingoAPIList.csv` 생성
2. `Content/Data/MockResponses/` 폴더 생성
3. 각 API별 Mock JSON 파일 생성

### Step 3: DataTable 생성
1. Content Browser에서 우클릭 → Miscellaneous → Data Table
2. Row Structure: FKLingoAPITestEntry 선택
3. 이름: DT_KLingoAPIList
4. Import CSV (KLingoAPIList.csv)

### Step 4: Editor Utility Widget 생성
1. Content Browser에서 우클릭 → Editor Utilities → Editor Utility Widget
2. 이름: EUW_KLingoAPITester
3. UMG 디자이너에서 위젯 레이아웃 구성
4. Blueprint Graph에서 로직 구현

### Step 5: 에디터 메뉴에 추가
1. EUW_KLingoAPITester 우클릭 → Run Editor Utility Widget
2. (선택) Tools 메뉴에 영구 추가 설정

---

## 10. 주의사항

### 에디터 전용 코드
- `#if WITH_EDITOR` 전처리기 사용
- `UEditorSubsystem` 상속
- Runtime 빌드에 포함되지 않도록 주의

### Mock 모드
- Mock JSON 파일 경로는 Content/ 기준 상대 경로
- `FPaths::ProjectContentDir()` 사용하여 절대 경로 변환

### 실제 API 호출
- UKLingoNetworkSystem 활용
- 델리게이트 바인딩 시 Lambda 사용 권장
- 에디터에서 WorldContext 가져오기: `GEditor->GetEditorWorldContext()`

---

## 최종 산출물

위 설계를 기반으로 다음 파일들을 생성해주세요:

1. **KLingoAPITestEntry.h**
2. **UKLingoAPITestSubsystem.h**
3. **UKLingoAPITestSubsystem.cpp**
4. **JsonPrettyPrinter.h**
5. **JsonPrettyPrinter.cpp**
6. **KLingoAPIList.csv**
7. **Mock JSON 샘플 (Login.json, StartGame.json)**
8. **Editor Utility Widget 구성 가이드 (상세 Blueprint 노드 설명)**

**생성 조건**:
- Onepiece 프로젝트 컨벤션 100% 준수
- 에디터 전용 코드 (#if WITH_EDITOR)
- 복사-붙여넣기로 즉시 사용 가능
- Doxygen 주석 포함
- 완전한 구현 (TODO 없음)
