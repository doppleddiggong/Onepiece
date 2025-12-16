// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "UPopup_EvaluationTotalResultItem.h"
#include "Components/TextBlock.h"

void UPopup_EvaluationTotalResultItem::InitPanel(const FTotalResult& InTotalResult)
{
	if (Txt_FinalScore)
	{
		Txt_FinalScore->SetText(FText::AsNumber(InTotalResult.final_score));
	}

	if (Txt_Grade)
	{
		Txt_Grade->SetText(FText::FromString(InTotalResult.grade));
	}

	if (Txt_FeedbackSummary)
	{
		Txt_FeedbackSummary->SetText(FText::FromString(InTotalResult.feedback_summary));
	}
}
