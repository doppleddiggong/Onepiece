// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "UPopup_EvaluationScenarioResultItem.h"
#include "Components/TextBlock.h"

void UPopup_EvaluationScenarioResultItem::InitPanel(const FScenarioResult& InScenarioResult)
{
	if (Txt_DisplayName)
	{
		Txt_DisplayName->SetText(FText::FromString(InScenarioResult.display_name));
	}

	if (Txt_FinalScore)
	{
		Txt_FinalScore->SetText(FText::AsNumber(InScenarioResult.final_score));
	}

	if (Txt_Grade)
	{
		Txt_Grade->SetText(FText::FromString(InScenarioResult.grade));
	}

	if (Txt_FeedbackTitle)
	{
		Txt_FeedbackTitle->SetText(FText::FromString(InScenarioResult.feedback_summary.title));
	}

	if (Txt_FeedbackMessage)
	{
		Txt_FeedbackMessage->SetText(FText::FromString(InScenarioResult.feedback_summary.message));
	}

	if (Txt_ActionItem)
	{
		Txt_ActionItem->SetText(FText::FromString(InScenarioResult.action_item));
	}
}
