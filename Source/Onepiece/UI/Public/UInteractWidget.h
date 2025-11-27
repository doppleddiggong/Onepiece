// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UInteractWidget.generated.h"

/**
 * 
 */
UCLASS()
class ONEPIECE_API UInteractWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitInfo(const FString InKey, const FString InDesc);
	
protected:
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UTextBlock> Txt_Key;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UTextBlock> Txt_Desc; 
};
