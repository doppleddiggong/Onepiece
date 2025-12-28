// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "UPopup_DailyResultItem.h"
#include "Components/TextBlock.h"
#include "Onepiece/Onepiece.h"

void UPopup_DailyResultItem::InitData(const FString& Question, const FString& FeedBack, const int32 Score)
{
	// 질문 설정 (한국어 단어)
	if (Txt_Question)
	{
		Txt_Question->SetText(FText::FromString(Question));
	}

	// 피드백 설정
	if (Txt_Feedback)
	{
		if (!FeedBack.IsEmpty())
		{
			Txt_Feedback->SetText(FText::FromString(FeedBack));
		}
		else
		{
			Txt_Feedback->SetText(FText::FromString(TEXT("-")));
		}

		Txt_Feedback->SetLineHeightPercentage(DefineData::LineHeightPercentage);
	}

	// 점수 설정
	if (Txt_Score)
	{
		Txt_Score->SetText(FText::FromString(FString::Printf(TEXT("%d"), Score)));
	}
}
