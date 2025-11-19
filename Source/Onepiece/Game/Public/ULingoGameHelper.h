// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ULingoGameHelper.generated.h"

/**
 * 
 */
UCLASS()
class ONEPIECE_API ULingoGameHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Utils")
	static ALingoPlayerState* GetLingoPlayerState(const UObject* WorldContextObject);
};
