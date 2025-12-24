// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "QuestOrderWidget.h"

#include "GameLogging.h"
#include "QuestOrderSlotWidget.h"
#include "Components/ProgressBar.h"

void UQuestOrderWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	ProgressBar_Quest->SetPercent(1.f/4);
	Slot_R->PlayQuestSlot();
	Slot_R->SetQuestType('R');
	Slot_L->SetQuestType('L');
	Slot_S->SetQuestType('S');
	Slot_W->SetQuestType('W');
}

void UQuestOrderWidget::UpdateQuestOrder(const FString& inQuestOrder)
{
	// 현재 상황
	PRINTLOG(TEXT("%s"), *inQuestOrder);
	
	TArray<FString> orders;
	inQuestOrder.ParseIntoArray(orders, TEXT(" "));
	
	// Slot 갱신
	if (orders[CurrentSlotIndex] == "V")
	{
		switch(CurrentSlotIndex)
		{
		case 0:
			Slot_R->FinishQuestSlot();
			Slot_L->PlayQuestSlot();
			break;
		case 1:
			Slot_L->FinishQuestSlot();
			Slot_S->PlayQuestSlot();
			break;
		case 2:
			Slot_S->FinishQuestSlot();
			Slot_W->PlayQuestSlot();
			break;
		case 3:
			Slot_W->FinishQuestSlot();
			break;
		default:
			break;
		}
		
		CurrentSlotIndex++;
		ProgressBar_Quest->SetPercent((float)(CurrentSlotIndex + 1) / orders.Num());
	}
}
