# Onepiece 프로젝트 코딩 컨벤션 (Project-Specific Rules)

**[Base Rules]** `Project/ue_coding_conventions.md`

이 문서는 Onepiece 프로젝트 고유의 코딩 컨벤션을 정의합니다. 모든 기본 Unreal Engine C++ 코딩 규칙은 `Project/ue_coding_conventions.md`에서 상속받으며, 아래에는 Onepiece 프로젝트에만 적용되는 특화 규칙을 기술합니다.

---

## 1. 프로젝트 정보

- **프로젝트명**: Onepiece
- **모듈명**: ONEPIECE
- **주요 라이브러리**: CoffeeLibrary, LatteLibrary

---

## 2. 파일 헤더 주석

모든 소스 파일 상단에 다음 저작권 및 라이선스 정보를 포함한 주석을 추가합니다.

```cpp
// Copyright (c) 2025 Doppleddiggong. All rights reserved.
// Unauthorized copying, modification, or distribution of this file,
// via any medium is strictly prohibited. Proprietary and confidential.
```

---

## 3. 로깅 시스템

### 3.1. 일반 로깅

- **매크로**: `PRINTLOG`
- **용도**: 일반적인 디버그 및 정보성 메시지 기록

### 3.2. 네트워크 로깅

- **매크로**: `NETWORK_LOG`
- **열거형**: `ENetworkLogType`
- **접두사**: `[GET]`, `[POST]`, `[WS]` 등을 활용하여 로그의 종류를 명확히 구분
- **용도**: 네트워크 통신 관련 로그 기록

---

## 4. 네트워크 모듈 컨벤션 (Network Module Conventions)

Onepiece 프로젝트는 CoffeeLibrary와 LatteLibrary를 활용한 네트워크 시스템을 사용합니다.

### 4.1. 서브시스템 활용

- **기반 클래스**: `UGameInstanceSubsystem`을 상속받아 네트워크 시스템을 구현합니다.
- **전역 접근**: `DEFINE_SUBSYSTEM_GETTER_INLINE` 매크로를 사용하여 전역 접근을 용이하게 합니다.

### 4.2. 서버 설정 관리

- **설정 클래스**: `UCustomNetworkSettings` (`UDeveloperSettings` 파생)
- **용도**: 개발(Dev), 스테이지(Stage), 라이브(Live) 환경별 서버 주소 및 포트 설정 관리
- **동적 변경**: 명령줄 인수를 통해 서버 모드를 동적으로 변경 가능

### 4.3. API 엔드포인트 정의

- **네임스페이스**: `RequestAPI`
- **형식**: 정적 `FString` 변수로 각 API 엔드포인트를 정의하여 일관성 유지

```cpp
namespace RequestAPI
{
    static const FString Health = TEXT("/api/health");
    static const FString Login = TEXT("/api/auth/login");
    // ... 기타 엔드포인트
}
```

### 4.4. URL 구성

- **네임스페이스**: `NetworkConfig`
- **방법**: 정적 함수를 통해 `UCustomNetworkSettings`에서 현재 서버 모드에 맞는 `BaseUrl`과 `Port`를 가져와 전체 URL을 구성

```cpp
namespace NetworkConfig
{
    static FString GetFullUrl(const FString& Endpoint)
    {
        const UCustomNetworkSettings* Settings = GetDefault<UCustomNetworkSettings>();
        return FString::Printf(TEXT("%s:%d%s"),
            *Settings->GetCurrentBaseUrl(),
            Settings->GetCurrentPort(),
            *Endpoint);
    }
}
```

### 4.5. HTTP 요청

- **모듈**: `FHttpModule`
- **데이터 변환**: `FJsonObjectConverter`를 통해 USTRUCT와 JSON 간의 데이터 변환 수행
- **요청 타입**: GET, POST 등의 HTTP 메서드 사용

### 4.6. WebSocket 통신

- **모듈**: `FWebSocketsModule`
- **기능**: WebSocket 연결, 메시지 송수신, 연결 상태 관리

### 4.7. 응답 데이터 구조

서버 응답 데이터를 위한 `USTRUCT` 정의 및 표준 메서드 포함:

```cpp
USTRUCT(BlueprintType)
struct FResponseHealth
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FString Status;

    UPROPERTY(BlueprintReadOnly)
    int32 Code;

    // 헬퍼 메서드
    void SetFromHttpResponse(const FString& JsonString);
    void PrintData() const;
};
```

**필수 헬퍼 메서드**:
- `SetFromHttpResponse`: JSON 응답을 파싱하여 구조체에 설정
- `PrintData`: 로깅을 위한 데이터 출력

### 4.8. 네트워크 로깅 규칙

- **매크로**: `NETWORK_LOG(LogType, Format, ...)`
- **열거형**: `ENetworkLogType` (예: `GET`, `POST`, `WS`, `ERROR`)
- **형식**: `[타입] 메시지` 형태로 통일

예시:
```cpp
NETWORK_LOG(ENetworkLogType::GET, TEXT("[GET] Requesting health check: %s"), *Url);
NETWORK_LOG(ENetworkLogType::POST, TEXT("[POST] Sending login data"));
NETWORK_LOG(ENetworkLogType::WS, TEXT("[WS] Connected to server"));
```

### 4.9. 델리게이트 사용

**C++ 콜백** (비동기 응답 처리):
```cpp
DECLARE_DELEGATE_TwoParams(FOnHealthResponse, bool /*bSuccess*/, const FResponseHealth& /*Response*/);
```

**블루프린트 이벤트** (블루프린트 바인딩):
```cpp
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthResponseBP, bool, bSuccess, const FResponseHealth&, Response);
```

---

## 5. 모듈별 네이밍

### 5.1. API 매크로

프로젝트 전역에서 `ONEPIECE_API` 매크로를 사용하여 클래스를 외부에 공개합니다.

```cpp
UCLASS()
class ONEPIECE_API AMyActor : public AActor
{
    GENERATED_BODY()
    // ...
};
```

---

## 6. 프로젝트 특화 예시

### 6.1. 네트워크 서브시스템 예시

```cpp
UCLASS()
class ONEPIECE_API UOnepieceNetworkSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // 서브시스템 전역 접근자
    DEFINE_SUBSYSTEM_GETTER_INLINE(UOnepieceNetworkSubsystem)

    // HTTP GET 요청
    void RequestHealth(FOnHealthResponse Callback);

    // WebSocket 연결
    void ConnectWebSocket(const FString& Url);

protected:
    TSharedPtr<IWebSocket> WebSocket;
};
```

### 6.2. 네트워크 응답 처리 예시

```cpp
void UOnepieceNetworkSubsystem::RequestHealth(FOnHealthResponse Callback)
{
    FString Url = NetworkConfig::GetFullUrl(RequestAPI::Health);

    NETWORK_LOG(ENetworkLogType::GET, TEXT("[GET] Health check: %s"), *Url);

    TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(Url);
    Request->SetVerb(TEXT("GET"));

    Request->OnProcessRequestComplete().BindLambda(
        [Callback](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess)
        {
            FResponseHealth ResponseData;

            if (bSuccess && Response.IsValid())
            {
                ResponseData.SetFromHttpResponse(Response->GetContentAsString());
                ResponseData.PrintData();
                Callback.ExecuteIfBound(true, ResponseData);
            }
            else
            {
                NETWORK_LOG(ENetworkLogType::ERROR, TEXT("[GET] Failed to get health"));
                Callback.ExecuteIfBound(false, ResponseData);
            }
        });

    Request->ProcessRequest();
}
```

---

## 7. 추가 참고사항

- 모든 네트워크 관련 코드는 CoffeeLibrary 및 LatteLibrary의 API를 준수해야 합니다.
- 서버 환경 전환 시 반드시 `UCustomNetworkSettings`를 통해 설정을 변경해야 합니다.
- 네트워크 요청은 비동기로 처리하며, 델리게이트를 통해 응답을 처리합니다.
