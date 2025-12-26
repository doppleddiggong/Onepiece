# HowToPlay 팝업 Blueprint 제작 가이드

## 개요

이 문서는 **Popup_HowToPlay** Blueprint 위젯을 제작하는 상세 가이드입니다. Control, Read, Listen, Speak, Write 5개 페이지로 구성된 게임 튜토리얼 팝업을 만듭니다.

---

## 1. 사전 준비: 공통 위젯 생성

### 1.1. WBP_DotItem (페이지 인디케이터 도트)

**경로**: `Content/CustomContents/UI/Components/WBP_DotItem`
**부모 클래스**: `UserWidget`

**계층 구조**:
```
CanvasPanel (루트)
└─ Image (이름: Image_Dot)
    - Anchor: 중앙 (0.5, 0.5, 0.5, 0.5)
    - Size: 10 × 10
    - Color: White
```

**설정**:
- **Image_Dot**:
  - Brush: 원형 또는 점 모양 텍스처
  - Color and Opacity: (R=1, G=1, B=1, A=1)
  - Size To Content: false

---

### 1.2. WBP_PageScrollDot (인디케이터 컨테이너)

**경로**: `Content/CustomContents/UI/Components/WBP_PageScrollDot`
**부모 클래스**: `PageScrollDot` (C++)

**계층 구조**:
```
HorizontalBox (이름: DotContainer)
    - Horizontal Alignment: Center
    - Vertical Alignment: Center
    - Spacing: 10
```

**클래스 설정**:
- **Dot Widget Class**: `WBP_DotItem`
- **Selected Color**: White (1, 1, 1, 1)
- **Unselected Color**: Gray (0.5, 0.5, 0.5, 1)
- **Selected Scale**: 1.2
- **Unselected Scale**: 1.0
- **Animation Speed**: 0.15

---

## 2. WBP_HowToPlayPage (개별 페이지)

**경로**: `Content/CustomContents/UI/Popups/HowToPlay/WBP_HowToPlayPage`
**부모 클래스**: `HowToPlayPageItem` (C++)

### 2.1. 계층 구조

```
CanvasPanel (루트, Size: 1920 × 1080)
├─ VerticalBox (이름: VBox_Content)
│   ├─ Image (이름: Image_Icon, Size: 256 × 256)
│   │   - Alignment: Center
│   │   - Margin: (0, 50, 0, 30)
│   ├─ TextBlock (이름: Txt_Title)
│   │   - Font Size: 72
│   │   - Justification: Center
│   │   - Color: White
│   │   - Margin: (0, 0, 0, 20)
│   ├─ TextBlock (이름: Txt_Description)
│   │   - Font Size: 36
│   │   - Justification: Center
│   │   - Color: Light Gray (0.8, 0.8, 0.8)
│   │   - Auto Wrap Text: true
│   │   - Margin: (100, 0, 100, 30)
│   └─ VerticalBox (이름: VBox_GuideSteps)
│       - Padding: (200, 0, 200, 0)
│       - Spacing: 10
│       └─ [동적 생성되는 가이드 단계 TextBlock들]
└─ Image (이름: Image_Background, Optional)
    - Z-Order: -1 (배경)
    - Brush: 반투명 어두운 색
```

### 2.2. Blueprint Event Graph

#### Event: OnPageDataSet (Blueprint Implementable Event)

```
OnPageDataSet (FHowToPlayPageData InPageData)
    ↓
[Branch] Is Valid (InPageData.IconTexture)
    ├─ True → [Image_Icon] Set Brush from Texture (InPageData.IconTexture)
    └─ False → [Image_Icon] Set Visibility (Collapsed)
    ↓
[Txt_Title] Set Text (InPageData.Title)
    ↓
[Txt_Description] Set Text (InPageData.Description)
    ↓
[VBox_GuideSteps] Clear Children
    ↓
[ForEachLoop] InPageData.GuideSteps
    ↓
    [Create Widget] TextBlock
        - Text: Array Element
        - Font Size: 28
        - Color: White
        - Justification: Left
        - Prefix: "• " (bullet point)
    ↓
    [VBox_GuideSteps] Add Child (TextBlock)
```

**주요 로직**:
1. 아이콘 텍스처가 있으면 표시, 없으면 숨김
2. 제목과 설명 텍스트 설정
3. 가이드 단계 리스트를 순회하며 TextBlock 동적 생성
4. 각 단계 앞에 "• " (bullet point) 추가

---

## 3. WBP_Popup_HowToPlay (메인 팝업)

**경로**: `Content/CustomContents/UI/Popups/WBP_Popup_HowToPlay`
**부모 클래스**: `Popup_HowToPlay` (C++)

### 3.1. 계층 구조

```
Overlay (루트)
├─ Image (이름: Image_Dimmed, Fill Screen)
│   - Brush Color: Black (0, 0, 0, 0.7) - 반투명 검정
│   - Z-Order: 0
├─ SizeBox (이름: SizeBox_Popup)
│   - Width Override: 1920
│   - Height Override: 1080
│   - Horizontal Alignment: Center
│   - Vertical Alignment: Center
│   └─ Border (이름: Border_PopupContainer)
│       - Brush Color: Dark Blue (0.05, 0.05, 0.2, 0.95)
│       - Padding: (50, 50, 50, 50)
│       └─ CanvasPanel (이름: Canvas_Main)
│           ├─ TextBlock (이름: Txt_PopupTitle)
│           │   - Text: "How to Play"
│           │   - Font Size: 64
│           │   - Position: (0, 0)
│           │   - Anchors: Top Center (0.5, 0, 0.5, 0)
│           ├─ UTextureButton (이름: Btn_Close)
│           │   - Position: (1870, 10)
│           │   - Size: 80 × 80
│           │   - Anchors: Top Right (1, 0, 1, 0)
│           │   - Normal Texture: (X 아이콘 텍스처)
│           ├─ CanvasPanel (이름: Canvas_PageScrollViewContainer)
│           │   - Position: (0, 100)
│           │   - Size: (1820, 850)
│           │   - Anchors: Fill (0, 0, 1, 1) with offsets
│           │   └─ SizeBox (PageScrollView 루트)
│           │       - Width Override: 1820
│           │       - Height Override: 850
│           │       └─ CanvasPanel (이름: Canvas_Root, Clipping = ClipToBounds)
│           │           ├─ CanvasPanel (이름: PageContainer, Anchors = (0,0,0,0))
│           │           │   └─ [동적 생성되는 페이지들]
│           │           └─ PageScrollDot (이름: PageDotIndicator)
│           │               - Position: (910, 800)
│           │               - Anchors: Bottom Center (0.5, 1, 0.5, 1)
│           ├─ UTextureButton (이름: Btn_Prev)
│           │   - Position: (50, 500)
│           │   - Size: 80 × 80
│           │   - Anchors: Left Center (0, 0.5, 0, 0.5)
│           │   - Normal Texture: (왼쪽 화살표 텍스처)
│           └─ UTextureButton (이름: Btn_Next)
│               - Position: (1690, 500)
│               - Size: 80 × 80
│               - Anchors: Right Center (1, 0.5, 1, 0.5)
│               - Normal Texture: (오른쪽 화살표 텍스처)
```

### 3.2. PageScrollView 설정 (C++ 프로퍼티)

**Class Settings → Page Scroll View**:
- **Scroll Direction**: Horizontal
- **Page Size**: (1820, 850)
- **Page Spacing**: 0
- **Fast Swipe Threshold**: 500.0
- **Snap Animation Speed**: 0.3
- **Enable Scale Effect**: false (또는 true로 설정하여 중앙 페이지 확대)
- **Focus Scale**: 1.0
- **Side Scale**: 0.9 (Enable Scale Effect = true일 때)
- **Page Item Class**: `WBP_HowToPlayPage`
- **Default Page Index**: 0

### 3.3. 페이지 데이터 설정 (C++ 프로퍼티)

**Class Settings → Page Data**:

#### Control Page Data
- **Page Type**: Control
- **Title**: "게임 조작법"
- **Description**: "마우스와 키보드로 게임을 플레이하는 방법을 배워보세요."
- **Icon Texture**: (컨트롤러 아이콘 텍스처)
- **Guide Steps**:
  1. "W, A, S, D 키로 캐릭터를 이동합니다."
  2. "마우스로 화면을 드래그하여 시점을 회전합니다."
  3. "E 키로 오브젝트와 상호작용합니다."
  4. "ESC 키로 메뉴를 열 수 있습니다."

#### Read Page Data
- **Page Type**: Read
- **Title**: "읽기 학습"
- **Description**: "텍스트를 읽고 이해하는 미션을 수행합니다."
- **Icon Texture**: (책 아이콘 텍스처)
- **Guide Steps**:
  1. "화면에 표시되는 영어 문장을 읽습니다."
  2. "제한 시간 내에 문장의 의미를 파악합니다."
  3. "정답을 선택하여 점수를 획득합니다."

#### Listen Page Data
- **Page Type**: Listen
- **Title**: "듣기 학습"
- **Description**: "음성을 듣고 이해하는 미션을 수행합니다."
- **Icon Texture**: (헤드폰 아이콘 텍스처)
- **Guide Steps**:
  1. "스피커 버튼을 클릭하여 음성을 재생합니다."
  2. "들은 내용을 바탕으로 문제를 풉니다."
  3. "필요하면 음성을 다시 들을 수 있습니다."

#### Speak Page Data
- **Page Type**: Speak
- **Title**: "말하기 학습"
- **Description**: "마이크로 음성을 녹음하여 답변합니다."
- **Icon Texture**: (마이크 아이콘 텍스처)
- **Guide Steps**:
  1. "마이크 버튼을 클릭하여 녹음을 시작합니다."
  2. "주어진 문장을 또박또박 읽습니다."
  3. "녹음을 종료하고 AI가 평가합니다."

#### Write Page Data
- **Page Type**: Write
- **Title**: "쓰기 학습"
- **Description**: "키보드로 답변을 입력합니다."
- **Icon Texture**: (키보드 아이콘 텍스처)
- **Guide Steps**:
  1. "입력 필드를 클릭하여 키보드를 활성화합니다."
  2. "정답을 영어로 입력합니다."
  3. "Enter 키를 눌러 답변을 제출합니다."

### 3.4. Widget Animation (선택적)

**참고**: UBasePopup이 자동으로 열기/닫기 애니메이션을 처리합니다. 커스텀 애니메이션을 원한다면 아래 내용을 참고하세요.

#### Anim_Open (열기 애니메이션, 선택적)
- **Duration**: 0.3초
- **Tracks**:
  - **Border_PopupContainer**:
    - Scale: (0.8, 0.8) → (1.0, 1.0)
    - Opacity: 0.0 → 1.0
  - **Image_Dimmed**:
    - Opacity: 0.0 → 0.7

#### Anim_Close (닫기 애니메이션, 선택적)
- **Duration**: 0.2초
- **Tracks**:
  - **Border_PopupContainer**:
    - Scale: (1.0, 1.0) → (0.8, 0.8)
    - Opacity: 1.0 → 0.0
  - **Image_Dimmed**:
    - Opacity: 0.7 → 0.0

### 3.5. Blueprint Event Graph

**참고**: UBasePopup이 자동으로 InitPopup()을 호출하고 애니메이션을 처리하므로, 별도의 Blueprint 이벤트 구현이 필요하지 않습니다. 커스텀 동작이 필요한 경우에만 아래 이벤트를 재정의하세요.

#### Event: PlayOpenAnimation (선택적, 커스텀 애니메이션)

```
PlayOpenAnimation (Override)
    ↓
[Play Animation] Anim_Open
    - Play Mode: Forward
    - Num Loops To Play: 1
```

#### Event: PlayCloseAnimation (선택적, 커스텀 애니메이션)

```
PlayCloseAnimation (Override)
    ↓
[Play Animation] Anim_Close
    - Play Mode: Forward
    - Num Loops To Play: 1
    ↓
[Delay] 0.2초 (애니메이션 완료 대기)
    ↓
[Parent: PlayCloseAnimation] (부모 클래스 호출로 Remove from Parent)
```

---

## 4. 사용 예시

### 4.1. 팝업 열기 (다른 위젯에서)

```
[Event] 버튼 클릭 (예: "How to Play" 버튼)
    ↓
[Create Widget] WBP_Popup_HowToPlay
    ↓
[Add to Viewport]
    - Z-Order: 100 (최상위)
```

### 4.2. UPopupManager를 통한 팝업 열기 (권장)

```
[Event] 버튼 클릭 (예: "How to Play" 버튼)
    ↓
[Get] UPopupManager::Get(GetWorld())
    ↓
[ShowPopup] WBP_Popup_HowToPlay
    - Z-Order: 자동 관리
```

### 4.3. 특정 페이지로 시작

```
[Create Widget] WBP_Popup_HowToPlay
    ↓
[Cast to Popup_HowToPlay]
    ↓
[GotoPage] EHowToPlayPageType::Listen (예: Listen 페이지로 시작)
    ↓
[Add to Viewport]
```

---

## 5. 스타일 커스터마이징 가이드

### 5.1. 색상 테마 변경

**파란색 테마** (기본):
- Border_PopupContainer: Dark Blue (0.05, 0.05, 0.2, 0.95)
- Button_Close Hover: Light Blue (0.3, 0.5, 0.8)
- PageDotIndicator Selected: Cyan (0.2, 0.8, 1.0)

**빨간색 테마**:
- Border_PopupContainer: Dark Red (0.2, 0.05, 0.05, 0.95)
- Button_Close Hover: Light Red (0.8, 0.3, 0.3)
- PageDotIndicator Selected: Orange (1.0, 0.5, 0.2)

### 5.2. 폰트 변경

모든 TextBlock의 Font Family를 프로젝트 폰트로 변경:
- **Txt_PopupTitle**: Roboto Bold 64
- **Txt_Title (페이지)**: Roboto Bold 72
- **Txt_Description (페이지)**: Roboto Regular 36
- **Guide Steps**: Roboto Regular 28

### 5.3. 페이지 크기 변경

더 큰 페이지가 필요하다면:
1. **SizeBox_Popup**: Width Override = 2560, Height Override = 1440
2. **PageScrollView → Page Size**: (2460, 1290)
3. **Canvas_PageScrollViewContainer**: Size = (2460, 1290)

---

## 6. 디버깅 체크리스트

### 드래그가 동작하지 않을 때
- [ ] Canvas_Root의 Clipping이 `ClipToBounds`로 설정되었는지 확인
- [ ] Canvas_Root의 Visibility가 `Visible`인지 확인
- [ ] PageScrollView의 Scroll Direction이 올바른지 확인

### 페이지가 생성되지 않을 때
- [ ] PageScrollView의 Page Item Class가 `WBP_HowToPlayPage`로 설정되었는지 확인
- [ ] NativeConstruct에서 `InitPopup()` 호출 확인
- [ ] PageContainer의 BindWidget 이름이 정확히 일치하는지 확인

### 인디케이터가 보이지 않을 때
- [ ] PageDotIndicator의 Dot Widget Class가 `WBP_DotItem`로 설정되었는지 확인
- [ ] WBP_DotItem에 "Image_Dot" 이름의 Image가 있는지 확인
- [ ] DotContainer의 BindWidget 이름이 정확히 일치하는지 확인

### 버튼이 동작하지 않을 때
- [ ] Btn_Close, Btn_Prev, Btn_Next의 BindWidget 이름 확인 (UTextureButton 타입)
- [ ] C++ InitPopup()에서 OnButtonClickedEvent 바인딩 확인
- [ ] 버튼이 Canvas 위에 배치되어 클릭 가능한지 확인 (Z-Order)
- [ ] 버튼의 Is Enabled가 true인지 확인
- [ ] UTextureButton의 Normal Texture가 설정되었는지 확인

### 애니메이션이 재생되지 않을 때
- [ ] Widget Animation 에셋이 생성되었는지 확인 (Anim_Open, Anim_Close)
- [ ] PlayOpenAnimation, PlayCloseAnimation 이벤트가 구현되었는지 확인
- [ ] Animation 트랙에 Border_PopupContainer가 추가되었는지 확인

---

## 7. 확장 아이디어

### 7.1. 음성 가이드 추가

각 페이지마다 음성 가이드 재생:
```
OnPageDataSet
    ↓
[Branch] Is Valid (InPageData.VoiceGuideSoundWave)
    ↓ True
    [Play Sound 2D] (InPageData.VoiceGuideSoundWave)
```

### 7.2. 체크리스트 기능

사용자가 각 단계를 완료했는지 체크:
```
VBox_GuideSteps
└─ HorizontalBox (각 단계)
    ├─ CheckBox (완료 체크)
    └─ TextBlock (단계 설명)
```

### 7.3. 비디오 튜토리얼

페이지마다 비디오 재생:
```
VBox_Content
└─ MediaPlayer Widget
    - Video Source: (페이지별 튜토리얼 비디오)
```

---

## 8. 파일 참조

### 생성된 C++ 파일
```
Source/Onepiece/MessageBox/
├─ Public/
│  ├─ UHowToPlayPageItem.h
│  └─ UPopup_HowToPlay.h
├─ Private/
│  ├─ UHowToPlayPageItem.cpp
│  └─ UPopup_HowToPlay.cpp
```

### 참조 파일
```
Source/Onepiece/MessageBox/
├─ Public/
│  ├─ UBasePopup.h (부모 클래스)
│  └─ UPopupManager.h (팝업 관리자)
├─ Private/
│  ├─ UBasePopup.cpp
│  └─ UPopupManager.cpp

Source/Onepiece/WidgetComponents/
├─ Public/
│  ├─ UPageScrollView.h (페이지 스크롤 뷰)
│  ├─ UPageScrollItem.h (페이지 아이템 베이스)
│  └─ UPageScrollDot.h (페이지 인디케이터)
```

### 생성할 Blueprint 파일
```
Content/CustomContents/UI/
├─ Components/
│  ├─ WBP_DotItem.uasset
│  └─ WBP_PageScrollDot.uasset
├─ Popups/HowToPlay/
│  ├─ WBP_HowToPlayPage.uasset
│  └─ WBP_Popup_HowToPlay.uasset
```

---

이 가이드를 따라 Blueprint를 제작하면 완전히 작동하는 HowToPlay 팝업을 만들 수 있습니다. 각 단계를 차근차근 따라가며 위젯을 구성하세요!
