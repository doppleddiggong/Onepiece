### **UMG 탭 그룹 위젯(Tab Group Widget) 기능 구현 요청**

**프로젝트 개요:**
Onepiece 프로젝트의 UMG 시스템 내에서 재사용 가능한 탭 UI를 구현합니다. 이 시스템의 핵심은 `UTabButtonGroup` 위젯으로, 외부에서 탭에 표시될 데이터(`TArray<FText>`)와 탭 버튼의 외형을 정의하는 위젯 클래스(`TSubclassOf<UTabButton>`)를 제공받아 전체 탭 그룹을 동적으로 생성하고 관리합니다. 사용자는 개별 탭 버튼을 직접 다루지 않으며, `UTabButtonGroup`의 데이터 소스를 변경하는 것만으로 UI를 제어할 수 있어야 합니다.

**프로젝트 코딩 컨벤션:**
-   **필수 참고 문서**:
    -   `AgentRule/Project/Onepiece/AGENT_GUIDE.md` - 프로젝트 공통 가이드
    -   `AgentRule/Project/Onepiece/CODING_CONVENTIONS.md` - Onepiece 고유 코딩 규칙
    -   `AgentRule/Project/ue_coding_conventions.md` - UE 범용 코딩 컨벤션
-   **파일 헤더**: 다음 저작권 표시 필수
    ```cpp
    // Copyright (c) 2025 Doppleddiggong. All rights reserved.
    // Unauthorized copying, modification, or distribution of this file,
    // via any medium is strictly prohibited. Proprietary and confidential.
    ```
-   **변수명**: `UPROPERTY`는 PascalCase, 일반 변수는 camelCase, bool 타입은 bPascalCase 사용
-   **포인터**: `TObjectPtr<>`를 사용하여 UE의 GC 시스템을 명시적으로 활용
-   **전방 선언**: `TObjectPtr<class UClassName>` 형식 권장
-   **Doxygen 주석**: 모든 헤더 파일의 프로퍼티와 함수에 `/// @brief`, `/// @param [in/out]`, `/// @return`, `/// @note` 스타일 적용
-   **클래스명**: `UTabButton`, `UTabButtonGroup` 사용
-   **API 매크로**: `ONEPIECE_API` 사용

**요구사항:**
아래 설명된 `UTabButton`과 `UTabButtonGroup` C++ 클래스를 구현해 주세요. `UTabButtonGroup`이 모든 로직을 주도하며, 사용자는 이 위젯의 프로퍼티를 설정하고 이벤트에 바인딩하는 것만으로 기능을 사용할 수 있도록 설계해야 합니다.

---

### **1. `UTabButton` 위젯 구현 (내부 관리용)**

`UTabButtonGroup`에 의해 동적으로 생성되고 관리되는 개별 탭 버튼입니다. 사용자가 직접 생성하거나 제어하지 않습니다.

#### **1.1. 클래스 및 주요 프로퍼티 (`TabButton.h`)**

```cpp
// Copyright (c) 2025 Doppleddiggong. All rights reserved.
// Unauthorized copying, modification, or distribution of this file,
// via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TabButton.generated.h"

/**
 * @brief UTabButtonGroup에 의해 관리되는 개별 탭 버튼 위젯.
 * @note 사용자가 직접 생성하지 않습니다.
 */
UCLASS()
class ONEPIECE_API UTabButton : public UUserWidget
{
	GENERATED_BODY()

public:
	// 델리게이트: 이 버튼이 클릭되었을 때 자신의 인덱스를 포함하여 브로드캐스트합니다.
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTabButtonClicked, int32, TabIndex);

	/// @brief 탭 버튼이 클릭되었을 때 발생하는 이벤트
	UPROPERTY(BlueprintAssignable, Category = "TabButton")
	FOnTabButtonClicked OnTabButtonClicked;

protected:
	/// @brief 클릭을 감지할 루트 버튼. 블루프린트에서 'Button_Tab' 이름으로 생성해야 합니다.
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<class UButton> Button_Tab;

	/// @brief 탭의 텍스트 라벨. 블루프린트에서 'Text_ButtonLabel' 이름으로 생성해야 합니다.
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<class UTextBlock> Text_ButtonLabel;

	/// @brief 탭이 선택되었을 때 활성화될 위젯 (예: 하이라이트 이미지). 블루프린트에서 'Widget_ActivateState' 이름으로 생성해야 합니다.
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<class UWidget> Widget_ActivateState;

private:
	/// @brief 이 탭 버튼의 그룹 내 인덱스
	int32 tabIndex = -1;

	/// @brief 이 탭 버튼을 소유한 부모 그룹
	TWeakObjectPtr<class UTabButtonGroup> ownerButtonGroup;

protected:
	virtual void NativeConstruct() override;

	/// @brief 버튼 클릭 시 호출될 내부 함수
	UFUNCTION()
	void HandleButtonClicked();

public:
	/// @brief 버튼을 초기화하는 함수. 부모 그룹에 의해 호출됩니다.
	/// @param [in] InTabIndex 이 버튼에 할당될 인덱스
	/// @param [in] InOwnerGroup 이 버튼을 소유한 부모 그룹
	void InitializeButton(int32 InTabIndex, UTabButtonGroup* InOwnerGroup);

	/// @brief 이 버튼의 선택 상태를 갱신합니다.
	/// @param [in] bIsSelected 새로운 선택 상태
	void SetSelected(bool bIsSelected);

	/// @brief 버튼의 텍스트 라벨을 설정합니다.
	/// @param [in] InText 설정할 텍스트
	void SetLabel(const FText& InText);
};
```

#### **1.2. 핵심 기능 (`TabButton.cpp`)**

1.  **`InitializeButton()`**:
    *   `tabIndex`와 `ownerButtonGroup`을 저장합니다.

2.  **`NativeConstruct()`**:
    *   `Button_Tab`의 `OnClicked` 델리게이트에 `HandleButtonClicked` 함수를 바인딩합니다.

3.  **`HandleButtonClicked()`**:
    *   `OnTabButtonClicked` 델리게이트를 브로드캐스트하여 자신의 `tabIndex`를 알립니다.

4.  **`SetSelected(bool bIsSelected)`**:
    *   `Widget_ActivateState`의 Visibility를 `bIsSelected` 값에 따라 `ESlateVisibility::HitTestInvisible` 또는 `ESlateVisibility::Collapsed`로 설정합니다.
    *   `ownerButtonGroup`에서 `GetSelectedColor()`와 `GetUnselectedColor()` 값을 가져와 `Text_ButtonLabel`의 색상을 변경합니다.

---

### **2. `UTabButtonGroup` 위젯 구현 (사용자 컨트롤용)**

데이터를 기반으로 `UTabButton` 인스턴스들을 동적으로 생성하고, 전체 탭 시스템의 상태를 관리하는 핵심 위젯입니다.

#### **2.1. 클래스 및 주요 프로퍼티 (`TabButtonGroup.h`)**

```cpp
// Copyright (c) 2025 Doppleddiggong. All rights reserved.
// Unauthorized copying, modification, or distribution of this file,
// via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TabButtonGroup.generated.h"

/**
 * @brief 데이터를 기반으로 탭 버튼들을 동적으로 생성하고 관리하는 탭 그룹 위젯.
 */
UCLASS()
class ONEPIECE_API UTabButtonGroup : public UUserWidget
{
	GENERATED_BODY()

public:
	// 델리게이트: 탭 선택이 변경될 때마다 선택된 인덱스를 브로드캐스트합니다.
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTabSelected, int32, TabIndex);

	/// @brief 탭 선택이 변경될 때 발생하는 이벤트
	UPROPERTY(BlueprintAssignable, Category = "TabButtonGroup|Events")
	FOnTabSelected OnTabSelected;

	//================================================================//
	// DATA PROPERTIES (사용자가 설정)
	//================================================================//
protected:
	/// @brief 각 탭에 표시될 텍스트 라벨 배열. 이 배열의 크기에 따라 탭이 생성됩니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TabButtonGroup|Data")
	TArray<FText> TabLabels;

	/// @brief 탭 버튼으로 생성할 UTabButton 위젯 블루프린트 클래스.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TabButtonGroup|Data")
	TSubclassOf<class UTabButton> TabButtonClass;

	/// @brief 위젯이 생성될 때 기본으로 선택될 탭의 인덱스.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TabButtonGroup|Data")
	int32 DefaultTabIndex = 0;

	/// @brief 선택된 탭의 텍스트 색상.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TabButtonGroup|Appearance")
	FLinearColor Color_Selected = FLinearColor::White;

	/// @brief 비선택된 탭의 텍스트 색상.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TabButtonGroup|Appearance")
	FLinearColor Color_UnSelected = FLinearColor(0.5f, 0.5f, 0.5f);

	/// @brief 선택 인디케이터의 이동 애니메이션 활성화 여부.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TabButtonGroup|Animation")
	bool bAnimateIndicator = true;

	/// @brief 선택 인디케이터의 애니메이션 속도 (초 단위).
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TabButtonGroup|Animation", meta=(EditCondition="bAnimateIndicator"))
	float IndicatorAnimationSpeed = 0.15f;

	//================================================================//
	// COMPONENT PROPERTIES (블루프린트에서 바인딩)
	//================================================================//
protected:
	/// @brief 생성된 탭 버튼들이 추가될 컨테이너. 블루프린트에서 'TabContainer' 이름으로 생성해야 합니다.
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<class UHorizontalBox> TabContainer;

	/// @brief 선택된 탭을 따라 움직이는 이미지 위젯. 블루프린트에서 'Image_SelectedIndicator' 이름으로 생성해야 합니다.
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<class UImage> Image_SelectedIndicator;

	//================================================================//
	// INTERNAL STATE
	//================================================================//
private:
	/// @brief 동적으로 생성되어 현재 관리 중인 탭 버튼 위젯 인스턴스 배열.
	UPROPERTY(Transient)
	TArray<TObjectPtr<class UTabButton>> TabButtonInstances;

	/// @brief 현재 선택된 탭의 인덱스.
	int32 currentTabIndex = -1;

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

	/// @brief TabLabels 데이터를 기반으로 탭 버튼들을 다시 생성하고 배치합니다.
	UFUNCTION(BlueprintCallable, Category="TabButtonGroup")
	void RebuildTabs();

	/// @brief 탭 버튼 중 하나가 클릭되었을 때 호출될 내부 핸들러.
	UFUNCTION()
	void OnTabClicked_Internal(int32 TabIndex);

private:
	/// @brief 선택 인디케이터의 위치를 현재 선택된 탭으로 업데이트합니다.
	void UpdateIndicatorPosition();

public:
	/// @brief 특정 인덱스의 탭을 강제로 선택합니다.
	/// @param [in] Index 선택할 탭의 인덱스
	/// @param [in] bBroadcastEvent OnTabSelected 델리게이트를 호출할지 여부
	UFUNCTION(BlueprintCallable, Category="TabButtonGroup")
	void SetTab(int32 Index, bool bBroadcastEvent = true);

	/// @brief 선택된 탭의 색상을 반환합니다.
	/// @return 선택된 탭의 색상
	FLinearColor GetSelectedColor() const { return Color_Selected; }

	/// @brief 비선택된 탭의 색상을 반환합니다.
	/// @return 비선택된 탭의 색상
	FLinearColor GetUnselectedColor() const { return Color_UnSelected; }
};
```

#### **2.2. 핵심 기능 (`TabButtonGroup.cpp`)**

1.  **`NativePreConstruct()`**: 에디터 상에서 UI 변경을 즉시 확인할 수 있도록 `RebuildTabs()`를 호출합니다.

2.  **`NativeConstruct()`**:
    *   `RebuildTabs()`를 호출하여 위젯을 구성합니다.
    *   `SetTab(DefaultTabIndex, false)`를 호출하여 초기 탭을 설정합니다. (이때는 `OnTabSelected` 이벤트를 발생시키지 않습니다.)

3.  **`RebuildTabs()`**:
    *   `TabButtonClass`가 유효한지 확인합니다.
    *   `TabContainer`의 모든 자식 위젯과 `TabButtonInstances` 배열을 비웁니다.
    *   `TabLabels` 배열을 순회하며 각 항목에 대해:
        *   `CreateWidget<UTabButton>()`으로 `TabButtonClass`의 인스턴스를 생성합니다.
        *   `button->InitializeButton(Index, this)`로 버튼을 초기화합니다.
        *   `button->SetLabel()`로 텍스트를 설정합니다.
        *   `button->OnTabButtonClicked` 델리게이트에 `OnTabClicked_Internal` 함수를 바인딩합니다.
        *   `TabContainer->AddChildToHorizontalBox()`를 사용하여 컨테이너에 자식으로 추가하고 Slot 설정을 합니다.
        *   생성된 인스턴스를 `TabButtonInstances` 배열에 추가합니다.

4.  **`SetTab(int32 Index, ...)`**:
    *   인덱스가 유효하고 현재 선택된 탭과 다른지 확인합니다.
    *   `currentTabIndex`를 새로운 `Index`로 업데이트합니다.
    *   모든 `TabButtonInstances`를 순회하며 `SetSelected()`를 호출하여 현재 선택된 탭만 `true`로 설정합니다.
    *   `UpdateIndicatorPosition()`을 호출하여 인디케이터 위치를 갱신합니다.
    *   `bBroadcastEvent`가 `true`이면 `OnTabSelected` 델리게이트를 브로드캐스트합니다.

5.  **`UpdateIndicatorPosition()`**:
    *   `TabButtonInstances[currentTabIndex]`의 `GetCachedGeometry()`를 사용하여 현재 탭의 위치와 크기를 가져옵니다.
    *   인디케이터가 부모 컨테이너 내에서 올바른 위치로 이동하도록 `UCanvasPanelSlot`으로 캐스팅하여 포지션을 설정합니다.
    *   `bAnimateIndicator`가 `true`일 경우, `FTimerHandle`과 `FMath::Lerp`를 사용하여 현재 위치에서 목표 위치까지 부드럽게 이동하는 애니메이션을 구현합니다. (Tick 사용은 지양)

---

### **3. 구현 가이드 및 사용법**

#### **A. `WBP_TabButton` 블루프린트 위젯 생성**
1.  C++ `UTabButton` 클래스를 부모로 하는 위젯 블루프린트 `WBP_TabButton`을 생성합니다.
2.  **계층 구조 (Hierarchy)**:
    *   `CanvasPanel` (루트)
        *   `Button` (이름: `Button_Tab`, `Visibility`: `Visible`)
        *   `Image` (이름: `Widget_ActivateState`, `Visibility`: `Collapsed`)
        *   `TextBlock` (이름: `Text_ButtonLabel`)
3.  디자인: 버튼, 활성 상태 이미지, 텍스트의 배치, 크기, 폰트 등을 자유롭게 디자인합니다. C++ 코드가 이름(`meta=BindWidget`)을 통해 이 컴포넌트들을 자동으로 찾아 제어합니다.

#### **B. `WBP_TabButtonGroup` 블루프린트 위젯 생성**
1.  C++ `UTabButtonGroup` 클래스를 부모로 하는 `WBP_TabButtonGroup`을 생성합니다.
2.  **계층 구조 (Hierarchy)**:
    *   `CanvasPanel` (루트, 인디케이터의 자유로운 이동을 위해 필요)
        *   `HorizontalBox` (이름: `TabContainer`)
        *   `Image` (이름: `Image_SelectedIndicator`)
3.  인디케이터 이미지는 `CanvasPanel`의 자식으로 두어 `SetPosition`으로 위치를 제어할 수 있게 합니다.

#### **C. 실제 사용 예시 (in `WBP_SomeMenu`)**
1.  메뉴 위젯(`WBP_SomeMenu`)의 디자이너에 `WBP_TabButtonGroup`을 추가합니다.
2.  `WBP_TabButtonGroup`을 선택하고 **디테일(Details) 패널**에서 다음을 설정합니다.
    *   **Tab Labels**: `+` 버튼을 눌러 "캐릭터", "인벤토리", "설정" 등 원하는 탭의 텍스트를 추가합니다.
    *   **Tab Button Class**: `WBP_TabButton`으로 설정합니다.
    *   **Default Tab Index**: `0`으로 설정합니다.
    *   **Colors** 및 **Animation** 설정을 원하는 대로 조정합니다.
3.  **그래프(Graph)** 탭으로 이동하여 `WidgetSwitcher`와 연동합니다.
    *   `WBP_TabButtonGroup` 변수를 선택하고 `OnTabSelected` 이벤트에 노드를 추가합니다.
    *   `Switch on Int` 노드 또는 `Set Active Widget Index`를 사용하여 이벤트에서 받은 `TabIndex`에 따라 `WidgetSwitcher`가 보여줄 화면을 변경하는 로직을 구현합니다.

---

### **최종 체크리스트**

-   [ ] 모든 파일에 Onepiece 표준 저작권 헤더 추가
-   [ ] `TObjectPtr<>` 사용 및 전방 선언 스타일 준수
-   [ ] Doxygen 스타일 주석 적용 (`@param [in/out]`, `@return`, `@note` 포함)
-   [ ] `ONEPIECE_API` 매크로 사용
-   [ ] 파일명 규칙 준수 (`TabButton.h/cpp`, `TabButtonGroup.h/cpp`)
-   [ ] `UTabButton` 블루프린트에서 `BindWidget` 이름 규칙 준수
-   [ ] `UTabButtonGroup`에서 데이터(`TabLabels`) 변경 시 `RebuildTabs()` 호출로 UI가 올바르게 재생성되는지 확인
-   [ ] 에디터 프리뷰(`NativePreConstruct`)에서 탭 생성이 정상 동작하는지 확인
-   [ ] 인디케이터 애니메이션이 부드럽게 동작하는지 확인
-   [ ] `OnTabSelected` 이벤트가 정상적으로 `TabIndex`를 전달하는지 테스트
-   [ ] 변수명 컨벤션 준수 (일반 변수는 camelCase, UPROPERTY는 PascalCase, bool은 bPascalCase)
