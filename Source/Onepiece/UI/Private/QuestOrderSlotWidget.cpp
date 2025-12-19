// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "QuestOrderSlotWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"

void UQuestOrderSlotWidget::SetQuestType(FString inQuestType)
{
	Txt_QuestType->SetText(FText::FromString(inQuestType));
}

void UQuestOrderSlotWidget::PlayQuestSlot()
{
	Image_Bg->SetColorAndOpacity(FColor::FromHex("00FF94FF"));
}

void UQuestOrderSlotWidget::FinishQuestSlot()
{
	Image_Bg->SetColorAndOpacity(FColor::FromHex("FFCA50FF"));
	WidgetSwitcher_Slot->SetActiveWidgetIndex(1);
}
