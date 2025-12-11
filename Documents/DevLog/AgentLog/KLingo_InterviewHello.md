# 🚀 UPopup_InterviewHello 구현 명세서

## 📋 프로젝트 정보
- **프로젝트명**: Onepiece (KLingo)
- **엔진 버전**: Unreal Engine 5.4
- **모듈명**: ONEPIECE_API
- **로그 카테고리**: LogOnepiece
- **기존 구현**: UPopup_Interview (ScrollBox 방식) → UPopup_InterviewHello (단일 질문 방식)

## 🎯 구현 목표
기존 ScrollBox 방식의 Interview Popup을 Duolingo 스타일의 단일 질문 방식으로 재구현합니다.

### 기존 vs 새로운 구조 비교

| 항목 | 기존 (UPopup_Interview) | 새로운 (UPopup_InterviewHello) |
|------|-------------------------|--------------------------------|
| **표시 방식** | ScrollBox에 모든 질문 표시 | 한 번에 하나의 질문만 표시 |
| **네비게이션** | 스크롤 | Prev/Next 버튼 |
| **답변 저장** | 각 Item Widget에 저장 | 팝업 내부 배열(TempAnswers)에 저장 |
| **진행 표시** | 없음 | Progress Bar |
| **추가 기능** | 없음 | "Today do not show" 체크박스 |

## 📸 UI 참조
- **UI 디자인**: `Documents/DevLog/AgentLog/Popup_InterviewHello.png`
- **위젯 구조**: `Documents/DevLog/AgentLog/Popup_InterviewHello_Comp.png`

---

## 📌 1. 클래스 구조

### 상속 관계
```cpp
UPopup_InterviewHello : public UBasePopup
```
- **UBasePopup**은 모든 팝업의 기본 클래스
- OpenAnimation() 기능 제공
- Copyright 헤더 포함 필수

### 파일 위치
- **Header**: `Source/Onepiece/MessageBox/Public/UPopup_InterviewHello.h`
- **Source**: `Source/Onepiece/MessageBox/Private/UPopup_InterviewHello.cpp`

---

## 📌 2. UI 위젯 구조 (BindWidget)

UI 이미지(`Popup_InterviewHello_Comp.png`)를 참고하여 다음 위젯들을 BindWidget으로 선언:

| 위젯 이름 | 타입 | 설명 |
|-----------|------|------|
| **Txt_Title** | UTextBlock | 타이틀 ("Interview") |
| **TXt_Question** | UTextBlock | 현재 질문 텍스트 (영어) |
| **Edit_Answer** | UEditableText or UMultiLineEditableText | 플레이어 답변 입력란 |
| **ProgressBar_Question** | UProgressBar | 질문 진행률 표시 |
| **Button_PrevArrow** | UButton or UTextureButton | 이전 질문 (◀) |
| **Btn_Submit** or **Btn_Next** | UButton or UImageButton | Next/Submit 전환 버튼 |
| **Button_NextArrow** | UButton or UTextureButton | 다음 질문 (▶) |
| **Button_CheckToday** | UCheckBox | "Today do not show" 체크박스 |
| **Btn_Close** | UButton or UTextureButton | 팝업 닫기 (우측 상단 ❌) |

**참고**: 위젯 이름은 UMG 블루프린트의 실제 이름과 정확히 일치해야 합니다.

---

## 📌 3. 데이터 구조 (NetworkData.h 참조)

프로젝트의 `NetworkData.h`에 이미 정의된 구조체를 사용합니다:

### FInterviewQuestionData
```cpp
USTRUCT(BlueprintType)
struct FInterviewQuestionData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    int32 Id;           // 질문 고유 ID

    UPROPERTY(BlueprintReadWrite)
    int32 TypeCode;     // 질문 타입 코드

    UPROPERTY(BlueprintReadWrite)
    FString Eng;        // 영어 질문

    UPROPERTY(BlueprintReadWrite)
    FString Kor;        // 한국어 질문

    UPROPERTY(BlueprintReadWrite)
    FString EngKey;     // 영어 키

    UPROPERTY(BlueprintReadWrite)
    FString KorKey;     // 한국어 키

    UPROPERTY(BlueprintReadWrite)
    FString CreatedAt;  // 생성 시간
};
```

### FResponseInterviewHello
```cpp
USTRUCT(BlueprintType)
struct FResponseInterviewHello
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Interview")
    TArray<FInterviewQuestionData> Questions;
};
```

### FInterviewAnswerData
```cpp
USTRUCT(BlueprintType)
struct FInterviewAnswerData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    int interview_id;   // 질문 ID (FInterviewQuestionData.Id)

    UPROPERTY(BlueprintReadWrite)
    FString answer;     // 플레이어 답변

    UPROPERTY(BlueprintReadWrite)
    int user_id;        // 유저 ID
};
```

### FRequestInterviewAnswer
```cpp
USTRUCT(BlueprintType)
struct FRequestInterviewAnswer
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Interview")
    TArray<FInterviewAnswerData> answer;
};
```

### 초기화
```cpp
void InitPopup(const FResponseInterviewHello& InterviewData);
```
- API 응답으로 받은 질문 배열을 팝업에 전달합니다.

---

## 📌 4. UX 동작 규칙

### 4.1 단일 질문 모드 (Duolingo 스타일)
- 한 번에 **질문 하나만** 표시
- 현재 질문 인덱스: `int32 CurrentQuestionIndex`
- 총 질문 개수: `SavedQuestions.Num()`

### 4.2 Progress Bar
```cpp
float Progress = (float)(CurrentQuestionIndex + 1) / (float)SavedQuestions.Num();
ProgressBar_Question->SetPercent(Progress);
```

### 4.3 Prev / Next 네비게이션 규칙

| 버튼 | 동작 | 표시 조건 |
|------|------|----------|
| **Button_PrevArrow** | `CurrentQuestionIndex--` | `CurrentQuestionIndex > 0` |
| **Button_NextArrow** | `CurrentQuestionIndex++` | `CurrentQuestionIndex < SavedQuestions.Num() - 1` |
| **Btn_Next** (중앙 Next) | `CurrentQuestionIndex++` | 마지막 질문 아님 AND 모든 답변 완료 안됨 |
| **Btn_Submit** (중앙 Submit) | 답변 제출 | 마지막 질문 AND 모든 답변 완료됨 |

**중요**: 범위를 벗어나는 버튼은 `SetVisibility(ESlateVisibility::Hidden)` 처리

### 4.4 답변 임시 저장

```cpp
TArray<FString> TempAnswers;  // Questions.Num()과 크기 동일
```

#### 이동 시 저장/로드 프로세스
1. **떠나기 전**: `SaveCurrentAnswer()`
   ```cpp
   TempAnswers[CurrentQuestionIndex] = Edit_Answer->GetText().ToString();
   ```

2. **이동 후**: `LoadCurrentAnswer()`
   ```cpp
   Edit_Answer->SetText(FText::FromString(TempAnswers[CurrentQuestionIndex]));
   ```

3. **질문 내용 업데이트**: `RefreshUI()`
   ```cpp
   TXt_Question->SetText(FText::FromString(SavedQuestions[CurrentQuestionIndex].Eng));
   ```

### 4.5 Submit 버튼 상태 (옵션 1: 마지막 질문에서 항상 표시)

```cpp
void UpdateSubmitButtonState()
{
    // 모든 답변이 채워졌는지 확인
    bool bAllAnswered = true;
    for (const FString& Answer : TempAnswers)
    {
        if (Answer.TrimStartAndEnd().IsEmpty())
        {
            bAllAnswered = false;
            break;
        }
    }

    // 마지막 질문인지 확인
    bool bIsLastQuestion = (CurrentQuestionIndex == SavedQuestions.Num() - 1);

    if (bIsLastQuestion)
    {
        // 마지막 질문이면 무조건 Submit 표시
        if (Btn_Submit)
        {
            Btn_Submit->SetVisibility(ESlateVisibility::Visible);
            // 모든 답변이 완료되었을 때만 활성화
            Btn_Submit->SetIsEnabled(bAllAnswered);
        }
        if (Btn_Next)
        {
            Btn_Next->SetVisibility(ESlateVisibility::Hidden);
        }
    }
    else
    {
        // 마지막 질문이 아니면 Next 표시
        if (Btn_Next)
        {
            Btn_Next->SetVisibility(ESlateVisibility::Visible);
        }
        if (Btn_Submit)
        {
            Btn_Submit->SetVisibility(ESlateVisibility::Hidden);
        }
    }
}
```

**UX 개선점:**
- ✅ 마지막 질문에 도달하면 Submit 버튼이 항상 표시됨
- ✅ 모든 답변이 완료되지 않으면 Submit 버튼이 **비활성화** 상태로 표시됨
- ✅ 사용자가 "아직 답변하지 않은 질문이 있구나" 인지 가능

**실시간 업데이트:**
```cpp
// Edit_Answer의 OnTextChanged 이벤트 바인딩 (NativeConstruct)
if (Edit_Answer)
{
    Edit_Answer->OnTextChanged.AddDynamic(this, &UPopup_InterviewHello::OnAnswerTextChanged);
}

// 텍스트 변경 시 호출
void OnAnswerTextChanged(const FText& Text)
{
    // 현재 답변을 실시간으로 저장
    if (TempAnswers.IsValidIndex(CurrentQuestionIndex))
    {
        TempAnswers[CurrentQuestionIndex] = Text.ToString();
    }

    // Submit 버튼 상태 즉시 업데이트
    UpdateSubmitButtonState();
}
```

이렇게 하면 사용자가 마지막 질문에서 답변을 입력하는 즉시 Submit 버튼이 **활성화**됩니다! ✅

### 4.6 제출 (OnClickSubmit)

```cpp
void UPopup_InterviewHello::OnClickSubmit()
{
    // 1. 답변 데이터 생성
    TArray<FInterviewAnswerData> AnswerDataList;
    int32 UserId = ULingoGameHelper::GetUserId(GetWorld());

    for (int32 i = 0; i < SavedQuestions.Num(); ++i)
    {
        FInterviewAnswerData AnswerData;
        AnswerData.interview_id = SavedQuestions[i].Id;
        AnswerData.answer = TempAnswers[i];
        AnswerData.user_id = UserId;
        AnswerDataList.Add(AnswerData);
    }

    // 2. API 요청
    if (UKLingoNetworkSystem* NetworkSystem = UKLingoNetworkSystem::Get(GetWorld()))
    {
        FRequestInterviewAnswer Request;
        Request.answer = AnswerDataList;

        NetworkSystem->RequestInterviewAnswer(
            Request,
            FResponseInterviewAnswerDelegate::CreateUObject(
                this, &UPopup_InterviewHello::OnResponseInterviewAnswer)
        );
    }
}
```

---

## 📌 5. "오늘 보지 않기" 기능 (Submit 성공 시 저장)

### 5.1 체크박스 상태 저장 (OnCheckToday)
```cpp
// 멤버 변수
bool bCheckTodayDoNotShow = false;

// 체크박스 이벤트: 상태만 저장 (Config 저장은 Submit 성공 시)
void UPopup_InterviewHello::OnCheckToday(bool bIsChecked)
{
    // 체크박스 상태만 저장 (실제 Config 저장은 Submit 성공 시)
    bCheckTodayDoNotShow = bIsChecked;

    PRINTLOG(TEXT("[UPopup_InterviewHello] 'Today do not show' checkbox: %s"),
        bIsChecked ? TEXT("Checked") : TEXT("Unchecked"));
}
```

### 5.2 Submit 성공 시 Config 저장 (OnResponseInterviewAnswer)
```cpp
void UPopup_InterviewHello::OnResponseInterviewAnswer(FResponseInterviewAnswer& ResponseData, bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        PRINTLOG(TEXT("[UPopup_InterviewHello] Interview Answer SUCCESS"));

        // "Today do not show" 체크되어 있으면 오늘 날짜 저장
        if (bCheckTodayDoNotShow)
        {
            int32 UserId = ULingoGameHelper::GetUserId(GetWorld());
            FString ConfigSection = TEXT("/Script/Onepiece.InterviewPopup");
            FString ConfigKey = FString::Printf(TEXT("SkipInterviewDate_%d"), UserId);

            // 현재 날짜를 "YYYY-MM-DD" 형식으로 저장
            FDateTime Now = FDateTime::Now();
            FString TodayDate = FString::Printf(TEXT("%04d-%02d-%02d"),
                Now.GetYear(), Now.GetMonth(), Now.GetDay());

            GConfig->SetString(
                *ConfigSection,
                *ConfigKey,
                *TodayDate,
                GGameUserSettingsIni
            );
            GConfig->Flush(false, GGameUserSettingsIni);

            PRINTLOG(TEXT("[UPopup_InterviewHello] 'Today do not show' saved for User %d, Date: %s"),
                UserId, *TodayDate);
        }

        // ... (튜터 메시지, 팝업 닫기)
    }
}
```

### 5.3 Config 읽기 및 날짜 비교 (팝업 표시 전 확인)
```cpp
bool ShouldSkipInterviewToday(const UObject* WorldContextObject)
{
    int32 UserId = ULingoGameHelper::GetUserId(WorldContextObject);
    FString ConfigSection = TEXT("/Script/Onepiece.InterviewPopup");
    FString ConfigKey = FString::Printf(TEXT("SkipInterviewDate_%d"), UserId);

    // 저장된 날짜 읽기
    FString SavedDate;
    if (!GConfig->GetString(*ConfigSection, *ConfigKey, SavedDate, GGameUserSettingsIni))
    {
        // 저장된 날짜 없음 → 보여줌
        return false;
    }

    // 오늘 날짜 생성
    FDateTime Now = FDateTime::Now();
    FString TodayDate = FString::Printf(TEXT("%04d-%02d-%02d"),
        Now.GetYear(), Now.GetMonth(), Now.GetDay());

    // 날짜 비교: 저장된 날짜 == 오늘 날짜 → Skip
    if (SavedDate == TodayDate)
    {
        PRINTLOG(TEXT("[Interview] Skipping today for User %d (Saved: %s)"), UserId, *SavedDate);
        return true;  // 오늘은 건너뜀
    }

    // 날짜가 다르면 → 보여줌
    return false;
}
```

**중요 포인트:**
- ✅ **체크박스만으로는 저장 안 됨**: OnCheckToday는 상태만 멤버 변수에 저장
- ✅ **Submit 성공 시 저장**: 체크되어 있고 Submit이 성공해야 Config에 날짜 저장
- ✅ **날짜 저장**: "YYYY-MM-DD" 형식으로 저장
- ✅ **날짜 비교**: 저장된 날짜와 오늘 날짜가 같으면 건너뜀
- ✅ **내일은 표시**: 날짜가 바뀌면 자동으로 다시 보여짐

**참고**: 이 기능은 팝업을 여는 쪽(PopupManager 또는 호출자)에서 체크해야 할 수도 있습니다.

---

## 📌 6. 필수 메서드 구조

### 6.1 Public 메서드
```cpp
/** 팝업 초기화 */
UFUNCTION(BlueprintCallable, Category = "Popup")
void InitPopup(const FResponseInterviewHello& InterviewData);
```

### 6.2 Protected 메서드
```cpp
protected:
    virtual void NativeConstruct() override;
```

### 6.3 Private 메서드

#### UI 업데이트
```cpp
private:
    /** 현재 질문과 답변을 UI에 반영 */
    void RefreshUI();

    /** Prev/Next 화살표 버튼 표시/숨김 처리 */
    void UpdateNavigationButtons();

    /** Submit 버튼 활성화 상태 업데이트 */
    void UpdateSubmitButtonState();

    /** Progress Bar 업데이트 */
    void UpdateProgressBar();
```

#### 답변 관리
```cpp
    /** 현재 질문의 답변을 TempAnswers에 저장 */
    void SaveCurrentAnswer();

    /** TempAnswers에서 현재 질문의 답변을 불러와 Edit_Answer에 표시 */
    void LoadCurrentAnswer();
```

#### 버튼 이벤트
```cpp
    /** 이전 질문으로 이동 */
    UFUNCTION()
    void OnClickPrevArrow();

    /** 다음 질문으로 이동 */
    UFUNCTION()
    void OnClickNextArrow();

    /** Next 버튼 (중앙) 클릭 */
    UFUNCTION()
    void OnClickNext();

    /** Submit 버튼 클릭 */
    UFUNCTION()
    void OnClickSubmit();

    /** 닫기 버튼 클릭 */
    UFUNCTION()
    void OnClickClose();

    /** "Today do not show" 체크박스 변경 */
    UFUNCTION()
    void OnCheckToday(bool bIsChecked);
```

#### API 응답 콜백
```cpp
    /** Interview Answer API 응답 처리 */
    UFUNCTION()
    void OnResponseInterviewAnswer(FResponseInterviewAnswer& ResponseData, bool bWasSuccessful);
```

### 6.4 중요 원칙
- **Null 체크 필수**: 모든 위젯 포인터 사용 전 `if (Widget)` 체크
- **범위 체크**: 배열 접근 시 `IsValidIndex()` 사용
- **예외 없이 완전 동작**: 어떤 상황에서도 크래시 없이 안전하게 동작

---

## 📌 7. 프로젝트 시스템 통합

### 7.1 필수 Include
```cpp
// Header
#include "UBasePopup.h"
#include "NetworkData.h"

// Source
#include "UPopupManager.h"
#include "UKLingoNetworkSystem.h"
#include "ULingoGameHelper.h"
#include "UDialogManager.h"
#include "UBroadcastManager.h"
#include "GameLogging.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Components/ProgressBar.h"
#include "Components/CheckBox.h"
// ... 기타 사용하는 위젯 컴포넌트
```

### 7.2 Manager 클래스 사용
```cpp
// 팝업 닫기
UPopupManager::Get(GetWorld())->HideCurrentPopup();

// 네트워크 요청
UKLingoNetworkSystem::Get(GetWorld())->RequestInterviewAnswer(...);

// Toast 메시지
UDialogManager::Get(GetWorld())->ShowToast(Message);

// 튜터 메시지
UBroadcastManager::Get(GetWorld())->SendTutorMessage(Message);

// UserId 가져오기
int32 UserId = ULingoGameHelper::GetUserId(GetWorld());
```

### 7.3 로그
```cpp
#include "GameLogging.h"

PRINTLOG(TEXT("Interview Answer SUCCESS"));
```

---

## 📌 8. 체크리스트 (구현 필수 기능)

### ✅ 핵심 기능
- [ ] 단일 질문 표시 (한 번에 하나)
- [ ] Prev/Next 화살표 버튼 네비게이션
- [ ] 범위 벗어난 버튼 자동 숨김
- [ ] 답변 임시 저장 배열 (`TempAnswers`)
- [ ] 이동 시 `SaveCurrentAnswer()` / `LoadCurrentAnswer()`
- [ ] Progress Bar 업데이트
- [ ] Submit 버튼 활성화 조건 (모든 답변 완료 + 마지막 질문)
- [ ] Next ↔ Submit 버튼 전환

### ✅ API 통신
- [ ] `InitPopup(const FResponseInterviewHello&)`에서 데이터 초기화
- [ ] `TempAnswers.SetNum(Questions.Num())` 초기화
- [ ] Submit 시 `FInterviewAnswerData` 배열 생성
- [ ] `UKLingoNetworkSystem::RequestInterviewAnswer()` 호출
- [ ] `OnResponseInterviewAnswer()` 콜백 처리

### ✅ 로컬 저장
- [ ] "Today do not show" 체크박스 바인딩
- [ ] `OnCheckToday(bool bIsChecked)` 구현
- [ ] GConfig를 이용한 Config 저장

### ✅ 안전성
- [ ] 모든 위젯 포인터 Null 체크
- [ ] 배열 접근 시 `IsValidIndex()` 체크
- [ ] 중복 바인딩 방지 (`RemoveDynamic` → `AddDynamic`)

---

## 📌 9. 코드 생성 가이드

### 9.1 파일 구조
```
Source/Onepiece/MessageBox/
├── Public/
│   └── UPopup_InterviewHello.h
└── Private/
    └── UPopup_InterviewHello.cpp
```

### 9.2 Copyright 헤더
```cpp
// Copyright (c) 2025 Doppleddiggong. All rights reserved.
// Unauthorized copying, modification, or distribution of this file,
// via any medium is strictly prohibited. Proprietary and confidential.
```

### 9.3 클래스 선언 예시
```cpp
UCLASS()
class ONEPIECE_API UPopup_InterviewHello : public UBasePopup
{
    GENERATED_BODY()

public:
    // Public 메서드

protected:
    virtual void NativeConstruct() override;

private:
    // Private 메서드
    // ...

protected:
    // BindWidget 위젯들
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UTextBlock> Txt_Title;
    // ...

private:
    // 멤버 변수
    TArray<FInterviewQuestionData> SavedQuestions;
    TArray<FString> TempAnswers;
    int32 CurrentQuestionIndex = 0;
};
```

### 9.4 빌드 요구사항
- UE 5.4 호환
- `ONEPIECE_API` 매크로 사용
- Unreal C++ 코딩 컨벤션 준수
- `TObjectPtr<>` 사용 (UE5 스타일)
- 실제 프로젝트에서 즉시 빌드 가능

---

## 📌 10. 참고 자료

### 기존 구현 참조
- `Source/Onepiece/MessageBox/Public/UPopup_Interview.h` (기존 ScrollBox 방식)
- `Source/Onepiece/MessageBox/Private/UPopup_Interview.cpp`

### 관련 시스템
- `UBasePopup`: 팝업 베이스 클래스
- `UPopupManager`: 팝업 표시/숨김 관리
- `UKLingoNetworkSystem`: API 통신
- `ULingoGameHelper`: 유틸리티 함수
- `NetworkData.h`: 데이터 구조 정의

### UI 디자인
- `Documents/DevLog/AgentLog/Popup_InterviewHello.png`
- `Documents/DevLog/AgentLog/Popup_InterviewHello_Comp.png`

---

## 📝 구현 시 주의사항

1. **위젯 이름 정확히 일치**: UMG 블루프린트의 위젯 이름과 BindWidget 변수명이 정확히 일치해야 합니다.
2. **버튼 바인딩**: `NativeConstruct()`에서 모든 버튼 이벤트를 바인딩합니다.
3. **중복 방지**: `RemoveDynamic` → `AddDynamic` 패턴 사용.
4. **인덱스 관리**: `CurrentQuestionIndex`가 유효 범위 내에 있는지 항상 확인.
5. **UI 동기화**: 질문 이동 시마다 `RefreshUI()`, `UpdateNavigationButtons()`, `UpdateSubmitButtonState()`, `UpdateProgressBar()` 호출.
6. **답변 저장**: 이동 전에 반드시 `SaveCurrentAnswer()` 호출.
7. **에러 처리**: API 실패 시 사용자에게 명확한 메시지 표시.

---

**문서 버전**: 1.0
**최종 수정**: 2025-12-11
**작성자**: Claude (Onepiece 프로젝트 분석 기반)