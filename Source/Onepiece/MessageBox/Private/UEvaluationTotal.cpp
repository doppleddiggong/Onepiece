// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "UEvaluationTotal.h"

#include "UGameDataManager.h"
#include "ULingoGameHelper.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UEvaluationTotal::InitPanel(const FTotalResult& InTotalResult)
{
	Txt_Desc->SetText(FText::FromString(InTotalResult.feedback_summary));

	auto GradeTexture = ULingoGameHelper::ConvertGradeString(InTotalResult.grade);
	if (UTexture2D* Texture = UGameDataManager::Get(this)->GetTexture(GradeTexture))
	{
		Image_Grade->SetBrushFromTexture(Texture);
	}
}
