# 퀘스트 시스템 리팩토링 및 네트워크 API 분화 기술 리포트

**작성일**: 2025-12-06
**작성자**: Claude AI Assistant
**카테고리**: System / Network / Refactoring
**심각도**: High

---

## 📋 목차

1. [문제 개요](#1-문제-개요)
2. [기존 시스템 아키텍처](#2-기존-시스템-아키텍처)
3. [문제 원인 분석](#3-문제-원인-분석)
4. [해결 방안](#4-해결-방안)
5. [핵심 코드 수정 내역](#5-핵심-코드-수정-내역)
6. [테스트 가이드](#6-테스트-가이드)
7. [학습 포인트](#7-학습-포인트)

---

## 1. 문제 개요

### 1.1 증상
- **현상**: 모든 종류의 퀘스트(읽기, 듣기 등)가 단일 `FResponseScenario` 데이터 구조와 `RequestScenario` API 호출을 공유하여 사용하고 있었습니다. 이로 인해 어떤 퀘스트에서 어떤 데이터 필드가 사용되는지 명확하지 않았고, 새로운 퀘스트 타입을 추가하거나 기존 로직을 수정하기 어려운 구조였습니다.
- **영향**: 코드의 가독성이 저하되고, 데이터 파싱 오류 발생 가능성이 높으며, 시스템 확장 시 많은 부분을 수정해야 하는 유지보수의 어려움이 있었습니다.

### 1.2 관련 시스템
- `UKLingoNetworkSystem`: 서버 API 호출 관리 시스템
- `ALingoGameMode`, `ALingoGameState`: 게임 로직 및 상태 관리
- `AContactTrigger`: 퀘스트 시작 트리거
- `NetworkData.h`: 네트워크 데이터 구조체 정의
- `UPopup_ReadQuest`, `UPopup_Result`: 퀘스트 관련 UI
- `ScoreManager`: 점수 계산 로직 (삭제됨)

---

## 2. 기존 시스템 아키텍처

### 2.1 범용 API 호출 구조

```
Client (Actor)         GameMode                NetworkSystem          Server
      |                    |                         |                    |
      |-- OnTrigger()----->|                         |                    |
      |                    |-- BeginQuest()--------->|                    |
      |                    |                         |-- RequestScenario()->|
      |                    |                         |                    |-- Generic /scenario
      |                    |                         |<-- FResponseScenario|
      |                    |<------------------------|                    |
      |<-------------------| (Multicast Show UI)     |                    |
```
- 모든 퀘스트가 동일한 `RequestScenario` API를 통해 `FResponseScenario`라는 범용 구조체를 받아 처리했습니다.

---

## 3. 문제 원인 분석

### 3.1 핵심 문제
**범용 자료구조와 API로 인한 시스템의 경직성 및 모호성**이 핵심 문제입니다.

### 3.2 상세 분석

#### 3.2.1 데이터 구조의 모호성
`FResponseScenario` 구조체는 읽기 퀘스트 데이터, 듣기 퀘스트 데이터(`voice_data`), 심지어 공통 데이터(`full_data`)까지 모두 포함하고 있었습니다. 이로 인해 특정 퀘스트에서 어떤 필드를 사용해야 하는지 코드만 보고 파악하기 어려웠습니다.

#### 3.2.2 API의 불명확성
`RequestScenario`와 `RequestQuestResult`라는 이름의 API는 어떤 종류의 퀘스트를 처리하는지 명확히 나타내지 않았습니다. 이는 서버와 클라이언트 양쪽에서 분기 처리를 복잡하게 만드는 원인이 되었습니다.

#### 3.2.3 불필요한 클래스 존재
단순 점수 및 등급 계산 로직이 `UScoreManager`라는 별도 서브시스템에 존재했습니다. 이 기능은 간단한 유틸리티 함수로 충분히 대체 가능하여 불필요한 복잡성을 더했습니다.

---

## 4. 해결 방안

### 4.1 해결 전략
각 퀘스트 타입(읽기, 듣기)에 맞는 **특화된(Specialized) 데이터 구조체와 네트워크 API 함수를 분리**하여 시스템의 명확성과 확장성을 높이는 방향으로 리팩토링을 진행했습니다.

### 4.2 해결 로직
1.  **데이터 구조체 분화**: `FResponseScenario`를 `FResponseReadScenario`, `FResponseListenScenario` 등으로 분리하고, 결과 전송용 구조체도 `FRequestReadResult`, `FResponseReadResult` 등으로 명확히 구분했습니다.
2.  **네트워크 API 분화**: `UKLingoNetworkSystem` 내 `RequestScenario` 함수를 제거하고, `RequestReadScenario`, `RequestListenScenario` 등 각 퀘스트에 맞는 API 요청 함수를 새로 정의했습니다.
3.  **게임 로직 수정**: `ALingoGameMode`, `ALingoGameState` 등이 새로운 특화 구조체를 사용하도록 로직을 수정했습니다. 예를 들어, `ALingoGameState`는 `CurScenarioData` 대신 `ReadScenarioData`, `ListenScenarioData`를 갖게 됩니다.
4.  **유틸리티 로직 통합**: `ScoreManager`를 삭제하고, `GetTimeRank`, `GetAccuracyPercentage` 같은 점수 계산 함수를 `ULingoGameHelper`의 static 함수로 이전하여 재사용성을 높였습니다.

### 4.3 해결 후 플로우
```
Client (AContactTrigger)   GameMode                NetworkSystem             Server
        |                      |                         |                       |
        |-- OnTrigger()------>|                         |                       |
        |                      |-- BeginReadQuest()----->|                       |
        |                      |                         |-- RequestReadScenario()->|
        |                      |                         |                       |-- Specific /scenario/.../read
        |                      |                         |<-- FResponseReadScenario|
        |                      |<------------------------|                       |
        |<---------------------| (Multicast_ShowReadQuestPopup)                  |
```
- 이제 각 퀘스트는 명확히 분리된 API와 데이터 구조를 통해 통신합니다.

---

## 5. 핵심 코드 수정 내역

### 5.1 수정 파일
- `Source/Onepiece/Network/Public/NetworkData.h`
- `Source/Onepiece/Network/Private/UKLingoNetworkSystem.cpp`
- `Source/Onepiece/Game/Public/ALingoGameMode.h`
- `Source/Onepiece/Game/Public/ALingoGameState.h`
- `Source/Onepiece/MessageBox/Public/ScoreManager.h` (삭제)

### 5.2 `NetworkData.h`: 구조체 분화
- **수정 전**:
```cpp
// FResponseScenario: 모든 퀘스트 정보를 담던 범용 구조체
USTRUCT()
struct FResponseScenario { ... };

// FRequestReadQuestResult: 범용 결과 요청 구조체
USTRUCT()
struct FRequestReadQuestResult { ... };
```
- **수정 후**:
```cpp
// FResponseReadScenario: 읽기 퀘스트 전용 응답 구조체
USTRUCT()
struct FResponseReadScenario { ... };

// FResponseListenScenario: 듣기 퀘스트 전용 응답 구조체
USTRUCT()
struct FResponseListenScenario { ... };

// FRequestReadResult: 읽기 퀘스트 결과 전용 요청 구조체
USTRUCT()
struct FRequestReadResult { ... };
```

### 5.3 `UKLingoNetworkSystem.cpp`: API 함수 분화
- **수정 전**:
```cpp
void UKLingoNetworkSystem::RequestScenario(int32 Index, int32 Difficulty, ..., FResponseScenarioDelegate InDelegate);
void UKLingoNetworkSystem::RequestQuestResult(const FRequestReadQuestResult& Result, ...);
```
- **수정 후**:
```cpp
void UKLingoNetworkSystem::RequestReadScenario(FResponseReadScenarioDelegate InDelegate);
void UKLingoNetworkSystem::RequestListenScenario(FResponseListenScenarioDelegate InDelegate);
void UKLingoNetworkSystem::RequestReadResult(const FRequestReadResult& Result, FResponseReadResultDelegate InDelegate);
void UKLingoNetworkSystem::RequestListenResult(const FRequestListenResult& Result, FResponseListenResultDelegate InDelegate);
```

### 5.4 `ScoreManager` 삭제 및 `ULingoGameHelper`로 통합
- `ScoreManager.cpp`, `ScoreManager.h` 파일이 프로젝트에서 삭제되었습니다.
- `ULingoGameHelper.h`에 아래 함수들이 추가되었습니다.
```cpp
static FString GetTimeRank(float InTimeTaken);
static FString GetAccuracyPercentage(int WrongCnt);
```

---

## 6. 테스트 가이드

### 6.1 테스트 시나리오
1.  **읽기 퀘스트 테스트**:
    -   `AContactTrigger`와 상호작용하여 읽기 퀘스트를 시작합니다.
    -   `UPopup_ReadQuest` 팝업이 정상적으로 뜨고, 분리된 역할에 맞는 텍스트(`word_data1`, `word_data2`, `full_data`)가 표시되는지 확인합니다.
    -   퀘스트를 완료한 후 `UPopup_Result` 팝업에서 소요 시간, 정확도, 등급이 올바르게 계산되어 표시되는지 확인합니다.
    -   서버 로그를 통해 `RequestReadResult` API가 정상적으로 호출되었는지 확인합니다.
2.  **듣기 퀘스트 테스트**:
    -   듣기 퀘스트용 `AContactTrigger`와 상호작용하여 퀘스트를 시작합니다.
    -   `UPopup_ReadQuest` 팝업이 뜨고, `voice_data`가 정상적으로 재생되는지 확인합니다.
    -   (구현되었다면) 결과 전송 및 결과 팝업이 정상 동작하는지 확인합니다.

### 6.2 Output Log 필터링
- `LogKLingo`: 네트워크 통신 관련 로그
- `LogLingo`: 게임 플레이 로직 관련 로그
- `[RES]`: 서버 응답 전문 확인

---

## 7. 학습 포인트

### 7.1 특화(Specialization)의 중요성
범용적인 설계는 초기 개발 속도를 높일 수 있지만, 프로젝트가 복잡해질수록 유지보수를 어렵게 만듭니다. 각 기능의 역할과 데이터에 맞게 시스템을 분화하고 특화하는 것은 장기적으로 코드의 안정성과 확장성을 확보하는 데 매우 중요합니다.

### 7.2 응집도(Cohesion)와 결합도(Coupling)
`ScoreManager`를 삭제하고 관련 함수를 `ULingoGameHelper`로 옮긴 것은 좋은 리팩토링 사례입니다. 단순 계산 유틸리티는 특정 상태를 관리하는 'Manager' 클래스보다, 어디서든 쉽게 접근할 수 있는 static 함수 모음으로 제공하는 것이 더 효율적입니다. 이는 클래스의 **응집도**를 높이고 불필요한 **결합도**를 낮추는 데 기여합니다.

---

**문서 버전**: 1.0
**최종 수정일**: 2025-12-06
