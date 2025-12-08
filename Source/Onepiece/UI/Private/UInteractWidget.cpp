// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "UInteractWidget.h"

#include "Components/TextBlock.h"

void UInteractWidget::InitInfo(const FString InKey, const FString InDesc)
{
	Txt_Key->SetText(FText::FromString(InKey));
	Txt_Desc->SetText(FText::FromString(InDesc));
}

void UInteractWidget::UpdateDesc(const FString& NewDesc)
{
	if (Txt_Desc)
		Txt_Desc->SetText(FText::FromString(NewDesc));
}
