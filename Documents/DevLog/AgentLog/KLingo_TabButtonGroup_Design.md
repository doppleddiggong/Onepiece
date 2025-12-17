# UMG 탭 그룹 위젯 (Tab Group Widget) 설계 문서

## 프로젝트 개요

Onepiece 프로젝트의 UMG 시스템 내에서 재사용 가능한 탭 UI를 구현합니다. 이 시스템은 세 가지 핵심 클래스로 구성됩니다:

1. **UTabButton**: 개별 탭 버튼 위젯
2. **UTabIndicator**: 선택된 탭을 표시하는 인디케이터 위젯
3. **UTabButtonGroup**: 전체 탭 시스템을 관리하는 컨트롤러 위젯

사용자는 `UTabButtonGroup`에 탭 데이터(`TArray<FText>`)와 탭 버튼 클래스(`TSubclassOf<UTabButton>`)를 제공하면, 전체 탭 그룹이 동적으로 생성되고 관리됩니다.

---

## 프로젝트 코딩 컨벤션

### 필수 참고 문서
- `AgentRule/Project/Onepiece/AGENT_GUIDE.md` - 프로젝트 공통 가이드
- `AgentRule/Project/Onepiece/CODING_CONVENTIONS.md` - Onepiece 고유 코딩 규칙
- `AgentRule/Project/ue_coding_conventions.md` - UE 범용 코딩 컨벤션

### 주요 규칙
- **파일 헤더**: 저작권 표시 필수
  ```cpp
  // Copyright (c) 2025 Doppleddiggong. All rights reserved.
  // Unauthorized copying, modification, or distribution of this file,
  // via any medium is strictly prohibited. Proprietary and confidential.
  ```
- **변수명**: `UPROPERTY`는 PascalCase, 일반 변수는 camelCase, bool 타입은 bPascalCase
- **포인터**: `TObjectPtr<>` 사용
- **Doxygen 주석**: `/// @brief`, `/// @param [in/out]`, `/// @return` 스타일
- **API 매크로**: `ONEPIECE_API` 사용

---

## 1. UTabButton 위젯 (개별 탭 버튼)

`UTabButtonGroup`에 의해 동적으로 생성되고 관리되는 개별 탭 버튼입니다.

### 1.1. 주요 프로퍼티

#### BindWidget 컴포넌트
```cpp
/// @brief 클릭을 감지할 루트 버튼
UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
TObjectPtr<class UButton> Button_Tab;

/// @brief 탭의 텍스트 라벨
UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
TObjectPtr<class UTextBlock> Txt_ButtonLabel;

/// @brief 탭이 선택되었을 때 활성화될 이미지
UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
TObjectPtr<class UImage> Image_ActivateState;
```

#### 내부 상태
```cpp
/// @brief 이 탭 버튼을 소유한 부모 그룹
TWeakObjectPtr<class UTabButtonGroup> OwnerTabGroup;

/// @brief 이 탭 버튼의 그룹 내 인덱스
int32 TabIndex = -1;
```

### 1.2. 핵심 기능

#### InitData()
```cpp
void InitData(int32 InTabIndex, UTabButtonGroup* InOwnerGroup);
```
- 탭 인덱스와 소유 그룹 저장

#### SetSelected()
```cpp
void SetSelected(bool bIsSelected);
```
- `Image_ActivateState`의 **Visibility와 색상** 제어
- `Txt_ButtonLabel`의 색상 제어
- `OwnerTabGroup->GetTextColor()`, `GetActivateColor()` 사용

#### SetLabel()
```cpp
void SetLabel(const FText& InText);
```
- 텍스트 라벨 설정

### 1.3. 이벤트

```cpp
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTabButtonClicked, int32, TabIndex);

UPROPERTY(BlueprintAssignable, Category = "TabButton")
FOnTabButtonClicked OnTabButtonClicked;
```

---

## 2. UTabIndicator 위젯 (선택 인디케이터)

선택된 탭으로 부드럽게 이동하는 애니메이션을 제공하는 인디케이터 위젯입니다.

### 2.1. 주요 기능

#### MoveTo()
```cpp
void MoveTo(FVector2D TargetPosition, bool bAnimate = true);
```
- 목표 위치로 이동
- `bAnimate = true`: 부드러운 애니메이션
- `bAnimate = false`: 즉시 이동

#### SetAnimationSpeed()
```cpp
void SetAnimationSpeed(float Speed);
```
- 애니메이션 속도 설정 (초 단위)

### 2.2. 이벤트

```cpp
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMoveCompleted);

UPROPERTY(BlueprintAssignable, Category = "TabIndicator|Events")
FOnMoveCompleted OnMoveCompleted;
```
- 이동 애니메이션 완료 시 발생

### 2.3. 내부 구현

- **타이머 기반 애니메이션**: ~60 FPS (0.016초 간격)
- **Lerp 이동**: 부드러운 보간
- **CanvasPanelSlot** 사용: 자유로운 위치 제어

---

## 3. UTabButtonGroup 위젯 (탭 시스템 관리자)

전체 탭 시스템을 관리하는 핵심 위젯입니다.

### 3.1. 데이터 프로퍼티

```cpp
/// @brief 각 탭에 표시될 텍스트 라벨 배열
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TabButtonGroup|Data")
TArray<FText> TabLabels;

/// @brief 탭 버튼으로 생성할 UTabButton 위젯 블루프린트 클래스
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TabButtonGroup|Data")
TSubclassOf<class UTabButton> TabButtonClass;

/// @brief 기본으로 선택될 탭의 인덱스
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TabButtonGroup|Data")
int32 DefaultTabIndex = 0;
```

### 3.2. 외형 프로퍼티

```cpp
/// @brief 선택된 탭의 텍스트 색상
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TabButtonGroup|Appearance")
FLinearColor TextSelectColor = FLinearColor::White;

/// @brief 비선택된 탭의 텍스트 색상
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TabButtonGroup|Appearance")
FLinearColor TextUnselectColor = FLinearColor(0.5f, 0.5f, 0.5f);

/// @brief 선택된 탭의 활성화 이미지 색상
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TabButtonGroup|Appearance")
FLinearColor ActivateSelectColor = FLinearColor::White;

/// @brief 비선택된 탭의 활성화 이미지 색상
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TabButtonGroup|Appearance")
FLinearColor ActivateUnselectColor = FLinearColor(0.5f, 0.5f, 0.5f);
```

### 3.3. 레이아웃 프로퍼티

```cpp
/// @brief 각 탭의 고정 크기 (Width × Height)
/// TabIndicator 크기도 자동으로 이 값으로 설정됩니다.
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TabButtonGroup|Layout")
FVector2D TabSize = FVector2D(300.0f, 75.0f);
```

**중요**: `TabSize` 하나만 설정하면:
- TabIndicator 크기 자동 설정
- 위치 계산: `X = TabSize.X * TabIndex`

### 3.4. 애니메이션 프로퍼티

```cpp
/// @brief 선택 인디케이터의 이동 애니메이션 활성화 여부
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TabButtonGroup|Animation")
bool bAnimateIndicator = true;

/// @brief 선택 인디케이터의 애니메이션 속도 (초 단위)
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TabButtonGroup|Animation", meta=(EditCondition="bAnimateIndicator"))
float IndicatorAnimationSpeed = 0.15f;
```

### 3.5. BindWidget 컴포넌트

```cpp
/// @brief 생성된 탭 버튼들이 추가될 컨테이너
UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
TObjectPtr<class UHorizontalBox> TabContainer;

/// @brief 선택된 탭을 따라 움직이는 인디케이터 위젯
UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
TObjectPtr<class UTabIndicator> TabIndicator;
```

### 3.6. 핵심 기능

#### ApplyTab()
```cpp
void ApplyTab();
```
- `TabLabels` 데이터를 기반으로 탭 버튼들을 동적 생성
- 기존 탭 제거 → 새 탭 생성 → `TabContainer`에 추가
- `HorizontalBoxSlot` 설정: `HAlign_Fill`, `VAlign_Fill`

#### OnSelectTab()
```cpp
void OnSelectTab(int32 TabIndex, bool bBroadcastEvent = true);
```
- 특정 인덱스의 탭을 선택
- 모든 탭 버튼의 선택 상태 업데이트
- 인디케이터 위치 업데이트
- `bBroadcastEvent = true`: `OnTabSelected` 이벤트 발생

#### UpdateIndicatorPosition()
```cpp
void UpdateIndicatorPosition();
```
- 고정 폭 기반 위치 계산: `X = TabSize.X * CurTabIndex`
- `TabIndicator->MoveTo(targetPosition, bAnimateIndicator)` 호출

### 3.7. 이벤트

```cpp
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTabSelected, int32, TabIndex);

UPROPERTY(BlueprintAssignable, Category = "TabButtonGroup|Events")
FOnTabSelected OnTabSelected;
```

---

## 4. 블루프린트 구조

### 4.1. WBP_TabButton (탭 버튼 위젯)

**부모 클래스**: `UTabButton`

**계층 구조**:
```
CanvasPanel (루트)
├─ Button (이름: Button_Tab)
├─ Image (이름: Image_ActivateState)
└─ TextBlock (이름: Txt_ButtonLabel)
```

**권장 크기**: 300 × 75 (TabSize와 일치)

### 4.2. WBP_TabIndicator (인디케이터 위젯)

**부모 클래스**: `UTabIndicator`

**계층 구조**:
```
CanvasPanel (루트)
└─ Image (인디케이터 이미지)
```

**주의**: 크기는 자동 설정되므로 블루프린트에서 설정하지 않아도 됨

### 4.3. WBP_TabButtonGroup (탭 그룹 위젯)

**부모 클래스**: `UTabButtonGroup`

**권장 계층 구조** (Overlay 방식):
```
Overlay (이름: Overlay_Root)
├─ TabContainer (HorizontalBox)
│   └─ [동적 생성되는 TabButton들]
└─ Canvas Panel (이름: Canvas_Indicator)
    └─ TabIndicator
```

**Overlay 설정**:
- **Overlay_Root**: Fill (0, 0, 1, 1)
- **TabContainer**: Horizontal/Vertical Alignment = Fill
- **Canvas_Indicator**: Horizontal/Vertical Alignment = Fill (TabContainer와 같은 공간)

**TabIndicator 설정**:
- Anchors: (0, 0, 0, 0) - 왼쪽 위 고정
- Position: (0, 0)
- Size: 자동 설정됨 (`TabSize`로)

---

## 5. 사용 방법

### 5.1. 기본 설정

1. **WBP_TabButtonGroup를 메뉴에 추가**
2. **디테일 패널에서 설정**:
   ```
   Tab Labels: ["캐릭터", "인벤토리", "설정"]
   Tab Button Class: WBP_TabButton
   Default Tab Index: 0
   TabSize: (300, 75)
   ```

### 5.2. WidgetSwitcher 연동

**블루프린트 그래프**:
```
[TabButtonGroup] OnTabSelected
    ↓
[TabIndex]
    ↓
[WidgetSwitcher] Set Active Widget Index
```

### 5.3. 코드 예시 (C++)

```cpp
// 특정 탭 선택 (이벤트 발생)
TabButtonGroup->OnSelectTab(1, true);

// 특정 탭 선택 (이벤트 발생 안 함)
TabButtonGroup->OnSelectTab(2, false);
```

---

## 6. 핵심 특징

### 6.1. 단순화된 위치 계산

- **고정 폭 기반**: 복잡한 지오메트리 계산 불필요
- **공식**: `X = TabSize.X * TabIndex`
- **예시**:
  - 0번 탭: X = 300 × 0 = 0
  - 1번 탭: X = 300 × 1 = 300
  - 2번 탭: X = 300 × 2 = 600

### 6.2. 자동 크기 설정

- `TabSize` 하나만 변경하면 `TabIndicator` 크기도 자동 변경
- `NativeConstruct()`에서 `CanvasPanelSlot::SetSize(TabSize)` 호출

### 6.3. 책임 분리

- **UTabButton**: 개별 탭의 외형과 클릭 처리
- **UTabIndicator**: 이동 애니메이션 처리
- **UTabButtonGroup**: 전체 탭 시스템 관리

### 6.4. 이벤트 기반 확장

- `OnMoveCompleted`: 인디케이터 이동 완료 시 (사운드, 이펙트 등)
- `OnTabSelected`: 탭 선택 변경 시 (컨텐츠 전환 등)

---

## 7. 구현 체크리스트

### 코드
- [x] 저작권 헤더 추가
- [x] `TObjectPtr<>` 사용
- [x] Doxygen 주석 적용
- [x] `ONEPIECE_API` 매크로 사용
- [x] 변수명 컨벤션 준수

### 기능
- [x] 탭 동적 생성/제거
- [x] 탭 선택 상태 관리
- [x] 인디케이터 이동 애니메이션
- [x] 고정 폭 기반 위치 계산
- [x] TabSize 자동 적용
- [x] 색상 제어 (텍스트 + 활성화 이미지)

### 블루프린트
- [x] `BindWidget` 이름 규칙 준수
- [x] Overlay 구조 적용
- [x] 에디터 프리뷰 동작 확인

### 이벤트
- [x] `OnTabSelected` 델리게이트
- [x] `OnMoveCompleted` 델리게이트
- [x] `OnTabButtonClicked` 델리게이트

---

## 8. 고급 활용

### 8.1. 커스텀 인디케이터 애니메이션

`OnMoveCompleted` 이벤트를 활용:
```cpp
TabIndicator->OnMoveCompleted.AddDynamic(this, &UMyWidget::OnIndicatorMoved);

void UMyWidget::OnIndicatorMoved()
{
    // 사운드 재생
    // 파티클 이펙트
    // 햅틱 피드백
}
```

### 8.2. 동적 탭 추가/제거

```cpp
// 탭 추가
TabLabels.Add(FText::FromString("새 탭"));
ApplyTab();

// 탭 제거
TabLabels.RemoveAt(1);
ApplyTab();
```

### 8.3. 커스텀 탭 크기

```cpp
// 탭 크기 변경
TabSize = FVector2D(400.0f, 100.0f);

// NativeConstruct()에서 자동으로 TabIndicator 크기 업데이트됨
```

---

## 9. 문제 해결

### 인디케이터 위치가 안 맞을 때

1. **Overlay 구조 확인**:
   - TabContainer와 Canvas_Indicator가 같은 Overlay의 자식인지 확인
   - 둘 다 Fill Alignment인지 확인

2. **TabIndicator Anchors 확인**:
   - Anchors: (0, 0, 0, 0) - 왼쪽 위 고정

3. **TabSize 확인**:
   - TabButton 크기와 TabSize가 일치하는지 확인

### 탭이 생성되지 않을 때

1. **TabButtonClass 설정 확인**
2. **TabLabels 배열이 비어있지 않은지 확인**
3. **BindWidget 이름 확인**: `TabContainer` 정확히 일치

---

## 10. 참고 파일

### 소스 파일
- `Source/Onepiece/WidgetComponents/Public/UTabButton.h`
- `Source/Onepiece/WidgetComponents/Private/UTabButton.cpp`
- `Source/Onepiece/WidgetComponents/Public/UTabIndicator.h`
- `Source/Onepiece/WidgetComponents/Private/UTabIndicator.cpp`
- `Source/Onepiece/WidgetComponents/Public/UTabButtonGroup.h`
- `Source/Onepiece/WidgetComponents/Private/UTabButtonGroup.cpp`

### 블루프린트 예시
- `Content/CustomContents/UI/Components/WBP_TabButton.uasset`
- `Content/CustomContents/UI/Components/WBP_TabIndicator.uasset` (권장)
- `Content/CustomContents/UI/Components/WBP_TabButtonGroup.uasset`
