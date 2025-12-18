// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "UQuestInfoWidget.h"
#include "Components/TextBlock.h"

void UQuestInfoWidget::UpdateQuestText(FString InTitle, FString InMessage);
{
	Txt_Title->SetText(FText::FromString(InTitle));
	Txt_Message->SetText(FText::FromString(InMessage));
}