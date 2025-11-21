# KLingo API Quick Tester - Editor Utility Widget 설정 가이드

> **프로젝트**: Onepiece
> **목적**: INI 기반 Quick Test 시스템을 위한 Editor Utility Widget 구성

---

## 📋 사전 준비 사항

### 1. C++ 코드 컴파일 완료 확인

다음 파일들이 생성되어 있어야 합니다:
- ✅ `KLingoAPITestSettings.h/cpp`
- ✅ `UKLingoAPITestSubsystem.h/cpp`
- ✅ `Config/DefaultToolbarSettings.ini` 업데이트 완료

### 2. 프로젝트 빌드

```
1. 언리얼 에디터 닫기
2. .uproject 우클릭 → Generate Visual Studio project files
3. Visual Studio에서 빌드 (Ctrl+Shift+B)
4. 에디터 실행
```

---

## 🎨 Editor Utility Widget 생성

### Step 1: Editor Utility Widget 생성

1. **Content Browser**에서 우클릭
2. **Editor Utilities** → **Editor Utility Widget** 선택
3. 이름: `EUW_KLingoAPITester`
4. 더블클릭하여 열기

---

## 🖼️ UMG 디자이너 레이아웃 구성

### 최종 위젯 구조

```
Canvas Panel
└── Vertical Box (Fill)
    ├── Text Block - 타이틀
    ├── Horizontal Box - 설정
    │   ├── Text Block - "Mock Mode:"
    │   ├── Check Box
    │   ├── Spacer
    │   ├── Text Block - "Environment:"
    │   └── Combo Box String
    ├── Spacer (10px)
    ├── Text Block - "Quick Tests:"
    ├── Scroll Box
    │   └── Vertical Box (ButtonContainer)
    ├── Spacer (10px)
    ├── Text Block - "Last Response:"
    ├── Border
    │   └── Multi Line Editable Text Box (ResponseText)
    └── Text Block - 상태 표시 (StatusText)
```

---

## 📐 상세 위젯 구성

### 1. Canvas Panel (Root)

- **Anchors**: Fill
- **Padding**: 10, 10, 10, 10

---

### 2. Vertical Box (메인 컨테이너)

**Properties**:
- Name: `MainVerticalBox`
- Anchors: Fill
- Alignment: Top, HCenter

---

### 3. Text Block - 타이틀

**Properties**:
- Name: `TitleText`
- Text: `KLingo API Quick Tester`
- Font Size: 24
- Justification: Center
- Color: (R=1, G=1, B=1, A=1)

**Padding**: Bottom 10

---

### 4. Horizontal Box - 설정

#### 4-1. Text Block - "Mock Mode:"
- Text: `Mock Mode:`
- Font Size: 14

#### 4-2. Check Box
- Name: `CheckBox_MockMode`
- Is Enabled: false (현재 미구현)

#### 4-3. Spacer
- Size: Fill

#### 4-4. Text Block - "Environment:"
- Text: `Environment:`
- Font Size: 14

#### 4-5. Combo Box String
- Name: `ComboBox_Environment`
- Options: Dev, Stage, Live
- Default: Dev
- Is Enabled: false (현재 미구현)

**Padding**: Bottom 10

---

### 5. Text Block - "Quick Tests:"

**Properties**:
- Name: `QuickTestsLabel`
- Text: `Quick Tests:`
- Font Size: 16
- Is Bold: true

**Padding**: Bottom 5

---

### 6. Scroll Box

**Properties**:
- Name: `ScrollBox_Tests`
- Orientation: Vertical
- Scroll Bar Visibility: Visible When Needed
- Size: Fill (Height 300)

**내부 위젯**:
- Vertical Box
  - Name: `VerticalBox_ButtonContainer`
  - **여기에 버튼들이 동적으로 추가됩니다**

**Padding**: Bottom 10

---

### 7. Text Block - "Last Response:"

**Properties**:
- Name: `ResponseLabel`
- Text: `Last Response:`
- Font Size: 16
- Is Bold: true

**Padding**: Bottom 5

---

### 8. Border (응답 표시 영역)

**Properties**:
- Name: `Border_Response`
- Brush Color: (R=0.05, G=0.05, B=0.05, A=1)
- Padding: 10

**내부 위젯**:
- **Multi Line Editable Text Box**
  - Name: `MultiLineTextBox_Response`
  - Text: `(No response yet)`
  - Is Read Only: true
  - Font: Consolas or Courier New
  - Font Size: 12
  - Background Color: (R=0.02, G=0.02, B=0.02, A=1)
  - Foreground Color: (R=0.9, G=0.9, B=0.9, A=1)
  - Auto Wrap Text: true
  - Size: Fill (Height 300)

**Padding**: Bottom 10

---

### 9. Text Block - 상태 표시

**Properties**:
- Name: `TextBlock_Status`
- Text: `Ready`
- Font Size: 12
- Color: (R=0.7, G=0.7, B=0.7, A=1)

---

## 🔧 Blueprint Graph 구성

### Variables (변수 추가)

**Graph** → **My Blueprint** → **Variables**에서 다음 변수 추가:

1. **Subsystem** (UKLingoAPITestSubsystem)
   - Type: `KLingo API Test Subsystem` (Object Reference)
   - Category: `References`

2. **QuickTests** (Array)
   - Type: `Quick Test Entry` (Array)
   - Category: `Data`

---

### Event Graph

#### Event: Pre Construct

```
Pre Construct
    ↓
Get Editor Subsystem (KLingoAPITestSubsystem)
    ↓
SET [Subsystem]
    ↓
IS VALID? [Subsystem]
    ↓ (True)
[Subsystem] → Get Quick Tests
    ↓
SET [QuickTests]
    ↓
Clear Children [VerticalBox_ButtonContainer]
    ↓
ForEach [QuickTests]
    ↓
Create Widget (Button Widget - 아래 참조)
    ↓
Add Child to Vertical Box [VerticalBox_ButtonContainer]
```

**Button Widget 생성**:
1. `Create Widget` 노드
2. Class: `CommonButtonBase` 또는 직접 생성
3. 또는 간단하게: `Wrap Box`에 `Button` 추가

**간단한 방법 (추천)**:
```
ForEach [QuickTests]
    ↓
Create Button Widget
    ├─ Get [Array Element].Label → Set Button Text
    ├─ Get [Array Element] → Store in Button Tag (인덱스 저장)
    └─ Bind OnClicked Event
    ↓
Add to [VerticalBox_ButtonContainer]
```

#### Button OnClicked Event (동적 바인딩)

**방법 1: Custom Event 생성**

1. **Custom Event** 생성: `OnQuickTestButtonClicked`
2. Parameters: `TestEntry` (Quick Test Entry)

```
[OnQuickTestButtonClicked]
    ↓
IS VALID? [Subsystem]
    ↓ (True)
Set Text [TextBlock_Status] → "Executing..."
Set Text [MultiLineTextBox_Response] → "Waiting..."
    ↓
[Subsystem] → Execute Quick Test
    └─ TestEntry: [TestEntry]
```

**방법 2: 델리게이트 바인딩**

```
Event Construct
    ↓
IS VALID? [Subsystem]
    ↓ (True)
Bind Event to [Subsystem].OnQuickTestComplete
    ↓
[OnQuickTestComplete Event]
    ├─ Success: bool
    ├─ Response: string
    └─ ElapsedTime: float
    ↓
Branch [Success]
    ├─ True:  Set Status Text → "Success (XXXms)"
    │         Set Response Text → [Response]
    │         Set Text Color → Green
    └─ False: Set Status Text → "Failed (XXXms)"
              Set Response Text → [Response]
              Set Text Color → Red
```

---

## 🎯 최종 Blueprint 예제 (단순화)

### Event Pre Construct

```
[Pre Construct]
    ↓
[Get Editor Subsystem] → KLingoAPITestSubsystem
    ↓
[Set Subsystem]
    ↓
[Subsystem].GetQuickTests
    ↓
[ForEachLoop] → QuickTests Array
    ↓
[CreateWidget] → WBP_QuickTestButton (별도 위젯)
    ↓
[Set Button Label] ← QuickTests[Index].Label
[Set Button Data] ← QuickTests[Index]
    ↓
[Add Child] → VerticalBox_ButtonContainer
```

### Event Construct

```
[Construct]
    ↓
[Bind Event] → Subsystem.OnQuickTestComplete
    ↓ (Bound)
[OnQuickTestComplete]
    ├─ bSuccess: bool
    ├─ Response: string
    └─ ElapsedTime: float
    ↓
[Format Text] → "Status: {0} ({1}ms)"
    ↓
[Set Text] → TextBlock_Status
[Set Text] → MultiLineTextBox_Response ← Response
```

---

## 🚀 실행 방법

### 1. Editor Utility Widget 실행

**방법 A: 직접 실행**
1. Content Browser에서 `EUW_KLingoAPITester` 우클릭
2. **Run Editor Utility Widget** 선택

**방법 B: 메뉴에 추가**
1. Editor Preferences → Plugins → Editor Scripting Utilities
2. Enable: true
3. Tools 메뉴에 추가

---

### 2. Quick Test 실행

1. Widget 열기
2. Quick Test 버튼 클릭 (예: Login Test)
3. Response 확인

**예상 결과**:
```json
{
  "token": "abc123...",
  "playerInfo": {
    "nickname": "TestPlayer",
    "gold": 1000
  }
}
```

---

## 🐛 트러블슈팅

### 버튼이 표시되지 않음

**원인**: QuickTests 배열이 비어있음

**해결**:
1. Project Settings → KLingo API Test 확인
2. Config/DefaultToolbarSettings.ini 확인
3. 에디터 재시작

---

### "Network Error" 응답

**원인**: 서버 연결 실패

**해결**:
1. UCustomNetworkSettings에서 서버 URL 확인
2. 서버가 실행 중인지 확인
3. Mock Mode 사용 (추후 구현)

---

### Subsystem이 null

**원인**: 에디터 전용 코드가 빌드되지 않음

**해결**:
1. `#if WITH_EDITOR` 확인
2. Development Editor 구성으로 빌드
3. 에디터 재시작

---

## 📝 추가 커스터마이징

### 버튼 스타일 변경

**Button Widget Properties**:
- Normal Color: (R=0.2, G=0.2, B=0.8, A=1)
- Hovered Color: (R=0.3, G=0.3, B=1, A=1)
- Pressed Color: (R=0.1, G=0.1, B=0.5, A=1)
- Padding: 10, 5, 10, 5

### 응답 텍스트 색상

성공/실패에 따라 색상 변경:
- Success: Green (R=0.2, G=1, B=0.2, A=1)
- Failed: Red (R=1, G=0.2, B=0.2, A=1)

---

## ✅ 완료 체크리스트

- [ ] Editor Utility Widget 생성
- [ ] UMG 레이아웃 구성
- [ ] Variables 추가
- [ ] Event Pre Construct 구현
- [ ] Event Construct 구현
- [ ] OnQuickTestComplete 바인딩
- [ ] 버튼 동적 생성 확인
- [ ] Quick Test 실행 테스트

---

## 🎉 완성!

이제 에디터에서 KLingo API를 빠르게 테스트할 수 있습니다!

**새 API 추가**:
1. `Config/DefaultToolbarSettings.ini`에 한 줄 추가
2. 에디터 재시작
3. 자동으로 버튼 생성됨

**팀 공유**:
1. Git commit (INI 파일 포함)
2. 팀원 pull
3. 모두 같은 Quick Tests 사용
