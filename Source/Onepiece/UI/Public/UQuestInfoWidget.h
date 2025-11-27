// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UQuestInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class ONEPIECE_API UQuestInfoWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	
public:
	UFUNCTION()
	void InitQuestInfo(EQuestRole QuestRole);
	
public:
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Style")
	class UTextBlock* Txt_Message = nullptr;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Style")
	class UImage* Image_Synbol = nullptr;
};