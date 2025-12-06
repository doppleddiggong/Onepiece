# Dropper & Holder 시스템 구현 리포트

**작성일**: 2025-12-06
**작성자**: Claude AI Assistant
**카테고리**: Gameplay
**심각도**: Low

---

## 📋 목차

1. [문제 개요](#1-문제-개요)
2. [시스템 아키텍처](#2-시스템-아키텍처)
3. [핵심 컴포넌트 상세 분석](#3-핵심-컴포넌트-상세-분석)
4. [해결 방안](#4-해결-방안)
5. [코드 구현 내역](#5-코드-구현-내역)
6. [테스트 가이드](#6-테스트-가이드)
7. [학습 포인트](#7-학습-포인트)
8. [결론](#8-결론)

---

## 1. 문제 개요

### 1.1 증상
- **현상**: 플레이어에게 단어 조합 퍼즐을 제공하고, 정답을 제출 및 판정하는 시스템의 필요성이 제기되었습니다. 특정 아이템(`luggage`)을 드로퍼(`ADropper`)로부터 받아 홀더(`AHolder`)에 놓아 정답 여부를 확인하는 게임플레이 메커니즘을 구현해야 했습니다.
- **재현**: (신규 기능 구현이므로 해당 없음. 사용 시나리오에서 재현 방법을 대체 설명)
- **영향**: 이 시스템은 게임 내 퍼즐 요소를 추가하고 플레이어의 상호작용 경험을 확장하여 게임의 깊이와 재미를 더합니다.

### 1.2 관련 시스템
- `APlayerControl`: 플레이어 입력 처리 및 `ADropper`와의 상호작용 요청
- `ADropper`: `luggage` 액터 스폰 관리
- `Aluggage`: 단어 정보를 포함하는 상호작용 가능한 아이템
- `AHolder`: `luggage`를 받아 정답을 판정하는 로직
- `UInteractionSystem`: 플레이어의 일반적인 상호작용 (아이템 줍기/놓기) 처리

---

## 2. 시스템 아키텍처

### 2.1 Dropper & Holder 시스템 구조

```
┌────────────────────────────────┐
│        APlayerControl          │
│  - Input Handling              │
│  - RequestDrop()               │
│  - Server_OnGrab()/Drop()/Interact() │
└────────────────────────────────┘
           │ Input Event (e.g., Jump, Grab, Drop)
           │
           │ 1. Dropper 스폰 요청
           │ 2. InteractionSystem 호출 (PickUp/Drop)
           ▼
┌────────────────────────────────┐
│          ADropper              │
│  - Spawns Aluggage actors      │
│  - Manages spawn data          │
│  - Networked spawn process     │
└────────────────────────────────┘
           │ 3. Aluggage 스폰
           ▼
┌────────────────────────────────┐
│          Aluggage              │
│  - Contains FLuggageData       │
│  - InteractableComponent       │
│  - Physical object             │
└────────────────────────────────┘
           │ 4. Player Interaction (PickUp/Drop)
           ▼
┌────────────────────────────────┐
│          AHolder               │
│  - Detects overlapping Aluggage│
│  - Validates Aluggage data     │
│  - Triggers OnActivate event   │
│  - Networked state             │
└────────────────────────────────┘
```
(참고: 기존 다이어그램과 흐름을 통합하여 재구성)

---

## 3. 핵심 컴포넌트 상세 분석

### 3.1 ADropper: 동적 `luggage` 스폰

`ADropper`는 퍼즐 아이템인 `Aluggage`를 지정된 위치에 생성하는 역할을 담당합니다. 외부에서 어떤 클래스와 데이터를 스폰할지 주입받아 유연하게 작동합니다.

#### 3.1.1 주요 속성
```cpp
// 스폰할 클래스
UPROPERTY(Replicated)
TSubclassOf<AActor> SpawnClass;

// 스폰할 액터에게 전달할 데이터 (FLuggageData: 단어 정보 포함)
UPROPERTY(Replicated)
FLuggageData NextData;

// 스폰 처리 중인지 여부 (중복 요청 방지)
UPROPERTY(Replicated)
bool bIsSpawnIng = false;
```
- `bIsSpawnIng` 플래그는 스폰 작업이 진행 중임을 나타내어, 여러 스폰 요청이 동시에 처리되는 것을 방지합니다. 이 값은 `Replicated`되어 모든 클라이언트에서 일관된 상태를 유지합니다.

#### 3.1.2 핵심 메서드
- `SetSpawnClass(TSubclassOf<AActor> InClass)`: 스폰할 `AActor` 파생 클래스를 지정합니다.
- `SetSpawnData(const FLuggageData& InData)`: 스폰될 액터에게 전달할 `FLuggageData` 구조체를 설정합니다.
- `RequestSpawn()`: 서버에 액터 스폰을 요청하는 진입점입니다. `bIsSpawnIng` 상태를 확인하여 유효성을 검사합니다.
- `Spawn()`: 서버에서 실제 액터를 생성(`GetWorld()->SpawnActor()`)하고, 애니메이션 및 충돌 상태 변경(`Multicast` RPC)을 모든 클라이언트에 동기화합니다.

### 3.2 AHolder: `luggage` 검증 및 정답 판정

`AHolder`는 플레이어가 배치한 `Aluggage`를 감지하고, 해당 `luggage`가 사전에 설정된 정답 조건과 일치하는지 판정하는 발판 역할을 합니다.

#### 3.2.1 주요 속성
```cpp
// 현재 홀더 위에 올라와 있는 액터
UPROPERTY(ReplicatedUsing=OnRep_CurTarget)
TObjectPtr<class AActor> CurTarget;

// 홀더의 활성화(정답) 상태
UPROPERTY(ReplicatedUsing=OnRep_IsActivated)
bool bIsActivated = false;

// 이 홀더가 기대하는 정답 데이터 인덱스
int32 AnswerColorIdx = -1;
int32 AnswerPatternIdx = -1;
```
- `bIsActivated`는 정답 `luggage`가 홀더에 놓였을 때 `true`가 되며, `OnRep_IsActivated`를 통해 시각적 피드백을 모든 클라이언트에 동기화합니다.

#### 3.2.2 핵심 메서드
- `SetAnswerData(int32 InAnswerColorIdx, int32 InAnswerPatternIdx)`: 이 홀더가 정답으로 판정할 `luggage`의 특정 데이터(색상, 패턴 인덱스)를 설정합니다.
- `OnBoxOverlapBegin(...)`: `BoxCollision` 컴포넌트를 통해 다른 액터(`Aluggage` 예상)가 오버랩되기 시작할 때 호출됩니다. 이벤트를 통해 오버랩된 액터가 `Aluggage`인지 확인하고 `CheckLuggage()`를 호출합니다.
- `CheckLuggage(Aluggage* TargetLuggage)`: 오버랩된 `Aluggage`의 `FLuggageData`와 `AHolder`에 설정된 `AnswerColorIdx`, `AnswerPatternIdx`를 비교하여 정답 여부를 판정합니다.
- `OnActivate(bool bSuccess)`: 정답 판정 결과(`bSuccess`)를 전달하는 `BlueprintImplementableEvent`입니다. 블루프린트에서 이를 구현하여 홀더의 시각적 변화(색상 변경, VFX/SFX 재생 등)를 처리할 수 있습니다.

### 3.3 APlayerControl: 입력 및 시스템 연동

`APlayerControl`은 플레이어의 입력(`Jump`, `Grab`, `Drop`, `Interact`)을 받아 게임 내 행동을 지시하고, 특히 `ADropper`에게 `luggage` 스폰을 요청하며 `UInteractionSystem`을 활용하여 `luggage`를 조작하는 연결점 역할을 합니다.

#### 3.3.1 핵심 메서드
- `TEST_DropperDropProcess()`: 현재 테스트 목적으로 `Jump` 입력에 바인딩되어 있으며, `ADropper`를 찾아 `luggage` 스폰을 요청하는 함수입니다. 실제 게임에서는 다른 트리거(예: UI 버튼)에 연결될 수 있습니다.
- `RequestDrop(APlayerControl* Requester)`: 월드에 존재하는 `ADropper` 액터를 찾아서 `FLuggageData`와 스폰할 `Aluggage` 클래스를 설정한 후, `ADropper::RequestSpawn()`을 호출하여 스폰 프로세스를 시작합니다. 이 함수는 서버에서 실행되어야 합니다.
- `Server_OnGrab()`, `Server_OnGrabRelease()`, `Server_OnInteract()`: 플레이어의 `Grab`, `Drop`, `Interact` 입력에 대응하는 서버 측 RPC 함수들입니다. 이 함수들은 `APlayerActor`에 부착된 `UInteractionSystem`을 호출하여 `TryPickUp()`, `TryDrop()`, `TryInteract()` 등의 상호작용을 실행합니다.

---

## 4. 해결 방안

### 4.1 해결 전략
Dropper & Holder 시스템은 다음과 같은 전략을 기반으로 설계 및 구현되었습니다.
- **모듈화된 역할 분담**: `ADropper`, `AHolder`, `APlayerControl` 각각 명확한 역할(스폰, 판정, 입력/요청)을 가지도록 분리하여 시스템의 복잡도를 낮추고 유지보수성을 높였습니다.
- **네트워크 최우선 설계**: 모든 핵심 상태 변화 및 중요한 게임플레이 로직은 서버-클라이언트 간 동기화를 고려하여 RPC(`Server`, `Multicast`) 및 `Replicated` 속성을 적극 활용했습니다.
- **확장성 및 유연성**: `ADropper`는 스폰할 클래스와 데이터를 외부에서 주입받아 다양한 종류의 아이템을 스폰할 수 있으며, `AHolder`는 `BlueprintImplementableEvent`를 통해 블루프린트에서 판정 후의 동작을 자유롭게 커스터마이징할 수 있습니다.
- **기존 시스템 활용**: 플레이어의 아이템 조작은 이미 구현된 `UInteractionSystem`을 활용하여 개발 비용을 절감하고 일관된 사용자 경험을 제공합니다.

### 4.2 해결 로직 (전체 흐름도)

**1. 스폰 요청 (APlayerControl → ADropper)**
*   플레이어의 특정 입력(현재 `Jump` 키에 테스트용으로 바인딩된 `TEST_DropperDropProcess()`)이 `APlayerControl`에서 감지됩니다.
*   `APlayerControl::RequestDrop()` 함수(서버 실행)는 월드 내 모든 `ADropper` 액터를 순회하며 스폰 가능한 `ADropper`를 찾습니다.
*   찾은 `ADropper`에 스폰할 `Aluggage` 클래스(`BP_Luggage_C`)와 무작위 `FLuggageData`(단어 정보)를 설정합니다.
*   `ADropper::RequestSpawn()`을 호출하여 스폰 프로세스를 시작합니다.

**2. 스폰 실행 (ADropper → Aluggage)**
*   `ADropper::RequestSpawn()`은 `bIsSpawnIng` 플래그를 통해 중복 스폰 요청을 방지합니다.
*   `ADropper::Spawn()` (서버 실행)이 호출되어 실제 `Aluggage` 액터를 `SpawnPos`에 생성합니다.
*   스폰 과정 중 재생될 애니메이션(`Multicast_PlayAnimation()`)과 충돌 상태(`Multicast_DisableCollision()`, `Multicast_RestoreCollision()`)는 `NetMulticast` RPC를 통해 모든 클라이언트에게 전파되어 시각적 일관성을 유지합니다.

**3. 플레이어 상호작용 (Player ↔ Aluggage)**
*   플레이어는 스폰된 `Aluggage` 근처로 이동하여 `Grab` 입력(`APlayerControl::Server_OnGrab()`)을 통해 `UInteractionSystem::TryPickUp()`을 호출합니다.
*   `Aluggage`는 플레이어 캐릭터의 지정된 `HoldPosition`에 부착되어 플레이어와 함께 이동합니다.
*   플레이어는 `AHolder` 위로 이동한 후 `Drop` 입력(`APlayerControl::Server_OnGrabRelease()`)을 통해 `UInteractionSystem::TryDrop()`을 호출하여 `Aluggage`를 내려놓습니다.

**4. 정답 판정 (Aluggage → AHolder)**
*   내려놓은 `Aluggage`가 `AHolder`의 `BoxCollision`과 오버랩되면, `AHolder::OnBoxOverlapBegin()` 이벤트가 트리거됩니다.
*   `AHolder`는 오버랩된 액터가 `Aluggage`인지 확인하고, `AHolder::CheckLuggage()`를 호출하여 `Aluggage`가 가진 `FLuggageData`와 `AHolder`에 설정된 정답(`AnswerColorIdx`, `AnswerPatternIdx`)을 비교합니다.

**5. 결과 피드백 (AHolder)**
*   `AHolder::CheckLuggage()`의 판정 결과에 따라 `AHolder::UpdateActivateState()`가 호출되어 `bIsActivated` 속성이 업데이트됩니다.
*   `bIsActivated`가 `ReplicatedUsing=OnRep_IsActivated`로 설정되어 있으므로, 이 속성 변경은 모든 클라이언트에서 `OnRep_IsActivated()` 함수를 호출합니다.
*   `OnRep_IsActivated()` 내부에서는 `OnActivate(bool State)` `BlueprintImplementableEvent`가 호출되어, 블루프린트에서 홀더의 시각적 및 청각적 피드백(예: 정답 시 초록색 점등, 오답 시 빨간색 점멸)을 구현할 수 있도록 합니다.

### 4.3 해결 후 플로우
이 시스템의 구현으로, 플레이어는 `ADropper`를 통해 `luggage` 아이템을 획득하고, 이를 `AHolder`에 배치하여 퍼즐을 해결하는 일련의 게임플레이 루프를 경험할 수 있게 됩니다. 모든 과정은 네트워크를 통해 안정적으로 동기화됩니다.

---

## 5. 코드 구현 내역

### 5.1 구현 파일
- `Source/Onepiece/Prop/Public/ADropper.h`
- `Source/Onepiece/Prop/Private/ADropper.cpp`
- `Source/Onepiece/Prop/Public/AHolder.h`
- `Source/Onepiece/Prop/Private/AHolder.cpp`
- `Source/Onepiece/Character/Public/APlayerControl.h`
- `Source/Onepiece/Character/Private/APlayerControl.cpp`
- `Source/Onepiece/Prop/Public/luggage.h` (간접적으로 Aluggage의 존재 전제)
- `Source/Onepiece/NetworkData.h` (FLuggageData 정의)

### 5.2 주요 함수 및 클래스
- `ADropper` 클래스 (`RequestSpawn`, `Spawn`, `Multicast_PlayAnimation` 등)
- `AHolder` 클래스 (`OnBoxOverlapBegin`, `CheckLuggage`, `OnActivate` 등)
- `APlayerControl` 클래스 (`TEST_DropperDropProcess`, `RequestDrop`, `Server_OnGrab` 등)
- `FLuggageData` 구조체 (단어 정보를 담는 데이터 컨테이너)

### 5.3 핵심 변경 사항 (신규 구현이므로 '변경 사항' 대신 '주요 구현 요소'로 해석)
1.  **`ADropper` 구현**: `FLuggageData`와 `SpawnClass`를 받아 `Aluggage`를 스폰하는 기능 및 네트워크 동기화 로직 구현.
2.  **`AHolder` 구현**: `BoxCollision`을 통한 `Aluggage` 감지, `FLuggageData` 기반의 정답 판정, `BlueprintImplementableEvent`를 통한 결과 피드백 로직 구현.
3.  **`APlayerControl` 연동**: 플레이어 입력에 따라 `ADropper` 스폰을 요청하고, `UInteractionSystem`을 통해 `Aluggage`를 조작하는 RPC 기반의 서버 로직 구현.
4.  **`FLuggageData` 정의**: `NetworkData.h`에 `Aluggage`가 가질 단어 정보를 구조체로 정의.

---

## 6. 테스트 가이드

### 6.1 테스트 시나리오
1.  **`luggage` 스폰 테스트**:
    *   에디터에서 `APlayerControl`을 상속받은 플레이어 컨트롤러를 가진 플레이어 캐릭터로 게임 시작.
    *   맵에 `ADropper` 액터를 배치.
    *   `Jump` 키 (현재 `TEST_DropperDropProcess`에 바인딩된 입력)를 눌러 `ADropper`가 `Aluggage`를 성공적으로 스폰하는지 확인.
    *   스폰된 `Aluggage`가 물리적으로 월드에 존재하며 조작 가능한지 확인.
2.  **`luggage` 줍기/놓기 테스트**:
    *   스폰된 `Aluggage`에 접근하여 `Grab` 키를 눌러 집을 수 있는지 확인.
    *   `Aluggage`를 든 상태에서 캐릭터와 함께 이동하는지 확인.
    *   `Drop` 키를 눌러 `Aluggage`를 성공적으로 내려놓고 물리 시뮬레이션이 재개되는지 확인.
3.  **`AHolder` 정답 판정 테스트 (성공)**:
    *   맵에 `AHolder` 액터를 배치하고, 블루프린트에서 `SetAnswerData`를 통해 특정 `FLuggageData`를 정답으로 설정 (예: `AnswerColorIdx = 0, AnswerPatternIdx = 0`).
    *   `ADropper`를 통해 해당 정답에 맞는 `Aluggage`를 스폰하고 집습니다.
    *   `AHolder` 위로 `Aluggage`를 내려놓았을 때, `AHolder`가 `OnActivate(true)` 이벤트를 발생시키고 설정된 성공 피드백(예: 색상 변경)이 나타나는지 확인.
4.  **`AHolder` 정답 판정 테스트 (실패)**:
    *   위와 동일하게 `AHolder`를 설정하고, `ADropper`를 통해 정답과 다른 `Aluggage`를 스폰하여 집습니다.
    *   `AHolder` 위로 `Aluggage`를 내려놓았을 때, `AHolder`가 `OnActivate(false)` 이벤트를 발생시키고 설정된 실패 피드백이 나타나는지 확인.
5.  **멀티플레이어 동기화 테스트**:
    *   두 개 이상의 클라이언트로 게임을 실행.
    *   한 클라이언트에서 `luggage` 스폰, 줍기, 놓기, `AHolder`에 정답 제출 과정을 수행.
    *   다른 클라이언트에서 모든 과정(`Aluggage` 스폰, 이동, `AHolder` 상태 변화)이 시각적으로 올바르게 동기화되는지 확인.

### 6.2 디버깅 로그 체크리스트
- `LogOnepiece`: `ADropper` 및 `AHolder`의 내부 로직 (스폰 성공/실패, 판정 결과)에 대한 로그 확인.
- `LogInteraction`: `UInteractionSystem`의 아이템 줍기/놓기 관련 로그 확인.
- `LogNet`: RPC 호출 및 Replication 관련 네트워크 로그 확인.

### 6.3 Output Log 필터링
- `ADropper`, `AHolder`, `APlayerControl`, `luggage`, `InteractionSystem`, `LogNet`

---

## 7. 학습 포인트

### 7.1 모듈화된 시스템 설계
- 각 컴포넌트(`ADropper`, `AHolder`, `APlayerControl`)가 고유의 명확한 책임을 가지도록 설계함으로써, 코드의 가독성, 재사용성 및 유지보수성이 향상됩니다. 이는 복잡한 게임 시스템 개발에서 핵심적인 원칙입니다.

### 7.2 언리얼 엔진 네트워크 Replication 활용
- `UPROPERTY(Replicated)` 및 `ReplicatedUsing`을 통해 중요한 상태(`bIsSpawnIng`, `bIsActivated`, `CurTarget`)를 모든 클라이언트에게 자동으로 동기화하는 방법을 이해합니다.
- `Server` RPC는 클라이언트의 요청을 서버에서 안전하게 처리하고, `NetMulticast` RPC는 서버에서 발생한 이벤트를 모든 클라이언트에게 동시에 전파하는 데 사용됩니다.

### 7.3 `BlueprintImplementableEvent`를 통한 확장성
- `OnActivate`와 같은 `BlueprintImplementableEvent`는 C++ 코드를 직접 수정하지 않고도 게임 디자이너나 블루프린트 개발자가 게임플레이 로직이나 시각적 피드백을 자유롭게 커스터마이징할 수 있도록 합니다. 이는 C++와 블루프린트의 효율적인 협업 방식입니다.

---

## 8. 결론

### 8.1 요약
- **문제**: 단어 조합 퍼즐을 위한 `luggage` 스폰, 플레이어 조작, 정답 판정 시스템 필요.
- **원인**: (신규 기능 구현이므로 해당 없음)
- **해결**: `ADropper`, `AHolder`, `APlayerControl` 및 `UInteractionSystem`을 활용하여 모듈화되고 네트워크 동기화가 지원되는 시스템 구현.
- **효과**: 게임 내 퍼즐 요소를 추가하고, 확장 가능하며 재사용 가능한 상호작용 메커니즘을 제공.

### 8.2 핵심 교훈
1.  명확한 역할 분담과 모듈화는 복잡한 시스템 개발의 효율성을 높인다.
2.  언리얼 엔진의 강력한 네트워크 기능을 이해하고 활용하는 것이 멀티플레이어 게임 개발에 필수적이다.
3.  C++과 블루프린트 간의 유연한 연동(`BlueprintImplementableEvent`)은 개발 속도와 디자이너의 자유도를 향상시킨다.

---

**문서 버전**: 1.0
**최종 수정일**: 2025-12-06

**참조 파일**:
- `Source/Onepiece/Prop/Public/ADropper.h`
- `Source/Onepiece/Prop/Private/ADropper.cpp`
- `Source/Onepiece/Prop/Public/AHolder.h`
- `Source/Onepiece/Prop/Private/AHolder.cpp`
- `Source/Onepiece/Character/Public/APlayerControl.h`
- `Source/Onepiece/Character/Private/APlayerControl.cpp`
- `Source/Onepiece/Prop/Public/luggage.h`
- `Source/Onepiece/NetworkData.h`
