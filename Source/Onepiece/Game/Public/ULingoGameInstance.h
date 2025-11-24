// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ULingoGameInstance.generated.h"

UCLASS()
class ONEPIECE_API ULingoGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	ULingoGameInstance();

	virtual void Init() override;
};
