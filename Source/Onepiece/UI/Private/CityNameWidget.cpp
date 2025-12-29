// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "CityNameWidget.h"

#include "Components/TextBlock.h"

void UCityNameWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UCityNameWidget::SetCityName(FString InCityName)
{
	Txt_CityName->SetText(FText::FromString(InCityName));
}

void UCityNameWidget::SetTextColor(FLinearColor InColor)
{
	Txt_CityName->SetColorAndOpacity(InColor);
}
