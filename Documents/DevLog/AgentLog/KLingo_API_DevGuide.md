# KLingo API - Onepiece 프로젝트 네트워크 시스템 구현 가이드

> **프로젝트**: Onepiece (POTENUP 최종 프로젝트)
> **목적**: KLingo 언어 학습 API를 Onepiece 프로젝트 구조와 코딩 컨벤션에 맞춰 구현

## 문서 목표

이 문서는 **Onepiece 프로젝트의 기존 네트워크 구조를 활용**하여 KLingo API를 완전히 구현하기 위한 가이드입니다.
프로젝트의 코딩 컨벤션, 네임스페이스 패턴, 델리게이트 구조를 준수하여 일관성 있는 코드를 생성합니다.

---

## 프로젝트 구조

```
Source/Onepiece/
├── Network/
│   ├── Public/
│   │   ├── KLingoNetworkData.h         # KLingo API 요청/응답 구조체
│   │   └── UKLingoNetworkSystem.h      # KLingo 네트워크 서브시스템
│   └── Private/
│       ├── KLingoNetworkData.cpp       # 응답 파싱 및 헬퍼 메서드 구현
│       └── UKLingoNetworkSystem.cpp    # HTTP 요청 로직 구현
└── Onepiece.Build.cs                   # 빌드 설정 (HTTP, Json 모듈 이미 포함됨)
```

**참고**: 기존 `UHttpNetworkSystem`과 동일한 구조로 `UKLingoNetworkSystem`을 추가합니다.

---

## Onepiece 프로젝트 컨벤션 요약

### 1. 파일 헤더
모든 파일 상단에 다음 헤더를 추가합니다:
```cpp
// Copyright (c) 2025 Doppleddiggong. All rights reserved.
// Unauthorized copying, modification, or distribution of this file,
// via any medium is strictly prohibited. Proprietary and confidential.
```

### 2. 네임스페이스 패턴
- **엔드포인트 정의**: `RequestAPI` 네임스페이스 사용
- **URL 구성**: `NetworkConfig::GetFullUrl()` 활용 (UCustomNetworkSettings 기반)

### 3. 서브시스템 구조
- `UGameInstanceSubsystem` 상속
- `DEFINE_SUBSYSTEM_GETTER_INLINE` 매크로 사용 (전역 접근자)
- `ONEPIECE_API` 매크로로 클래스 공개

### 4. 로깅 시스템
- **매크로**: `NETWORK_LOG(LogType, Format, ...)`
- **열거형**: `ENetworkLogType` (GET, POST, WS, ERROR 등)

### 5. 델리게이트 패턴
- **C++ 콜백**: `DECLARE_DELEGATE_TwoParams(FDelegateName, ResponseStruct&, bool /*bSuccess*/)`
- **블루프린트**: `DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(...)`

### 6. 응답 구조체 규칙
- `USTRUCT(BlueprintType)` 사용
- 필수 메서드:
  - `void SetFromHttpResponse(const TSharedPtr<IHttpResponse, ESPMode::ThreadSafe>& Response)`
  - `void PrintData()`

### 7. Multipart Form Data
- `FHttpMultipartFormData` 클래스 사용 (프로젝트에 이미 구현됨)
- 텍스트 필드: `AddText(FieldName, Value)`
- 파일 필드: `AddFile(FieldName, FilePath, MimeType)`

---

## 코드 생성 요청

다음 파일들을 **Onepiece 프로젝트 컨벤션에 맞춰** 생성해주세요:

### 1. KLingoNetworkData.h

**목적**: KLingo API의 모든 요청/응답 구조체 및 델리게이트를 정의합니다.

**파일 위치**: `Source/Onepiece/Network/Public/KLingoNetworkData.h`

**구조체 목록**:
- `FPlayerInfo` - 플레이어 정보 (userIndex, nickname, gold, character, characterColor, unlockEquip, login)
- `FWordData` - 단어 데이터 (kor, eng, pronunciation)
- `FReadTargetData` - Read 퀘스트 타겟 (findSymbol, findColor)
- `FQuestReadInfo` - Read 퀘스트 정보 (readIndex, difficulty, QuestTarget[10], correctAnswerIndex, wordData1, wordData2, fullData)
- `FListenTargetData` - Listen 퀘스트 타겟 (findTarget1, findTarget2)
- `FQuestListenInfo` - Listen 퀘스트 정보 (listenIndex, difficulty, QuestTarget[15], correctAnswerIndex, wordData1, wordData2, fullData)
- `FWriteTargetData` - Write 퀘스트 타겟 (textureData)
- `FWriteTeachData` - Write 교육 데이터 (index, teachString)
- `FQuestWriteInfo` - Write 퀘스트 정보 (writeIndex, difficulty, QuestTarget[])
- `FQuestSpeakInfo` - Speak 퀘스트 정보 (speakIndex, difficulty, answerPlayer[], QuestTarget[])
- `FResponseLogin` - Login 응답 (token, playerInfo)
- `FResponseCreateUser` - 사용자 생성 응답
- `FResponseInterview` - 인터뷰 응답
- `FResponseStartGame` - 게임 시작 응답 (4개 Quest 데이터)
- `FResponseGameLogin` - 게임 로그인 응답
- `FResponseQuestAnswer` - 퀘스트 답변 응답 (result, teachString, failCount)
- `FResponseQuestWrite` - Write 퀘스트 응답
- `FResponseQuestSpeak` - Speak 퀘스트 응답
- `FResponseGameResult` - 게임 결과 응답 (grade, scores)

**델리게이트 정의** (Onepiece 패턴 준수):
```cpp
// C++ 콜백 델리게이트
DECLARE_DELEGATE_TwoParams(FResponseLoginDelegate, FResponseLogin&, bool /*bSuccess*/);
DECLARE_DELEGATE_TwoParams(FResponseCreateUserDelegate, FResponseCreateUser&, bool /*bSuccess*/);
DECLARE_DELEGATE_TwoParams(FResponseInterviewDelegate, FResponseInterview&, bool /*bSuccess*/);
DECLARE_DELEGATE_TwoParams(FResponseStartGameDelegate, FResponseStartGame&, bool /*bSuccess*/);
DECLARE_DELEGATE_TwoParams(FResponseGameLoginDelegate, FResponseGameLogin&, bool /*bSuccess*/);
DECLARE_DELEGATE_TwoParams(FResponseQuestAnswerDelegate, FResponseQuestAnswer&, bool /*bSuccess*/);
DECLARE_DELEGATE_TwoParams(FResponseQuestWriteDelegate, FResponseQuestWrite&, bool /*bSuccess*/);
DECLARE_DELEGATE_TwoParams(FResponseQuestSpeakDelegate, FResponseQuestSpeak&, bool /*bSuccess*/);
DECLARE_DELEGATE_TwoParams(FResponseGameResultDelegate, FResponseGameResult&, bool /*bSuccess*/);
```

**네임스페이스 패턴** (`NetworkData.h` 스타일 적용):
```cpp
namespace RequestAPI
{
    // KLingo API 엔드포인트
    static FString KLingoLogin = FString("/api/v1/Login");
    static FString KLingoCreateUser = FString("/api/v1/CreateUserInfo");
    static FString KLingoRequestInterview = FString("/api/v1/RequestInterview");
    static FString KLingoSubmitInterview = FString("/api/v1/SubmitInterviewAnswer");
    static FString KLingoStartGame = FString("/api/v1/StartGame");
    static FString KLingoGameLogin = FString("/api/v1/GameLogin");
    static FString KLingoQuestAnswer = FString("/api/v1/QuestAnswer");
    static FString KLingoQuestFail = FString("/api/v1/QuestFail");
    static FString KLingoQuestWrite = FString("/api/v1/QuestWrite");
    static FString KLingoQuestSpeak = FString("/api/v1/QuestSpeak");
    static FString KLingoGameResult = FString("/api/v1/GameResult");
}
```

**구조체 규칙**:
- 모든 구조체: `USTRUCT(BlueprintType)` + `GENERATED_BODY()`
- 모든 필드: `UPROPERTY(BlueprintReadWrite, Category = "KLingo")`
- 응답 구조체는 `SetFromHttpResponse()` 및 `PrintData()` 메서드 포함

---

### 2. UKLingoNetworkSystem.h

**목적**: KLingo API 요청을 처리하는 게임 인스턴스 서브시스템을 선언합니다.

**파일 위치**: `Source/Onepiece/Network/Public/UKLingoNetworkSystem.h`

**클래스 구조** (기존 `UHttpNetworkSystem` 패턴 준수):
```cpp
UCLASS()
class ONEPIECE_API UKLingoNetworkSystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    DEFINE_SUBSYSTEM_GETTER_INLINE(UKLingoNetworkSystem);

    /// @brief HTTP 모듈 초기화 및 네트워크 대기 카운터를 설정합니다.
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    /// @brief 등록된 델리게이트를 정리하고 서브시스템을 종료합니다.
    virtual void Deinitialize() override;

    // API 함수들 (모두 Doxygen 주석 포함)

    /// @brief 계정으로 로그인 요청을 전송합니다.
    /// @param Account [in] 사용자 계정 ID입니다.
    /// @param InDelegate [in] 응답 수신 시 호출될 델리게이트입니다.
    void RequestLogin(const FString& Account, FResponseLoginDelegate InDelegate);

    /// @brief 새 사용자 정보를 생성합니다.
    /// @param Character [in] 캐릭터 인덱스입니다.
    /// @param CharacterColor [in] 캐릭터 색상 인덱스입니다.
    /// @param InDelegate [in] 응답 수신 시 호출될 델리게이트입니다.
    void RequestCreateUser(int32 Character, int32 CharacterColor, FResponseCreateUserDelegate InDelegate);

    /// @brief 인터뷰 요청을 전송합니다.
    void RequestInterview(FResponseInterviewDelegate InDelegate);

    /// @brief 인터뷰 답변을 제출합니다.
    /// @param Answers [in] 인터뷰 답변 배열입니다.
    void RequestSubmitInterview(const TArray<FString>& Answers, FResponseInterviewDelegate InDelegate);

    /// @brief 게임 시작 요청을 전송합니다.
    /// @param PlayerList [in] 플레이어 ID 목록입니다.
    /// @param Nicknames [in] 플레이어 닉네임 목록입니다.
    void RequestStartGame(const TArray<FString>& PlayerList, const TArray<FString>& Nicknames, FResponseStartGameDelegate InDelegate);

    /// @brief 게임 로그인 요청을 전송합니다.
    void RequestGameLogin(FResponseGameLoginDelegate InDelegate);

    /// @brief 퀘스트 답변을 제출합니다.
    /// @param QuestStep [in] 퀘스트 단계입니다.
    /// @param QuestAnswer [in] 퀘스트 답변입니다.
    /// @param PlayTime [in] 플레이 시간입니다.
    void RequestQuestAnswer(int32 QuestStep, int32 QuestAnswer, float PlayTime, FResponseQuestAnswerDelegate InDelegate);

    /// @brief 퀘스트 실패를 보고합니다.
    /// @param QuestStep [in] 실패한 퀘스트 단계입니다.
    void RequestQuestFail(int32 QuestStep, FResponseQuestAnswerDelegate InDelegate);

    /// @brief Write 퀘스트 답변을 제출합니다 (Multipart Form Data 사용).
    /// @param ImagePaths [in] 이미지 파일 경로 배열입니다.
    /// @param TextData [in] 텍스트 데이터 배열입니다.
    void RequestQuestWrite(const TArray<FString>& ImagePaths, const TArray<FString>& TextData, FResponseQuestWriteDelegate InDelegate);

    /// @brief Speak 퀘스트 답변을 제출합니다 (Multipart Form Data 사용).
    /// @param SpeakStep [in] Speak 단계입니다.
    /// @param WavFilePath [in] WAV 오디오 파일 경로입니다.
    void RequestQuestSpeak(int32 SpeakStep, const FString& WavFilePath, FResponseQuestSpeakDelegate InDelegate);

    /// @brief 게임 결과를 요청합니다.
    void RequestGameResult(FResponseGameResultDelegate InDelegate);

private:
    /// @brief 공통 네트워크 로그 메시지를 출력합니다.
    static void LogNetwork(ENetworkLogType InLogType, const FString& URL, const FString& Body = "");

    /// @brief 네트워크 처리 대기 카운트를 갱신하고 UI에 통보합니다.
    void AddNetworkWaitCount(int Value);

private:
    /// @brief 처리 중인 요청 수를 집계하는 카운터입니다.
    int NetworkWaitCount = 0;

    /// @brief 인증 토큰 (로그인 후 저장)
    FString AuthToken;
};
```

**참고**: 기존 `UHttpNetworkSystem`과 동일한 패턴을 사용하며, `CreateRequest()` 같은 헬퍼 함수는 별도로 분리하지 않고 각 함수 내에서 직접 HTTP 요청을 생성합니다.

---

### 3. KLingoNetworkData.cpp & UKLingoNetworkSystem.cpp

**파일 위치**:
- `Source/Onepiece/Network/Private/KLingoNetworkData.cpp`
- `Source/Onepiece/Network/Private/UKLingoNetworkSystem.cpp`

**구현 요구사항**:

#### KLingoNetworkData.cpp
- 모든 응답 구조체의 `SetFromHttpResponse()` 구현
  - `FJsonObjectConverter::JsonObjectToUStruct()` 사용
  - 에러 처리 포함
- 모든 응답 구조체의 `PrintData()` 구현
  - `NETWORK_LOG` 매크로 사용
  - 주요 필드 출력

#### UKLingoNetworkSystem.cpp
- `Initialize()` 및 `Deinitialize()` 구현
- 모든 API 함수 구현 (기존 `UHttpNetworkSystem::RequestHealth()` 패턴 준수)
- HTTP 요청 생성 패턴:
  ```cpp
  void UKLingoNetworkSystem::RequestLogin(const FString& Account, FResponseLoginDelegate InDelegate)
  {
      FString Url = NetworkConfig::GetFullUrl(RequestAPI::KLingoLogin);

      NETWORK_LOG(ENetworkLogType::POST, TEXT("[POST] Login request: %s"), *Url);

      // HTTP 요청 생성
      TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
      Request->SetURL(Url);
      Request->SetVerb(TEXT("POST"));
      Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

      // JSON Body 생성
      TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
      JsonObject->SetStringField(TEXT("account"), Account);

      FString OutputString;
      TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
      FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
      Request->SetContentAsString(OutputString);

      // 응답 콜백 바인딩
      Request->OnProcessRequestComplete().BindLambda(
          [this, InDelegate](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSuccess)
          {
              AddNetworkWaitCount(-1);

              FResponseLogin ResponseData;

              if (bSuccess && HttpResponse.IsValid() && HttpResponse->GetResponseCode() == 200)
              {
                  ResponseData.SetFromHttpResponse(HttpResponse);
                  ResponseData.PrintData();

                  // AuthToken 저장
                  AuthToken = ResponseData.Token;

                  InDelegate.ExecuteIfBound(ResponseData, true);
              }
              else
              {
                  NETWORK_LOG(ENetworkLogType::ERROR, TEXT("[POST] Login failed"));
                  InDelegate.ExecuteIfBound(ResponseData, false);
              }
          });

      AddNetworkWaitCount(1);
      Request->ProcessRequest();
  }
  ```

**Multipart Form Data 사용 예제** (`RequestQuestWrite`, `RequestQuestSpeak`):
```cpp
void UKLingoNetworkSystem::RequestQuestWrite(const TArray<FString>& ImagePaths, const TArray<FString>& TextData, FResponseQuestWriteDelegate InDelegate)
{
    FString Url = NetworkConfig::GetFullUrl(RequestAPI::KLingoQuestWrite);

    NETWORK_LOG(ENetworkLogType::POST, TEXT("[POST] Quest Write: %s"), *Url);

    // Multipart Form Data 생성
    FHttpMultipartFormData FormData;
    FormData.AddText(TEXT("token"), AuthToken);

    // 이미지 파일 추가
    for (int32 i = 0; i < ImagePaths.Num(); ++i)
    {
        FormData.AddFile(TEXT("imageData"), ImagePaths[i], TEXT("image/png"));
    }

    // 텍스트 데이터 추가
    for (int32 i = 0; i < TextData.Num(); ++i)
    {
        FormData.AddText(FString::Printf(TEXT("textData[%d]"), i), TextData[i]);
    }

    // HTTP 요청 생성 및 설정
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(Url);
    Request->SetVerb(TEXT("POST"));
    FormData.SetupHttpRequest(Request);

    // 응답 콜백 바인딩
    Request->OnProcessRequestComplete().BindLambda(
        [this, InDelegate](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSuccess)
        {
            AddNetworkWaitCount(-1);

            FResponseQuestWrite ResponseData;

            if (bSuccess && HttpResponse.IsValid() && HttpResponse->GetResponseCode() == 200)
            {
                ResponseData.SetFromHttpResponse(HttpResponse);
                ResponseData.PrintData();
                InDelegate.ExecuteIfBound(ResponseData, true);
            }
            else
            {
                NETWORK_LOG(ENetworkLogType::ERROR, TEXT("[POST] Quest Write failed"));
                InDelegate.ExecuteIfBound(ResponseData, false);
            }
        });

    AddNetworkWaitCount(1);
    Request->ProcessRequest();
}
```

---

### 4. 빌드 설정

**파일**: `Source/Onepiece/Onepiece.Build.cs`

**이미 포함된 모듈** (변경 불필요):
- `HTTP`
- `Json`
- `JsonUtilities`

**추가 필요한 경우**: `ImageWrapper` (이미지 처리가 필요한 경우)

```csharp
PublicDependencyModuleNames.AddRange(new string[]
{
    "HTTP",
    "Json",
    "JsonUtilities",
    // "ImageWrapper", // 필요 시 추가
});
```

---

## 파일 생성 형식 예제

### KLingoNetworkData.h 헤더 예제

```cpp
// Copyright (c) 2025 Doppleddiggong. All rights reserved.
// Unauthorized copying, modification, or distribution of this file,
// via any medium is strictly prohibited. Proprietary and confidential.

/// @file KLingoNetworkData.h
/// @brief KLingo API 요청과 응답에 사용되는 구조체 및 델리게이트를 정의합니다.
#pragma once

#include "CoreMinimal.h"
#include "UCustomNetworkSettings.h"
#include "Templates/SharedPointer.h"
#include "KLingoNetworkData.generated.h"

// =================================================================================
// KLingo API Endpoints
// =================================================================================
namespace RequestAPI
{
    /// @brief KLingo 로그인 엔드포인트입니다.
    static FString KLingoLogin = FString("/api/v1/Login");

    // ... (나머지 엔드포인트)
}

// =================================================================================
// Response Structures
// =================================================================================

/// @brief 플레이어 정보 구조체입니다.
USTRUCT(BlueprintType)
struct FPlayerInfo
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "KLingo")
    int32 UserIndex = 0;

    UPROPERTY(BlueprintReadWrite, Category = "KLingo")
    FString Nickname;

    // ... (모든 필드)
};

/// @brief 로그인 응답 델리게이트입니다.
DECLARE_DELEGATE_TwoParams(FResponseLoginDelegate, FResponseLogin&, bool);

/// @brief 로그인 응답 구조체입니다.
USTRUCT(BlueprintType)
struct FResponseLogin
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "KLingo")
    FString Token;

    UPROPERTY(BlueprintReadWrite, Category = "KLingo")
    FPlayerInfo PlayerInfo;

    /// @brief HTTP 응답을 파싱해 구조체를 채웁니다.
    void SetFromHttpResponse(const TSharedPtr<class IHttpResponse, ESPMode::ThreadSafe>& Response);

    /// @brief 디버그 로그에 응답 내용을 출력합니다.
    void PrintData();
};

// ... (나머지 구조체 및 델리게이트)
```

### UKLingoNetworkSystem.h 헤더 예제

```cpp
// Copyright (c) 2025 Doppleddiggong. All rights reserved.
// Unauthorized copying, modification, or distribution of this file,
// via any medium is strictly prohibited. Proprietary and confidential.

/// @file UKLingoNetworkSystem.h
/// @brief KLingo API 요청을 담당하는 서브시스템을 선언합니다.
#pragma once

#include "CoreMinimal.h"
#include "KLingoNetworkData.h"
#include "ENetworkLogType.h"
#include "Macro.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UKLingoNetworkSystem.generated.h"

UCLASS()
class ONEPIECE_API UKLingoNetworkSystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    DEFINE_SUBSYSTEM_GETTER_INLINE(UKLingoNetworkSystem);

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // ... (API 함수들)

private:
    static void LogNetwork(ENetworkLogType InLogType, const FString& URL, const FString& Body = "");
    void AddNetworkWaitCount(int Value);

private:
    int NetworkWaitCount = 0;
    FString AuthToken;
};
```

---

## 코딩 스타일 가이드

### 1. 네이밍 컨벤션 (Onepiece 프로젝트 규칙)
- **클래스명**: `UKLingoNetworkSystem` (U 접두사, PascalCase)
- **구조체명**: `FResponseLogin` (F 접두사, PascalCase)
- **델리게이트명**: `FResponseLoginDelegate` (F 접두사, Delegate 접미사)
- **함수명**: `RequestLogin()` (PascalCase, Request 접두사 사용)
- **멤버 변수**: `AuthToken` (PascalCase)
- **로컬 변수**: `bSuccess` (camelCase with type prefix)
- **매개변수**: `InDelegate` (In 접두사 사용)

### 2. 주석 스타일 (Doxygen)
```cpp
/// @brief 간단한 설명입니다.
/// @param ParamName [in] 매개변수 설명입니다.
/// @return 반환 값 설명입니다.
void FunctionName(int32 ParamName);
```

### 3. 로깅 시스템
```cpp
// 네트워크 로그 (NETWORK_LOG 매크로 사용)
NETWORK_LOG(ENetworkLogType::POST, TEXT("[POST] Login request: %s"), *Url);
NETWORK_LOG(ENetworkLogType::ERROR, TEXT("[ERROR] Login failed: %s"), *ErrorMessage);

// 일반 로그 (필요 시)
PRINTLOG(TEXT("Debug message: %s"), *Message);
```

### 4. 에러 처리
```cpp
// HTTP 응답 검증
if (!bSuccess || !HttpResponse.IsValid() || HttpResponse->GetResponseCode() != 200)
{
    NETWORK_LOG(ENetworkLogType::ERROR, TEXT("[POST] Request failed"));
    InDelegate.ExecuteIfBound(ResponseData, false);
    return;
}

// 포인터 검증
if (!Ptr.IsValid() || Ptr == nullptr)
{
    // 에러 처리
}
```

### 5. 델리게이트 바인딩 패턴
```cpp
// Lambda 바인딩 (기본)
void UMyActor::CallAPI()
{
    UKLingoNetworkSystem* NetworkSystem = UKLingoNetworkSystem::Get(GetWorld());
    NetworkSystem->RequestLogin(
        Account,
        FResponseLoginDelegate::CreateLambda(
            [this](FResponseLogin& Response, bool bSuccess)
            {
                if (bSuccess)
                {
                    PRINTLOG(TEXT("Login Success: %s"), *Response.Token);
                }
            })
    );
}

// UObject 함수 바인딩
FResponseLoginDelegate::CreateUObject(this, &UMyClass::HandleLogin);

void UMyClass::HandleLogin(FResponseLogin& Response, bool bSuccess)
{
    if (bSuccess)
    {
        // 처리 로직
    }
}
```

### 6. JSON 파싱 패턴 (SetFromHttpResponse 구현)
```cpp
void FResponseLogin::SetFromHttpResponse(const TSharedPtr<IHttpResponse, ESPMode::ThreadSafe>& Response)
{
    if (!Response.IsValid())
    {
        return;
    }

    FString JsonString = Response->GetContentAsString();
    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

    if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
    {
        // FJsonObjectConverter 사용 (권장)
        FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), FResponseLogin::StaticStruct(), this);

        // 또는 수동 파싱
        Token = JsonObject->GetStringField(TEXT("token"));

        // 중첩 객체
        if (JsonObject->HasTypedField<EJson::Object>(TEXT("playerInfo")))
        {
            TSharedPtr<FJsonObject> PlayerInfoObj = JsonObject->GetObjectField(TEXT("playerInfo"));
            FJsonObjectConverter::JsonObjectToUStruct(PlayerInfoObj.ToSharedRef(), FPlayerInfo::StaticStruct(), &PlayerInfo);
        }

        // 배열
        const TArray<TSharedPtr<FJsonValue>>* Array;
        if (JsonObject->TryGetArrayField(TEXT("scores"), Array))
        {
            for (const auto& Item : *Array)
            {
                Scores.Add(Item->AsNumber());
            }
        }
    }
}
```

---

## 구현 체크리스트

생성된 코드가 다음을 모두 포함하는지 확인:

### KLingoNetworkData.h
- [ ] Copyright 헤더 포함
- [ ] Doxygen 파일 주석 포함
- [ ] RequestAPI 네임스페이스에 모든 엔드포인트 정의
- [ ] FPlayerInfo (userIndex, nickname, gold, character, characterColor, unlockEquip, login)
- [ ] FWordData (kor, eng, pronunciation)
- [ ] FReadTargetData, FQuestReadInfo
- [ ] FListenTargetData, FQuestListenInfo
- [ ] FWriteTargetData, FWriteTeachData, FQuestWriteInfo
- [ ] FQuestSpeakInfo
- [ ] 모든 응답 구조체 (FResponseLogin, FResponseStartGame 등)
- [ ] 모든 델리게이트 정의 (FResponseLoginDelegate 등)
- [ ] 모든 구조체 USTRUCT(BlueprintType) + GENERATED_BODY()

### UKLingoNetworkSystem.h
- [ ] Copyright 헤더 포함
- [ ] Doxygen 파일 및 함수 주석 포함
- [ ] ONEPIECE_API 매크로 사용
- [ ] UGameInstanceSubsystem 상속
- [ ] DEFINE_SUBSYSTEM_GETTER_INLINE 매크로 사용
- [ ] Initialize/Deinitialize 오버라이드
- [ ] 11개 API 함수 선언 (모두 Doxygen 주석 포함)
- [ ] LogNetwork, AddNetworkWaitCount private 함수
- [ ] NetworkWaitCount, AuthToken 멤버 변수

### KLingoNetworkData.cpp
- [ ] Copyright 헤더 포함
- [ ] 모든 응답 구조체의 SetFromHttpResponse() 구현
- [ ] 모든 응답 구조체의 PrintData() 구현
- [ ] NETWORK_LOG 매크로 사용
- [ ] FJsonObjectConverter 활용

### UKLingoNetworkSystem.cpp
- [ ] Copyright 헤더 포함
- [ ] Initialize/Deinitialize 구현
- [ ] RequestLogin() 완전 구현
- [ ] RequestCreateUser() 완전 구현
- [ ] RequestStartGame() 완전 구현
- [ ] RequestQuestAnswer() 완전 구현
- [ ] RequestQuestWrite() multipart 구현 (FHttpMultipartFormData 사용)
- [ ] RequestQuestSpeak() multipart 구현 (FHttpMultipartFormData 사용)
- [ ] RequestGameResult() 완전 구현
- [ ] LogNetwork() 구현
- [ ] AddNetworkWaitCount() 구현
- [ ] NetworkConfig::GetFullUrl() 사용
- [ ] NETWORK_LOG 매크로 사용

### Onepiece.Build.cs
- [ ] HTTP, Json, JsonUtilities 모듈 이미 포함됨 (확인만 필요)

---

## 사용 예제

### 기본 사용법 (로그인)

```cpp
// MyActor.h
UCLASS()
class ONEPIECE_API AMyActor : public AActor
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;

private:
    void HandleLogin(FResponseLogin& Response, bool bSuccess);
};

// MyActor.cpp
void AMyActor::BeginPlay()
{
    Super::BeginPlay();

    // 서브시스템 가져오기
    UKLingoNetworkSystem* NetworkSystem = UKLingoNetworkSystem::Get(GetWorld());

    // 로그인 요청
    NetworkSystem->RequestLogin(
        TEXT("user123"),
        FResponseLoginDelegate::CreateUObject(this, &AMyActor::HandleLogin)
    );
}

void AMyActor::HandleLogin(FResponseLogin& Response, bool bSuccess)
{
    if (bSuccess)
    {
        PRINTLOG(TEXT("Login Success! Token: %s"), *Response.Token);
        PRINTLOG(TEXT("Player: %s (Gold: %d)"), *Response.PlayerInfo.Nickname, Response.PlayerInfo.Gold);
    }
    else
    {
        PRINTLOG(TEXT("Login Failed"));
    }
}
```

### Quest Write 사용 예제

```cpp
void AMyActor::SubmitWriteQuest()
{
    UKLingoNetworkSystem* NetworkSystem = UKLingoNetworkSystem::Get(GetWorld());

    TArray<FString> ImagePaths;
    ImagePaths.Add(TEXT("C:/Images/answer1.png"));
    ImagePaths.Add(TEXT("C:/Images/answer2.png"));

    TArray<FString> TextData;
    TextData.Add(TEXT("Hello"));
    TextData.Add(TEXT("World"));

    NetworkSystem->RequestQuestWrite(
        ImagePaths,
        TextData,
        FResponseQuestWriteDelegate::CreateLambda(
            [](FResponseQuestWrite& Response, bool bSuccess)
            {
                if (bSuccess)
                {
                    PRINTLOG(TEXT("Quest Write Success!"));
                }
            })
    );
}
```

### Quest Speak 사용 예제

```cpp
void AMyActor::SubmitSpeakQuest()
{
    UKLingoNetworkSystem* NetworkSystem = UKLingoNetworkSystem::Get(GetWorld());

    FString WavFilePath = TEXT("C:/Audio/answer.wav");

    NetworkSystem->RequestQuestSpeak(
        1, // SpeakStep
        WavFilePath,
        FResponseQuestSpeakDelegate::CreateLambda(
            [](FResponseQuestSpeak& Response, bool bSuccess)
            {
                if (bSuccess)
                {
                    PRINTLOG(TEXT("Quest Speak Success!"));
                }
            })
    );
}
```

---

## 구현 시 주의사항

### 1. 프로젝트 구조 준수
- 파일은 반드시 `Source/Onepiece/Network/Public` 및 `Private` 폴더에 생성
- 기존 `UHttpNetworkSystem` 패턴을 최대한 유지

### 2. 코딩 컨벤션 준수
- Copyright 헤더 필수
- Doxygen 주석 스타일 사용
- ONEPIECE_API 매크로 사용
- NETWORK_LOG 매크로 사용
- 델리게이트는 Onepiece 패턴 준수 (Struct&, bool)

### 3. 네트워크 설정 활용
- `NetworkConfig::GetFullUrl()` 사용
- `UCustomNetworkSettings`로 Dev/Stage/Live 환경 자동 처리
- AuthToken은 로그인 후 자동 저장 및 이후 요청에 사용

### 4. Multipart Form Data
- `FHttpMultipartFormData` 클래스 활용 (프로젝트에 이미 구현됨)
- 파일 경로를 직접 전달하면 자동으로 로드 및 전송

### 5. 에러 처리
- HTTP 응답 코드 200 체크
- 델리게이트 ExecuteIfBound 사용
- 네트워크 대기 카운터 관리 (AddNetworkWaitCount)

---

## 최종 출력 요청

위 모든 요구사항을 반영하여 다음 파일들을 **완전한 코드**로 생성해주세요:

1. **KLingoNetworkData.h** - 모든 구조체, 델리게이트, 네임스페이스
2. **KLingoNetworkData.cpp** - SetFromHttpResponse, PrintData 구현
3. **UKLingoNetworkSystem.h** - 서브시스템 헤더
4. **UKLingoNetworkSystem.cpp** - 모든 API 함수 구현

**생성 조건**:
- Onepiece 프로젝트 컨벤션 100% 준수
- 복사-붙여넣기로 즉시 컴파일 가능
- 모든 함수 완전 구현 (TODO, 빈 함수 없음)
- Doxygen 주석 포함
- 에러 처리 포함
- NETWORK_LOG 매크로 사용
