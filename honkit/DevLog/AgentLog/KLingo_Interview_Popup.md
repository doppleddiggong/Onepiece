# Interview Popup System 구현 작업 프롬프팅

## 작업 개요
**목표**: Interview 팝업 시스템 완성 및 테스트 환경 구축

**배경**:
- NetworkData.h에 `FInterviewData`와 `FInterviewQuestionData` 구조체는 이미 정의됨 (102-136줄)
- UPopup_Interview와 UPopup_InterviewItem 헤더는 정의되어 있으나 구현체(.cpp)는 비어있음
- 서버로부터 받은 인터뷰 질문 데이터를 팝업으로 표시하고 사용자 입력을 수집하는 기능 필요

**참조 패턴**:
- UPopup_ReadQuest: InitPopup 패턴, 버튼 바인딩, PopupManager 사용
- UPopup_InputMsg: 버튼 클릭 핸들러, 네트워크 요청 처리

---

## 작업 범위

### 1. UPopup_Interview.cpp 구현 (Source/Onepiece/MessageBox/Private/)
**현재 상태**: 빈 파일 (include만 존재)

**구현 필요 사항**:
```cpp
// 필요한 include 추가
#include "UPopup_InterviewItem.h"
#include "Components/ScrollBox.h"
#include "Components/VerticalBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "UPopupManager.h"
#include "GameLogging.h"

// 구현할 함수들:
1. NativeConstruct()
   - Btn_Close 클릭 이벤트 바인딩 (OnClickClose)
   - Btn_Submit 클릭 이벤트 바인딩 (OnClickSubmit)

2. InitPopup(const FInterviewData& InterviewData)
   - 전달받은 InterviewData의 Question 배열을 순회
   - 각 FInterviewQuestionData마다 InterviewItemClass 인스턴스 생성
   - 생성된 UPopup_InterviewItem에 InitItem 호출
   - VerticalBox에 위젯 추가

3. OnClickClose()
   - UPopupManager::Get(GetWorld())->HideCurrentPopup() 호출

4. OnClickSubmit()
   - VerticalBox의 모든 자식 위젯을 순회
   - 각 UPopup_InterviewItem의 GetAnswer() 호출하여 답변 수집
   - 답변 데이터를 로그 출력 (또는 네트워크 전송 준비)
   - UPopupManager로 팝업 닫기
```

---

### 2. UPopup_InterviewItem.cpp 구현 (Source/Onepiece/MessageBox/Private/)
**현재 상태**: 빈 파일 (include만 존재)

**구현 필요 사항**:
```cpp
// 필요한 include 추가
#include "NetworkData.h"
#include "Components/TextBlock.h"
#include "Components/MultiLineEditableText.h"
#include "GameLogging.h"

// 구현할 함수들:
1. NativeConstruct()
   - 필요 시 초기화 로직 (현재는 비워둘 수 있음)

2. InitItem(const FInterviewQuestionData& Data)
   - Txt_Index: FString::Printf(TEXT("Question.%02d"), Data.Id) 형식으로 표시
   - Text_Question: Data.Eng 표시
   - Edit_Answer: 플레이어 입력란 (초기값 빈 문자열)

3. GetAnswer()
   - Edit_Answer->GetText().ToString() 반환
```

---

### 3. APopupTesterActor 테스트 코드 추가

**파일**:
- Source/Onepiece/Network/Public/APopupTesterActor.h
- Source/Onepiece/Network/Private/APopupTesterActor.cpp

**추가할 내용**:

**Header (.h)**:
```cpp
// 함수 선언 추가
UFUNCTION(BlueprintCallable, CallInEditor, Category = "TEST|Message")
void InterviewPopup();
```

**Implementation (.cpp)**:
```cpp
void APopupTesterActor::InterviewPopup()
{
    if (const auto PopupMgr = UPopupManager::Get(GetWorld()))
    {
        if (const auto Popup = Cast<UPopup_Interview>(
            PopupMgr->ShowPopup(EPopupType::Interview)))
        {
            // 테스트용 더미 데이터 생성
            FInterviewData TestData;

            // 질문 1
            FInterviewQuestionData Q1;
            Q1.Id = 1;
            Q1.TypeCode = 0;
            Q1.Eng = TEXT("What is your current country of residence?");
            Q1.Kor = TEXT("현재 거주 국가는 어디인가요?");
            Q1.EngKey = TEXT("country");
            Q1.KorKey = TEXT("국가");
            Q1.CreatedAt = TEXT("2025-01-15");
            TestData.Question.Add(Q1);

            // 질문 2
            FInterviewQuestionData Q2;
            Q2.Id = 2;
            Q2.TypeCode = 0;
            Q2.Eng = TEXT("How did you find out about this program?");
            Q2.Kor = TEXT("이 프로그램을 어떻게 알게 되었나요?");
            Q2.EngKey = TEXT("discovery");
            Q2.KorKey = TEXT("발견경로");
            Q2.CreatedAt = TEXT("2025-01-15");
            TestData.Question.Add(Q2);

            // 질문 3
            FInterviewQuestionData Q3;
            Q3.Id = 3;
            Q3.TypeCode = 0;
            Q3.Eng = TEXT("What are your goals for learning Korean?");
            Q3.Kor = TEXT("한국어 학습의 목표는 무엇인가요?");
            Q3.EngKey = TEXT("goals");
            Q3.KorKey = TEXT("목표");
            Q3.CreatedAt = TEXT("2025-01-15");
            TestData.Question.Add(Q3);

            // 팝업 초기화
            Popup->InitPopup(TestData);
        }
    }
}
```

---

## 작업 순서

1. **UPopup_InterviewItem.cpp 구현** (간단한 위젯부터)
   - NativeConstruct, InitItem, GetAnswer 구현

2. **UPopup_Interview.cpp 구현**
   - NativeConstruct, InitPopup 구현
   - OnClickClose, OnClickSubmit 구현

3. **APopupTesterActor 테스트 코드 추가**
   - InterviewPopup 함수 추가
   - 더미 데이터 생성 로직 작성

4. **빌드 및 테스트**
   - 언리얼 에디터에서 빌드
   - APopupTesterActor를 레벨에 배치
   - InterviewPopup 함수 CallInEditor로 실행
   - 팝업 표시 확인, 입력 및 제출 테스트

---

## 체크리스트

**구현 전**:
- [x] NetworkData.h의 FInterviewData, FInterviewQuestionData 구조체 확인
- [x] UPopup_Interview.h, UPopup_InterviewItem.h 헤더 파일 확인
- [x] 참조 팝업 패턴 분석 (UPopup_ReadQuest, UPopup_InputMsg)
- [x] EPopupType에 Interview 타입 이미 정의되어 있는지 확인 (✓ 23줄에 정의됨)

**구현 중**:
- [ ] UPopup_InterviewItem.cpp 구현 완료
- [ ] UPopup_Interview.cpp 구현 완료
- [ ] APopupTesterActor 테스트 코드 추가
- [ ] 빌드 성공 확인

**테스트**:
- [ ] 팝업이 정상적으로 열리는지 확인
- [ ] 질문 항목이 올바르게 표시되는지 확인
- [ ] 답변 입력란이 동작하는지 확인
- [ ] 제출 버튼 클릭 시 답변이 수집되는지 확인
- [ ] 닫기 버튼이 정상 동작하는지 확인

---

## 주의사항

1. **UPopup_Interview.h**의 `InterviewItemClass`는 `EditDefaultsOnly`로 선언되어 있으므로, **블루프린트에서 반드시 할당**해야 합니다.
   - 언리얼 에디터에서 UPopup_Interview 위젯 블루프린트를 열고, InterviewItemClass에 UPopup_InterviewItem 위젯 클래스를 할당하세요.

2. **BindWidget**으로 선언된 위젯들은 블루프린트에서 **정확한 이름으로 생성**되어야 합니다:
   - UPopup_Interview: Popup_BaseBg, Image_TitleFrame, Txt_Title, Btn_Close, Btn_Submit, ScrollBox, VerticalBox
   - UPopup_InterviewItem: Txt_Index, Text_Question, Edit_Answer

3. **로그 출력**은 `GameLogging.h`의 `PRINTLOG` 매크로 사용

4. **답변 제출** 시 실제 서버 전송 로직은 추후 네트워크 시스템과 연동하여 구현
