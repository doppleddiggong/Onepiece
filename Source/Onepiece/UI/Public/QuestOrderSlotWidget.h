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
	UQuestOrderSlotWidget(FObjectInitializer const& ObjectInitializer);
	
	void SetQuestType(const char inQuestType);
	void PlayQuestSlot();
	void FinishQuestSlot();

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UImage> Image_Bg;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UWidgetSwitcher> WidgetSwitcher_Slot;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UImage> Image_QuestType;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UImage> Image_Complete;
	
private:
	UPROPERTY()
	TObjectPtr<class UTexture2D> ReadQuestTexture;
	
	UPROPERTY()
	TObjectPtr<class UTexture2D> ListenQuestTexture;
	
	UPROPERTY()
	TObjectPtr<class UTexture2D> SpeakQuestTexture;
	
	UPROPERTY()
	TObjectPtr<class UTexture2D> WriteQuestTexture;
	
	float StampStartScale = 2.f;
	float StampDX = 0;
	FTimerHandle StampTimerHandle;
	
	void PlayStampAnimation(float InDeltaTime);
};
