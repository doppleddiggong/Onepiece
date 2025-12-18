// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "EQuestRole.h"
#include "Delegates/Delegate.h"
#include "Blueprint/UserWidget.h"
#include "UQuestInfoWidget.generated.h"

UCLASS()
class ONEPIECE_API UQuestInfoWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateQuestText(FString InTitle, FString InDescription);
	
protected:
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Style")
	TObjectPtr<class UTextBlock> Txt_Title;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Style")
	TObjectPtr<class UTextBlock> Txt_Message;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Style")
	TObjectPtr<class UImage> Image_Symbol;
};