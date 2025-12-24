// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "UHistoryItem.h"
#include "Components/TextBlock.h"
#include "Onepiece/Onepiece.h"

void UHistoryItem::NativeConstruct()
{
	Super::NativeConstruct();

	Txt_Question->SetLineHeightPercentage( DefineData::LineHeightPercentage );
	Txt_Answer->SetLineHeightPercentage( DefineData::LineHeightPercentage );
}

void UHistoryItem::InitItem(const FChatHistoryItem& Data)
{
	HistoryData = Data;

	Txt_Question->SetText(FText::FromString(Data.Question));
	Txt_Answer->SetText(FText::FromString(Data.Answer));
	Txt_Timestamp->SetText(FText::FromString(Data.Timestamp));
}
