// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "UEvaluationScenario.h"

#include "UGameDataManager.h"
#include "ULingoGameHelper.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UEvaluationScenario::InitPanel(const FScenarioResult& InScenarioResult)
{
	auto ScenarioTexture = ULingoGameHelper::ConvertScenarioTexture(InScenarioResult.scenario_type);
	if (UTexture2D* Texture = UGameDataManager::Get(this)->GetTexture(ScenarioTexture))
	{
		Image_Symbol->SetBrushFromTexture(Texture);
	}

	Txt_Title->SetText(FText::FromString(InScenarioResult.feedback_summary.title));

	FString Description = FString::Printf(TEXT("%s\n\n[NextAction]\n%s"),
		*InScenarioResult.feedback_summary.message,
		*InScenarioResult.action_item);

	Txt_Desc->SetText(FText::FromString(Description));
	
	auto GradeTexture = ULingoGameHelper::ConvertGradeString(InScenarioResult.grade);
	if (UTexture2D* Texture = UGameDataManager::Get(this)->GetTexture(GradeTexture))
	{
		Image_Grade->SetBrushFromTexture(Texture);
	}
}
