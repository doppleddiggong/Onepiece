// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "UPopup_DailyResult.h"

#include "UPopup_DailyStudy.h"
#include "UPopupManager.h"
#include "UTextureButton.h"

#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"
#include "Components/VerticalBox.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/Border.h"

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

void UPopup_DailyResult::InitPopup(const FDailyStudyResult& InResult)
{
	// 결과 집계
	DailyResult = InResult;

	// 점수 표시
	Txt_FinalScore->SetText(FText::FromString(
		FString::Printf(TEXT("Score: %d"), DailyResult.CurrentScore)));
	
	Txt_BestScore->SetText(FText::FromString(
		FString::Printf(TEXT("Best: %d"), DailyResult.BestScore)));
	
	Txt_CompletedCount->SetText(FText::FromString(
		FString::Printf(TEXT("Completed: %d/%d"), DailyResult.CompletedCount, DailyResult.TotalCount)));

	// 안내 메시지 설정
	if (Txt_GuideMessage)
	{
		Txt_GuideMessage->SetText(FText::FromString(
			TEXT("이번 게임에서 배웠던 단어는 다음과 같습니다.\n각각의 단어별로 획득한 점수와 평가는 다음과 같습니다.\n모르는 것은 AI와 대화하면서 차근차근 학습해봅시다.")));
	}

	// 단어 목록 초기화
	if (ScrollBox_WordList)
	{
		ScrollBox_WordList->ClearChildren();

		// 각 단어별 항목 생성
		for (int32 i = 0; i < DailyResult.QuestionList.Num(); ++i)
		{
			if (DailyResult.AnswerList.IsValidIndex(i))
			{
				CreateWordItemWidget(DailyResult.QuestionList[i], DailyResult.AnswerList[i], i);
			}
		}
	}
}

// ========================================
// Button Events
// ========================================

void UPopup_DailyResult::OnClickConfirm()
{
	if ( auto PopupMgr = UPopupManager::Get(GetWorld()) )
	{
		PopupMgr->HideCurrentPopup();
	}
}

// ========================================
// Word Item Widget Creation
// ========================================

void UPopup_DailyResult::CreateWordItemWidget(const FDailyStudyWordItem& WordItem, const FDailyStudyAnswer& Answer, int32 Index)
{
	if (!ScrollBox_WordList)
		return;

	// HorizontalBox 생성 (단어 항목 컨테이너)
	UHorizontalBox* ItemBox = NewObject<UHorizontalBox>(this);
	if (!ItemBox)
		return;

	// 번호 텍스트 (예: "1.")
	UTextBlock* Txt_Index = NewObject<UTextBlock>(this);
	if (Txt_Index)
	{
		Txt_Index->SetText(FText::FromString(FString::Printf(TEXT("%d."), Index + 1)));
		Txt_Index->SetMinDesiredWidth(40.0f);
		ItemBox->AddChild(Txt_Index);
	}

	// 단어 정보 VerticalBox
	UVerticalBox* WordInfoBox = NewObject<UVerticalBox>(this);
	if (WordInfoBox)
	{
		// 한국어/영어 단어
		UTextBlock* Txt_Word = NewObject<UTextBlock>(this);
		if (Txt_Word)
		{
			FString WordText = FString::Printf(TEXT("%s (%s)"), *WordItem.Kor, *WordItem.Eng);
			Txt_Word->SetText(FText::FromString(WordText));
			WordInfoBox->AddChild(Txt_Word);
		}

		// 점수 및 평가
		UTextBlock* Txt_Score = NewObject<UTextBlock>(this);
		if (Txt_Score)
		{
			FString ScoreText;
			if (Answer.bSkipped)
			{
				ScoreText = TEXT("건너뜀 (점수: 0)");
			}
			else if (Answer.bCompleted)
			{
				ScoreText = FString::Printf(TEXT("점수: %d | 문법: %d | 맥락: %d"),
					Answer.JudgeResult.final_overall_score,
					Answer.JudgeResult.grammar_score,
					Answer.JudgeResult.context_score);
			}
			else
			{
				ScoreText = TEXT("미완료");
			}
			Txt_Score->SetText(FText::FromString(ScoreText));
			WordInfoBox->AddChild(Txt_Score);
		}

		ItemBox->AddChild(WordInfoBox);
	}

	// ScrollBox에 추가
	ScrollBox_WordList->AddChild(ItemBox);
}