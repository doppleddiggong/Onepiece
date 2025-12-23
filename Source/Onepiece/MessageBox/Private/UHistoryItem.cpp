// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "UHistoryItem.h"
#include "Components/TextBlock.h"
#include "GameLogging.h"

void UHistoryItem::NativeConstruct()
{
	Super::NativeConstruct();
}

void UHistoryItem::InitItem(const FChatHistoryItem& Data)
{
	HistoryData = Data;

	if (Txt_Index)
	{
		Txt_Index->SetText(FText::FromString(FString::Printf(TEXT("#%d"), Data.Index + 1)));
	}

	if (Txt_Question)
	{
		Txt_Question->SetText(FText::FromString(Data.Question));
	}

	if (Txt_Answer)
	{
		Txt_Answer->SetText(FText::FromString(Data.Answer));
	}

	if (Txt_Timestamp)
	{
		Txt_Timestamp->SetText(FText::FromString(Data.Timestamp));
	}

	PRINTLOG(TEXT("[HistoryItem] Initialized Index=%d, Q=%s"), Data.Index, *Data.Question);
}
