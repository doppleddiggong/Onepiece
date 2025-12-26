# UMG 페이지 스크롤 뷰 위젯 (Page Scroll View Widget) 설계 문서

## 프로젝트 개요

Onepiece 프로젝트의 UMG 시스템 내에서 재사용 가능한 **페이지 단위 스크롤 뷰**를 구현합니다. 이 시스템은 세 가지 핵심 클래스로 구성됩니다:

1. **UPageScrollView**: 전체 페이지 스크롤 시스템을 관리하는 컨트롤러 위젯
2. **UPageScrollItem**: 개별 페이지 콘텐츠를 담는 컨테이너 위젯
3. **UPageScrollDot**: 현재 페이지를 표시하는 인디케이터 위젯

사용자는 `UPageScrollView`에 페이지 크기와 페이지 수를 제공하면, 드래그 기반 스크롤과 자동 스냅 기능이 작동합니다.

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

## 1. EPageScrollDirection (스크롤 방향 열거형)

**파일**: `Source/Onepiece/WidgetComponents/Public/EPageScrollDirection.h`

```cpp
UENUM(BlueprintType)
enum class EPageScrollDirection : uint8
{
    Horizontal UMETA(DisplayName = "Horizontal"),
    Vertical UMETA(DisplayName = "Vertical")
};
```

---

## 2. UPageScrollView 위젯 (메인 컨트롤러)

`UPageScrollView`는 전체 페이지 스크롤 시스템을 관리하며, 드래그 감지, 스냅 애니메이션, 페이지 전환을 담당합니다.

### 2.1. 주요 프로퍼티

#### 레이아웃 설정
```cpp
/// @brief 스크롤 방향 (수평/수직)
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PageScrollView|Layout")
EPageScrollDirection ScrollDirection = EPageScrollDirection::Horizontal;

/// @brief 각 페이지의 크기 (Width × Height)
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PageScrollView|Layout")
FVector2D PageSize = FVector2D(1920.0f, 1080.0f);

/// @brief 페이지 간 간격 (픽셀)
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PageScrollView|Layout")
float PageSpacing = 0.0f;
```

#### 동작 설정
```cpp
/// @brief 빠른 스와이프 인식 임계값 (픽셀/초)
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PageScrollView|Behavior")
float FastSwipeThreshold = 500.0f;

/// @brief 스냅 애니메이션 속도 (초 단위)
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PageScrollView|Behavior")
float SnapAnimationSpeed = 0.3f;

/// @brief 중앙 스케일 효과 활성화 여부
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PageScrollView|Behavior")
bool bEnableScaleEffect = false;

/// @brief 중앙에 있는 페이지의 스케일 값
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PageScrollView|Behavior")
float FocusScale = 1.0f;

/// @brief 중앙에서 벗어난 페이지의 스케일 값
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PageScrollView|Behavior")
float SideScale = 0.8f;
```

#### 데이터 설정
```cpp
/// @brief 페이지 아이템으로 생성할 위젯 블루프린트 클래스
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PageScrollView|Data")
TSubclassOf<class UPageScrollItem> PageItemClass;

/// @brief 기본으로 선택될 페이지 인덱스
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PageScrollView|Data")
int32 DefaultPageIndex = 0;
```

#### BindWidget 컴포넌트
```cpp
/// @brief 페이지 아이템들을 담을 컨테이너 (드래그로 위치 이동)
UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
TObjectPtr<class UCanvasPanel> PageContainer;

/// @brief 페이지 인디케이터 (선택적)
UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
TObjectPtr<class UPageScrollDot> PageDotIndicator;
```

### 2.2. 핵심 기능

#### SetNumberOfPages()
```cpp
/// @brief 페이지 개수를 설정하고 동적으로 생성합니다.
/// @param [in] ItemCount 생성할 페이지 수
UFUNCTION(BlueprintCallable, Category = "PageScrollView")
void SetNumberOfPages(int32 ItemCount);
```
- 기존 페이지 제거 → 새 페이지 생성 → PageContainer에 배치
- 각 페이지는 `PageItemClass`로 생성
- 생성 후 자동으로 `DefaultPageIndex`로 이동

#### MovePage()
```cpp
/// @brief 특정 페이지로 이동합니다.
/// @param [in] PageIndex 이동할 페이지 인덱스
/// @param [in] bAnimate 애니메이션 여부
UFUNCTION(BlueprintCallable, Category = "PageScrollView")
void MovePage(int32 PageIndex, bool bAnimate = true);
```
- 목표 페이지의 위치를 계산하여 애니메이션 실행
- `bAnimate = false`면 즉시 이동
- 이동 완료 시 `OnPageChanged` 델리게이트 발생

#### CalculateTargetPage()
```cpp
int32 CalculateTargetPage(FVector2D DragDelta, float Velocity) const;
```
- **빠른 스와이프**: 속도 >= FastSwipeThreshold → 방향에 따라 페이지 이동
- **느린 드래그**: 가장 가까운 페이지로 스냅
- 범위 제한: 0 ~ (페이지 수 - 1)

#### TickSnapAnimation()
```cpp
void TickSnapAnimation();
```
- Timer 기반 (~60 FPS)
- `FMath::InterpEaseOut()` 사용한 부드러운 Ease-out curve
- 애니메이션 완료 시 `OnPageChanged` 델리게이트 발생

#### UpdateItemScales()
```cpp
void UpdateItemScales();
```
- `bEnableScaleEffect = true`일 때만 동작
- 뷰포트 중심으로부터 거리 계산
- Lerp로 FocusScale ↔ SideScale 보간
- 각 페이지의 `UpdateFocusState()` 호출

### 2.3. 마우스 이벤트 (드래그 감지)

```cpp
virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
```

**구현 패턴:**
- **OnMouseButtonDown**: `bIsDragging = true`, 시작 위치 저장, Mouse Capture
- **OnMouseMove**: 드래그 중 실시간 컨테이너 위치 업데이트
- **OnMouseButtonUp**: 속도 계산 → CalculateTargetPage() → MovePage(), Mouse Capture 해제

**작은 드래그 구분:**
- 드래그 거리 <= 10 픽셀: 클릭으로 처리 (버튼 이벤트 전달)

### 2.4. 이벤트

```cpp
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPageChanged, int32, PrevPage, int32, CurrentPage);

UPROPERTY(BlueprintAssignable, Category = "PageScrollView|Events")
FOnPageChanged OnPageChanged;
```

---

## 3. UPageScrollItem 위젯 (페이지 컨테이너)

`UPageScrollView`에 의해 동적으로 생성되며, 개별 페이지의 콘텐츠를 담는 컨테이너입니다.

### 3.1. 주요 프로퍼티

```cpp
private:
    /// @brief 이 페이지를 소유한 부모 ScrollView
    TWeakObjectPtr<class UPageScrollView> ownerScrollView;

    /// @brief 이 페이지의 인덱스
    int32 pageIndex = -1;

    /// @brief 현재 포커스 상태
    bool bIsFocused = false;
```

### 3.2. 핵심 기능

#### InitData()
```cpp
void InitData(int32 InIndex, UPageScrollView* InOwner);
```
- 페이지 인덱스와 소유 그룹 저장
- 초기화 후 `SetPageContent(InIndex)` 호출

#### UpdateFocusState()
```cpp
UFUNCTION(BlueprintNativeEvent, Category = "PageScrollItem")
void UpdateFocusState(bool bInFocused, float InScale);
```
- 스케일 효과가 활성화되었을 때 호출
- Blueprint에서 재정의 가능 (애니메이션, 색상 변경 등)
- C++에서는 `OnItemFocusChanged` 델리게이트만 발생

#### SetPageContent()
```cpp
UFUNCTION(BlueprintImplementableEvent, Category = "PageScrollItem")
void SetPageContent(int32 Index);
```
- Blueprint에서 구현 필수
- 예: 이미지 변경, 텍스트 업데이트 등

### 3.3. 이벤트

```cpp
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemFocusChanged, bool, bFocused);

UPROPERTY(BlueprintAssignable, Category = "PageScrollItem|Events")
FOnItemFocusChanged OnItemFocusChanged;
```

---

## 4. UPageScrollDot 위젯 (페이지 인디케이터)

현재 페이지를 시각적으로 표시하는 인디케이터 위젯입니다.

### 4.1. 주요 프로퍼티

```cpp
/// @brief 개별 도트 위젯 클래스
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PageScrollDot|Data")
TSubclassOf<class UUserWidget> DotWidgetClass;

/// @brief 선택된 도트의 색상
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PageScrollDot|Appearance")
FLinearColor SelectedColor = FLinearColor::White;

/// @brief 비선택된 도트의 색상
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PageScrollDot|Appearance")
FLinearColor UnselectedColor = FLinearColor(0.5f, 0.5f, 0.5f, 1.0f);

/// @brief 선택된 도트의 스케일
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PageScrollDot|Appearance")
float SelectedScale = 1.2f;

/// @brief 비선택된 도트의 스케일
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PageScrollDot|Appearance")
float UnselectedScale = 1.0f;

/// @brief 생성된 도트들을 담을 컨테이너
UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
TObjectPtr<class UHorizontalBox> DotContainer;
```

### 4.2. 핵심 기능

#### SetNumberOfPages()
```cpp
void SetNumberOfPages(int32 PageCount);
```
- 기존 도트 제거 → 새 도트 생성 → DotContainer에 추가
- Unity의 UIPageControl::SetNumberOfPages() 동일 패턴
- 첫 번째 도트를 자동으로 선택 상태로 설정

#### SetCurrentPage()
```cpp
void SetCurrentPage(int32 PageIndex);
```
- 선택된 도트: SelectedColor, SelectedScale 적용
- 비선택 도트: UnselectedColor, UnselectedScale 적용
- Image 컴포넌트의 `SetColorAndOpacity()` 및 `SetRenderScale()` 사용

#### GetDotImage()
```cpp
UImage* GetDotImage(UUserWidget* DotWidget) const;
```
- 도트 위젯에서 "Image_Dot" 이름의 Image 컴포넌트 찾기
- 못 찾으면 WidgetTree에서 첫 번째 Image 컴포넌트 사용

---

## 5. 블루프린트 구조

### 5.1. WBP_PageScrollView (메인 위젯)

**부모 클래스**: `UPageScrollView`

**계층 구조**:
```
SizeBox (루트, Viewport 크기 정의)
└─ CanvasPanel (이름: Canvas_Root, Clipping = ClipToBounds)
    ├─ CanvasPanel (이름: PageContainer, Anchors = (0,0,0,0))
    │   └─ [동적 생성되는 PageScrollItem 위젯들]
    └─ PageScrollDot (이름: PageDotIndicator, Anchors = (0.5,1,0.5,1))
```

**중요 설정**:
- **SizeBox**: Fill (0, 0, 1, 1), Width/Height Override로 고정 크기 설정
- **Canvas_Root**: Fill, **Clipping = ClipToBounds** (필수!)
- **PageContainer**: Anchors (0, 0, 0, 0) - 왼쪽 위 고정
- **PageDotIndicator**: Anchors (0.5, 1, 0.5, 1) - 하단 중앙

### 5.2. WBP_PageScrollItem (페이지 아이템)

**부모 클래스**: `UPageScrollItem`

**계층 구조**:
```
CanvasPanel (루트)
└─ [사용자 정의 콘텐츠]
    예: Image, TextBlock 등
```

**Blueprint 구현 필수**:
- `SetPageContent(int32 Index)` 이벤트 구현
- 페이지 인덱스에 따라 콘텐츠 변경 로직 작성

### 5.3. WBP_PageScrollDot (인디케이터)

**부모 클래스**: `UPageScrollDot`

**계층 구조**:
```
HorizontalBox (이름: DotContainer)
└─ [동적 생성되는 도트 위젯들]
```

### 5.4. WBP_DotItem (개별 도트)

**부모 클래스**: `UUserWidget`

**계층 구조**:
```
CanvasPanel (루트)
└─ Image (이름: Image_Dot)
```

**권장 크기**: 10 × 10

---

## 6. 사용 방법

### 6.1. 기본 설정

1. **WBP_PageScrollView를 UI에 추가**
2. **디테일 패널에서 설정**:
   ```
   Scroll Direction: Horizontal
   Page Size: (1920, 1080)
   Page Spacing: 50
   Page Item Class: WBP_MyCustomPage
   Default Page Index: 0
   Snap Animation Speed: 0.3
   ```

3. **NativeConstruct()에서 초기화**:
   ```cpp
   PageScrollView->SetNumberOfPages(5); // 5개 페이지 생성
   ```

### 6.2. 이벤트 연동

**블루프린트 그래프**:
```
[PageScrollView] OnPageChanged
    ↓ (PrevPage, CurrentPage)
    ↓
[커스텀 로직] 페이지별 콘텐츠 업데이트
```

### 6.3. 코드 예시 (C++)

```cpp
// 페이지 이동
PageScrollView->MovePage(2, true); // 2번 페이지로 애니메이션 이동
PageScrollView->NextPage(); // 다음 페이지
PageScrollView->PreviousPage(); // 이전 페이지

// 이벤트 바인딩
PageScrollView->OnPageChanged.AddDynamic(this, &UMyWidget::OnPageChanged);

void UMyWidget::OnPageChanged(int32 PrevPage, int32 CurrentPage)
{
    // 페이지 변경 처리
    UE_LOG(LogTemp, Log, TEXT("Page changed: %d → %d"), PrevPage, CurrentPage);
}
```

---

## 7. 핵심 특징

### 7.1. Canvas 기반 정확한 스냅

- **위치 계산**:
  - 페이지 0: X = 0
  - 페이지 1: X = -(PageSize.X + PageSpacing)
  - 페이지 2: X = -2 * (PageSize.X + PageSpacing)
- **음수 사용 이유**: 컨테이너를 왼쪽/위로 이동시키면서 페이지는 고정 위치 유지

### 7.2. 유연한 애니메이션

- **Timer 기반**: ~60 FPS (0.016초 간격)
- **Ease-out curve**: `FMath::InterpEaseOut(0.0f, 1.0f, Alpha, 2.0f)`
- **Lerp 보간**: 부드러운 위치 이동

### 7.3. 수평/수직 통합

- 단일 클래스로 수평/수직 스크롤 모두 지원
- `EPageScrollDirection` enum으로 방향 전환

### 7.4. 스케일 효과 (선택적)

- `bEnableScaleEffect = true`로 중앙 페이지 확대
- 거리 기반 Lerp로 부드러운 스케일 변화
- `UpdateFocusState()` 호출로 Blueprint 확장 가능

---

## 8. 구현 체크리스트

### 코드
- [x] 저작권 헤더 추가 (4개 파일)
- [x] `TObjectPtr<>` 사용
- [x] Doxygen 주석 적용
- [x] `ONEPIECE_API` 매크로 사용
- [x] 변수명 컨벤션 준수

### 기능
- [x] 드래그 감지 (NativeOnMouseButton*)
- [x] 스냅 알고리즘 (CalculateTargetPage)
- [x] 애니메이션 (Timer 기반 Lerp)
- [x] 수평/수직 스크롤 지원
- [x] 동적 페이지 생성 (SetNumberOfPages)
- [x] 스케일 효과
- [x] 페이지 인디케이터 연동
- [x] Mouse Capture 처리
- [x] 클릭 vs 드래그 구분

### 블루프린트
- [ ] `BindWidget` 위젯 생성
- [ ] Canvas Clipping 설정
- [ ] WBP_PageScrollView 제작
- [ ] WBP_PageScrollItem 제작
- [ ] WBP_PageScrollDot 제작
- [ ] WBP_DotItem 제작
- [ ] 에디터 프리뷰 동작 확인

### 이벤트
- [x] `OnPageChanged` 델리게이트
- [x] `OnItemFocusChanged` 델리게이트

---

## 9. 참고 파일

### 생성된 소스 파일
```
Source/Onepiece/WidgetComponents/
├─ Public/
│  ├─ EPageScrollDirection.h
│  ├─ UPageScrollView.h
│  ├─ UPageScrollItem.h
│  └─ UPageScrollDot.h
├─ Private/
│  ├─ UPageScrollView.cpp
│  ├─ UPageScrollItem.cpp
│  └─ UPageScrollDot.cpp
```

### 생성할 블루프린트 예시
```
Content/CustomContents/UI/Components/
├─ WBP_PageScrollView.uasset
├─ WBP_PageScrollItem.uasset
├─ WBP_PageScrollDot.uasset
└─ WBP_DotItem.uasset
```

### 참조한 기존 코드
- **E:\UE\Onepiece\Source\Onepiece\WidgetComponents\Public\UTabIndicator.h**
  *Timer 기반 애니메이션 패턴 참조*
- **E:\UE\Onepiece\Source\Onepiece\UI\Public\DrawingBoardWidget.h**
  *NativeOnMouseButton* 패턴 참조*
- **E:\UE\Onepiece\Documents\DevLog\AgentLog\KLingo_TabButtonGroup_Design.md**
  *설계 문서 형식 참조*

### Unity 참조 코드
- **E:\UE\Onepiece\Documents\Unity\PageView\UIPagingViewController.cs**
  *드래그 & 스냅 로직 참조*
- **E:\UE\Onepiece\Documents\Unity\PageView\UIPageControl.cs**
  *인디케이터 동적 생성 패턴*
- **E:\UE\Onepiece\Documents\Unity\PageView\CenteredObjectScaler.cs**
  *스케일 효과 참조*

---

## 10. 주의사항

### 10.1. Canvas Clipping 필수
```cpp
// WBP_PageScrollView의 Canvas_Root에 반드시 설정
Canvas_Root->SetClipping(EWidgetClipping::ClipToBounds);
```
- 설정하지 않으면 페이지가 뷰포트 밖으로 보임

### 10.2. Mouse Capture
```cpp
// NativeOnMouseButtonDown에서
return FReply::Handled().CaptureMouse(SharedThis(this));

// NativeOnMouseButtonUp에서
return FReply::Handled().ReleaseMouseCapture();
```
- 드래그 중 마우스가 위젯 밖으로 나가도 계속 추적

### 10.3. 버튼 클릭 vs 드래그 구분
```cpp
// 작은 드래그 거리 (<= 10 픽셀)는 클릭으로 처리
float dragDistance = dragDelta.Size();
if (dragDistance <= 10.0f)
{
    return FReply::Handled().ReleaseMouseCapture();
}
```

### 10.4. Blueprint 구현 필수
- `WBP_PageScrollItem`의 `SetPageContent(int32 Index)` 이벤트 구현 필수
- `WBP_DotItem`에 "Image_Dot" 이름의 Image 컴포넌트 배치 권장

---

## 11. 문제 해결

### 인디케이터 위치가 안 맞을 때
1. PageContainer의 Anchors가 (0,0,0,0)인지 확인
2. PageSize가 실제 페이지 크기와 일치하는지 확인
3. PageSpacing이 올바르게 설정되었는지 확인

### 애니메이션이 끊길 때
1. SnapAnimationSpeed 값 확인 (너무 작으면 즉시 이동처럼 보임)
2. Timer 간격 확인 (0.016f = 60 FPS)

### 드래그가 동작하지 않을 때
1. Canvas_Root의 Visibility가 Visible인지 확인
2. NativeOnMouseButtonDown에서 `FReply::Handled()` 반환 확인
3. Mouse Capture가 올바르게 설정되었는지 확인

### 페이지 생성이 안 될 때
1. PageItemClass가 설정되었는지 확인
2. PageScrollItem을 상속받은 Blueprint 클래스인지 확인
3. SetNumberOfPages() 호출 확인

---

이 설계 문서는 Unity의 UIPagingViewController를 Unreal Engine 5.6 UMG로 포팅하며, Onepiece 프로젝트의 기존 TabButtonGroup 패턴을 재사용합니다. Canvas 기반 접근 방식과 Timer 기반 애니메이션으로 정확하고 부드러운 페이지 스크롤을 제공합니다.
