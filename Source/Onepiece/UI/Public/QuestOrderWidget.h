// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuestOrderWidget.generated.h"

/**
 * 
 */
UCLASS()
class ONEPIECE_API UQuestOrderWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UProgressBar> ProgressBar_Quest;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UQuestOrderSlotWidget> Slot_R;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UQuestOrderSlotWidget> Slot_L;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UQuestOrderSlotWidget> Slot_S;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UQuestOrderSlotWidget> Slot_W;
};
