// Copyright (c) 2025 Doppleddiggong. All rights reserved.
// Unauthorized copying, modification, or distribution of this file,
// via any medium is strictly prohibited. Proprietary and confidential.

#include "UPopup_InterviewHello.h"

#include "APlayerControl.h"
#include "UPopupManager.h"
#include "UKLingoNetworkSystem.h"
#include "ULingoGameHelper.h"
#include "UDialogManager.h"
#include "UBroadcastManager.h"
#include "GameLogging.h"
#include "UImageButton.h"
#include "UTextureButton.h"
#include "Components/TextBlock.h"
#include "Components/EditableText.h"
#include "Components/ProgressBar.h"
#include "Components/CheckBox.h"
#include "UConfigLibrary.h"

void UPopup_InterviewHello::NativeConstruct()
{
	Super::NativeConstruct();

	// 버튼 이벤트 바인딩
	if (Button_PrevArrow)
	{
		Button_PrevArrow->OnButtonClickedEvent.RemoveDynamic(this, &UPopup_InterviewHello::OnClickPrevArrow);
		Button_PrevArrow->OnButtonClickedEvent.AddDynamic(this, &UPopup_InterviewHello::OnClickPrevArrow);
	}

	if (Button_NextArrow)
	{
		Button_NextArrow->OnButtonClickedEvent.RemoveDynamic(this, &UPopup_InterviewHello::OnClickNextArrow);
		Button_NextArrow->OnButtonClickedEvent.AddDynamic(this, &UPopup_InterviewHello::OnClickNextArrow);
	}

	if (Btn_Next)
	{
		Btn_Next->OnButtonClickedEvent.RemoveDynamic(this, &UPopup_InterviewHello::OnClickNext);
		Btn_Next->OnButtonClickedEvent.AddDynamic(this, &UPopup_InterviewHello::OnClickNext);
	}

	if (Btn_Submit)
	{
		Btn_Submit->OnButtonClickedEvent.RemoveDynamic(this, &UPopup_InterviewHello::OnClickSubmit);
		Btn_Submit->OnButtonClickedEvent.AddDynamic(this, &UPopup_InterviewHello::OnClickSubmit);
	}

	if (Btn_Close)
	{
		Btn_Close->OnButtonClickedEvent.RemoveDynamic(this, &UPopup_InterviewHello::OnClickClose);
		Btn_Close->OnButtonClickedEvent.AddDynamic(this, &UPopup_InterviewHello::OnClickClose);
	}

	if (Button_CheckToday)
	{
		Button_CheckToday->OnCheckStateChanged.RemoveDynamic(this, &UPopup_InterviewHello::OnCheckToday);
		Button_CheckToday->OnCheckStateChanged.AddDynamic(this, &UPopup_InterviewHello::OnCheckToday);
	}

	// 답변 입력란 텍스트 변경 이벤트 바인딩
	if (Edit_Answer)
	{
		Edit_Answer->OnTextChanged.RemoveDynamic(this, &UPopup_InterviewHello::OnAnswerTextChanged);
		Edit_Answer->OnTextChanged.AddDynamic(this, &UPopup_InterviewHello::OnAnswerTextChanged);
	}
}

void UPopup_InterviewHello::InitPopup(const FResponseInterviewHello& InterviewData)
{
	// 질문 데이터 저장
	SavedQuestions = InterviewData.Questions;

	// 답변 임시 저장 배열 초기화
	TempAnswers.SetNum(SavedQuestions.Num());
	for (int32 i = 0; i < TempAnswers.Num(); ++i)
	{
		TempAnswers[i] = TEXT("");
	}

	// 첫 번째 질문으로 초기화
	CurQuestionIndex = 0;

	// UI 업데이트
	RefreshUI();
	RefreshArrowButton();
	RefreshSubmitButtonState();
	RefreshProgressBar();
}

void UPopup_InterviewHello::RefreshUI()
{
	// 유효성 체크
	if (!SavedQuestions.IsValidIndex(CurQuestionIndex))
	{
		PRINTLOG(TEXT("[UPopup_InterviewHello] RefreshUI - Invalid CurrentQuestionIndex: %d"), CurQuestionIndex);
		return;
	}

	const FInterviewQuestionData& CurrentQuestion = SavedQuestions[CurQuestionIndex];

	// 질문 텍스트 업데이트
	if (TXt_Question)
	{
		TXt_Question->SetText(FText::FromString(CurrentQuestion.Eng));
	}

	// 현재 질문의 답변 불러오기
	LoadCurrentAnswer();
}

void UPopup_InterviewHello::RefreshArrowButton()
{
	// Prev 버튼: 첫 번째 질문이 아닐 때만 표시
		Button_PrevArrow->SetVisibility( CurQuestionIndex > 0 ? ESlateVisibility::Visible : ESlateVisibility::Hidden );

	// Next 버튼: 마지막 질문이 아닐 때만 표시
	Button_NextArrow->SetVisibility( CurQuestionIndex < SavedQuestions.Num() - 1 ? ESlateVisibility::Visible : ESlateVisibility::Hidden	);
}

void UPopup_InterviewHello::RefreshSubmitButtonState()
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
	bool bIsLastQuestion = (CurQuestionIndex == SavedQuestions.Num() - 1);

	// 버튼 표시/숨김 및 활성화 상태 전환
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

void UPopup_InterviewHello::RefreshProgressBar()
{
	if (ProgressBar_Question && SavedQuestions.Num() > 0)
	{
		float Progress = static_cast<float>(CurQuestionIndex + 1) / static_cast<float>(SavedQuestions.Num());
		ProgressBar_Question->SetPercent(Progress);
	}
}

void UPopup_InterviewHello::SaveCurrentAnswer()
{
	if (!Edit_Answer || !TempAnswers.IsValidIndex(CurQuestionIndex))
		return;

	TempAnswers[CurQuestionIndex] = Edit_Answer->GetText().ToString();
}

void UPopup_InterviewHello::LoadCurrentAnswer()
{
	if (!Edit_Answer || !TempAnswers.IsValidIndex(CurQuestionIndex))
		return;

	Edit_Answer->SetText(FText::FromString(TempAnswers[CurQuestionIndex]));
}

void UPopup_InterviewHello::OnClickPrevArrow()
{
	// 현재 답변 저장
	SaveCurrentAnswer();

	// 인덱스 감소
	if (CurQuestionIndex > 0)
	{
		CurQuestionIndex--;

		// UI 업데이트
		RefreshUI();
		RefreshArrowButton();
		RefreshSubmitButtonState();
		RefreshProgressBar();
	}
}

void UPopup_InterviewHello::OnClickNextArrow()
{
	// 현재 답변 저장
	SaveCurrentAnswer();

	// 인덱스 증가
	if (CurQuestionIndex < SavedQuestions.Num() - 1)
	{
		CurQuestionIndex++;

		// UI 업데이트
		RefreshUI();
		RefreshArrowButton();
		RefreshSubmitButtonState();
		RefreshProgressBar();
	}
}

void UPopup_InterviewHello::OnClickNext()
{
	OnClickNextArrow();
}

void UPopup_InterviewHello::OnClickSubmit()
{
	// 마지막 답변 저장
	SaveCurrentAnswer();

	// 모든 답변 검증
	for (int32 i = 0; i < TempAnswers.Num(); ++i)
	{
		if (TempAnswers[i].TrimStartAndEnd().IsEmpty())
		{
			// 빈 답변이 있으면 Toast 메시지 표시
			if (UDialogManager* DialogMgr = UDialogManager::Get(GetWorld()))
			{
				FString Message = FString::Printf( TEXT("Question %d is not answered. Please fill in all answers."), i + 1 );
				DialogMgr->ShowToast(Message);
			}
			return;
		}
	}

	// 답변 데이터 생성
	TArray<FInterviewAnswerData> AnswerDataList;
	const int32 UserId = ULingoGameHelper::GetUserId(GetWorld());

	for (int32 i = 0; i < SavedQuestions.Num(); ++i)
	{
		FInterviewAnswerData AnswerData;
		AnswerData.interview_id = SavedQuestions[i].Id;
		AnswerData.answer = TempAnswers[i];
		AnswerData.user_id = UserId;
		AnswerDataList.Add(AnswerData);
	}

	// 네트워크 전송
	if (UKLingoNetworkSystem* NetworkSystem = UKLingoNetworkSystem::Get(GetWorld()))
	{
		FRequestInterviewAnswer Request;
		Request.answer = AnswerDataList;

		NetworkSystem->RequestInterviewAnswer(
			Request,
			FResponseInterviewAnswerDelegate::CreateUObject(
				this, &UPopup_InterviewHello::OnResponseInterviewAnswer
			)
		);
	}
}

void UPopup_InterviewHello::OnClickClose()
{
	// PopupManager를 통해 팝업 닫기
	if (UPopupManager* PopupMgr = UPopupManager::Get(GetWorld()))
	{
		PopupMgr->HideCurrentPopup();

		if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
		{
			if (APlayerControl* PlayerControl = Cast<APlayerControl>(PC))
			{
				if (!PlayerControl->ShouldSkipTutorial())
				{
					// 튜토리얼 여부 화면 띄우기
					PopupMgr->ShowPopup(EPopupType::AskTutorial);
				}
			}
		}
	}
}

void UPopup_InterviewHello::OnCheckToday(bool bIsChecked)
{
	// 체크박스 상태만 저장 (실제 Config 저장은 Submit 성공 시)
	bCheckTodayDoNotShow = bIsChecked;
}

void UPopup_InterviewHello::OnAnswerTextChanged(const FText& Text)
{
	// 현재 질문의 답변을 실시간으로 TempAnswers에 저장
	if (TempAnswers.IsValidIndex(CurQuestionIndex))
	{
		TempAnswers[CurQuestionIndex] = Text.ToString();
	}

	// Submit 버튼 상태 업데이트 (마지막 질문에서만 영향)
	RefreshSubmitButtonState();
}

void UPopup_InterviewHello::OnResponseInterviewAnswer(FResponseInterviewAnswer& ResponseData, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		PRINTLOG(TEXT("[UPopup_InterviewHello] Interview Answer SUCCESS"));

		// "Today do not show" 체크되어 있으면 오늘 날짜 저장
		if (bCheckTodayDoNotShow)
		{
			const int32 UserId = ULingoGameHelper::GetUserId(GetWorld());

			// 현재 날짜를 "YYYY-MM-DD" 형식으로 저장
			const FDateTime Now = FDateTime::Now();
			const FString TodayDate = FString::Printf(TEXT("%04d-%02d-%02d"), Now.GetYear(), Now.GetMonth(), Now.GetDay());

			UConfigLibrary::SetUserString(UserId, TEXT("InterviewSkipDate"), TodayDate);

			if (auto DM = UDialogManager::Get(GetWorld()))
			{
				DM->ShowToast(TEXT("Do not show again setting complete"));
			}

			PRINTLOG(TEXT("[UPopup_InterviewHello] 'Today do not show' saved for User %d, Date: %s"), UserId, *TodayDate);
		}

		// 성공 시 튜터 메시지 표시
		if (auto BM = UBroadcastManager::Get(GetWorld()))
		{
			BM->SendTutorMessage( FText::FromString(TEXT("Interview answers submitted successfully!")) );
		}

		// 팝업 닫기
		if (UPopupManager* PopupMgr = UPopupManager::Get(GetWorld()))
		{
			PopupMgr->HideCurrentPopup();

			// 튜토리얼 완료 여부 확인 후 조건부로 AskTutorial 표시
			if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
			{
				if (APlayerControl* PlayerControl = Cast<APlayerControl>(PC))
				{
					if (!PlayerControl->ShouldSkipTutorial())
					{
						// 튜토리얼을 아직 안 했으면 AskTutorial 팝업 표시
						PopupMgr->ShowPopup(EPopupType::AskTutorial);
					}
				}
			}
		}
	}
	else
	{
		PRINTLOG(TEXT("[UPopup_InterviewHello] Interview Answer FAILED"));
	}
}

bool UPopup_InterviewHello::ShouldSkipInterviewToday(const UObject* WorldContextObject)
{
	const int32 UserId = ULingoGameHelper::GetUserId(WorldContextObject);

	// 저장된 날짜 읽기
	const FString SavedDate = UConfigLibrary::GetUserString(UserId, TEXT("InterviewSkipDate"), TEXT(""));
	if (SavedDate.IsEmpty())
	{
		// 저장된 날짜 없음 → 보여줌
		return false;
	}

	// 오늘 날짜 생성
	const FDateTime Now = FDateTime::Now();
	const FString TodayDate = FString::Printf(TEXT("%04d-%02d-%02d"), Now.GetYear(), Now.GetMonth(), Now.GetDay());

	// 날짜 비교: 저장된 날짜 == 오늘 날짜 → Skip
	if (SavedDate == TodayDate)
	{
		PRINTLOG(TEXT("[UPopup_InterviewHello] Skipping Interview today for User %d (Saved Date: %s)"), UserId, *SavedDate);
		return true;  // 오늘은 건너뜀
	}

	// 날짜가 다르면 → 보여줌
	return false;
}