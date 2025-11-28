// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "ULuggageInfoWidget.h"

#include "Components/TextBlock.h"


void ULuggageInfoWidget::InitLuggage(const FString& Type1, const FString& Type2)
{
	Txt_Type1->SetText(FText::FromString(Type1));
	Txt_Type2->SetText(FText::FromString(Type2));
}
