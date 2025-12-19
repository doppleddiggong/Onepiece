// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuestOrderSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class ONEPIECE_API UQuestOrderSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetQuestType(FString inQuestType);
	void PlayQuestSlot();
	void FinishQuestSlot();
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UImage> Image_Bg;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UWidgetSwitcher> WidgetSwitcher_Slot;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> Txt_QuestType;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UImage> Image_Complete;	
};
