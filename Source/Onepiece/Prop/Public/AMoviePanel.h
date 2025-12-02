// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AMoviePanel.generated.h"

UCLASS()
class ONEPIECE_API AMoviePanel : public AActor
{
	GENERATED_BODY()

public:
	AMoviePanel();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
