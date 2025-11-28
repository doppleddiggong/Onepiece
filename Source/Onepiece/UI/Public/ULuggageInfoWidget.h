// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "UInteractWidget.h"
#include "ULuggageInfoWidget.generated.h"

UCLASS()
class ONEPIECE_API ULuggageInfoWidget : public UInteractWidget
{
	GENERATED_BODY()

public:
	void InitLuggage(const FString& Type1, const FString& Type2);

protected:
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UTextBlock> Txt_Type1; 

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UTextBlock> Txt_Type2;
};
