// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "ULuggageInfoWidget.h"

#include "UGameDataManager.h"
#include "Components/TextBlock.h"

void ULuggageInfoWidget::InitLuggage(const FString& Type1, const FString& Type2)
{
	UpdateType1Data(Type1);
	UpdateType2Data(Type2);
}

void ULuggageInfoWidget::UpdateType1Data(const FString& TypeData)
{
	Txt_Type1->SetText(FText::FromString(TypeData));
}

void ULuggageInfoWidget::UpdateType2Data(const FString& TypeData)
{
	Txt_Type2->SetText(FText::FromString(TypeData));
}