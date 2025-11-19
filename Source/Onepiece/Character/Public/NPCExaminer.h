// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "ANPCBase.h"
#include "NPCExaminer.generated.h"

UCLASS()
class ONEPIECE_API ANPCExaminer : public ANPCBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANPCExaminer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
