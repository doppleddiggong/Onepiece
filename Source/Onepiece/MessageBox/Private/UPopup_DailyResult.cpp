// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "UPopup_DailyResult.h"
#include "UPopup_DailyResultItem.h"
#include "UPopupManager.h"
#include "UImageButton.h"

#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/Spacer.h"

void UPopup_DailyResult::NativeConstruct()
{
	Super::NativeConstruct();

	// 버튼 바인딩
	if (Btn_Confirm)
	{
		Btn_Confirm->OnButtonClickedEvent.RemoveDynamic(this, &UPopup_DailyResult::OnClickConfirm);
		Btn_Confirm->OnButtonClickedEvent.AddDynamic(this, &UPopup_DailyResult::OnClickConfirm);
	}
}

// ========================================
// Public Methods
// ========================================

void UPopup_DailyResult::InitPopup(const FDailyStudyResult& Result)
{
	// 데이터 저장
	StudyResult = Result;

	// 점수 표시
	if (Txt_FinalScore)
	{
		Txt_FinalScore->SetText(FText::FromString(
			FString::Printf(TEXT("Score: %d"), Result.CurrentScore)));
	}

	// 질문 리스트 초기화
	InitQuestionList();
}

// ========================================
// Button Events
// ========================================

void UPopup_DailyResult::OnClickConfirm()
{
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