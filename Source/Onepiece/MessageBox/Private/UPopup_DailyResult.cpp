// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "UPopup_DailyResult.h"

#include "ADailyKiosk.h"
#include "UPopup_DailyResultItem.h"
#include "UPopupManager.h"
#include "UImageButton.h"
#include "UTextureButton.h"
#include "UConfigLibrary.h"
#include "ULingoGameHelper.h"
#include "GameLogging.h"
#include "UDailyKioskWidget.h"

#include "Components/VerticalBox.h"
#include "Components/Spacer.h"
#include "Components/WidgetComponent.h"
#include "EngineUtils.h"

void UPopup_DailyResult::NativeConstruct()
{
	Super::NativeConstruct();

	// 버튼 바인딩
	if (Btn_Confirm)
	{
		Btn_Confirm->OnButtonClickedEvent.RemoveDynamic(this, &UPopup_DailyResult::OnClickConfirm);
		Btn_Confirm->OnButtonClickedEvent.AddDynamic(this, &UPopup_DailyResult::OnClickConfirm);
	}

	if (Btn_Close)
	{
		Btn_Close->OnButtonClickedEvent.RemoveDynamic(this, &UPopup_DailyResult::OnClickConfirm);
		Btn_Close->OnButtonClickedEvent.AddDynamic(this, &UPopup_DailyResult::OnClickConfirm);
	}

}

// ========================================
// Public Methods
// ========================================

void UPopup_DailyResult::InitPopup(const FDailyStudyResult& Result)
{
	// 데이터 저장
	StudyResult = Result;

	// 질문 리스트 초기화
	InitQuestionList();
}

// ========================================
// Button Events
// ========================================

void UPopup_DailyResult::OnClickConfirm()
{
	// ConfigLibrary를 이용하여 점수 저장 (플레이어 인덱스 기준)
	const int32 UserId = ULingoGameHelper::GetUserId(this);
	const int32 CurrentScore = StudyResult.CurrentScore;

	// 기존 최고 점수 가져오기
	const int32 BestScore = UConfigLibrary::GetUserInt(UserId, TEXT("DailyBestScore"), 0);

	// 현재 점수가 최고 점수보다 높으면 저장
	bool bScoreUpdated = false;
	if (CurrentScore > BestScore)
	{
		UConfigLibrary::SetUserInt(UserId, TEXT("DailyBestScore"), CurrentScore, true);
		PRINTLOG(TEXT("[DailyResult] New best score saved: %d (Previous: %d)"), CurrentScore, BestScore);
		bScoreUpdated = true;
	}
	else
	{
		PRINTLOG(TEXT("[DailyResult] Current score: %d (Best: %d)"), CurrentScore, BestScore);
	}

	// 모든 DailyKiosk의 위젯 업데이트 (클라이언트 측에서만 실행)
	// Popup은 클라이언트 UI이므로 이 코드는 각 클라이언트에서 독립적으로 실행됩니다.
	// Screen Space Widget이므로 각 클라이언트가 자신의 화면에만 렌더링합니다.
	if (bScoreUpdated)
	{
		int32 UpdateCount = 0;
		for (TActorIterator<ADailyKiosk> It(GetWorld()); It; ++It)
		{
			ADailyKiosk* Kiosk = *It;
			if (Kiosk && Kiosk->GetWidgetComp())
			{
				if (UDailyKioskWidget* Widget = Cast<UDailyKioskWidget>(Kiosk->GetWidgetComp()->GetWidget()))
				{
					Widget->UpdateBestScore();
					UpdateCount++;
				}
			}
		}

		if (UpdateCount > 0)
		{
			PRINTLOG(TEXT("[DailyResult] Updated %d DailyKiosk widget(s) for User %d"), UpdateCount, UserId);
		}
	}

	// 팝업 닫기
	if (auto PopupMgr = UPopupManager::Get(GetWorld()))
	{
		PopupMgr->HideCurrentPopup();
	}
}

// ========================================
// Question List Initialization
// ========================================

void UPopup_DailyResult::InitQuestionList()
{
	if (!VerticalBox)
		return;

	// 기존 아이템 제거
	VerticalBox->ClearChildren();

	// QuestionList와 AnswerList를 페어로 사용하여 아이템 생성
	for (int32 i = 0; i < StudyResult.QuestionList.Num(); ++i)
	{
		// AnswerList 범위 체크
		if (!StudyResult.AnswerList.IsValidIndex(i))
			continue;

		// 아이템 위젯 생성 및 초기화
		if (auto ItemWidget = CreateWidget<UPopup_DailyResultItem>(GetWorld(), AnswerItemClass))
		{
			// 간단하게 3개 파라미터만 전달 (FWordData에서 한글 단어 추출)
			ItemWidget->InitData(
				i+1,
				StudyResult.QuestionList[i].Kor,                      // Question (한글)
				StudyResult.AnswerList[i].final_feedback,             // FeedBack
				StudyResult.AnswerList[i].final_overall_score         // Score
			);
			
			VerticalBox->AddChildToVerticalBox(ItemWidget);

			// 마지막 아이템이 아니면 Spacer 추가 (아이템 간 간격)
			if (i < StudyResult.QuestionList.Num() - 1)
			{
				USpacer* Spacer = NewObject<USpacer>(this);
				if (Spacer)
				{
					Spacer->SetSize(FVector2D(1.0f, 15.0f));
					VerticalBox->AddChildToVerticalBox(Spacer);
				}
			}
		}
	}
}