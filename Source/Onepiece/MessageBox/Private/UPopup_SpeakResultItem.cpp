// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "UPopup_SpeakResultItem.h"

#include "ULingoGameHelper.h"
#include "UGameDataManager.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Onepiece/Onepiece.h"

void UPopup_SpeakResultItem::InitData(const FSpeakResultItem& InItemData)
{
	Txt_Index->SetText(FText::FromString(FString::Printf(TEXT("Q%d"), InItemData.Index)));
	Txt_Question->SetText(FText::FromString(InItemData.Question));
	Txt_Feedback->SetText(FText::FromString(InItemData.Feedback));

	Txt_Feedback->SetLineHeightPercentage( DefineData::LineHeightPercentage );

	
	auto GradeTexture = ULingoGameHelper::ConvertGradeScore(InItemData.Score);
	if (UTexture2D* Texture = UGameDataManager::Get(this)->GetTexture(GradeTexture))
	{
		Image_Grade->SetBrushFromTexture(Texture);
	}
}
