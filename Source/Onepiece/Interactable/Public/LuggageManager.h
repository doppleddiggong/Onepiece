// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LuggageManager.generated.h"

UCLASS()
class ONEPIECE_API ALuggageManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALuggageManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	// Spawn
	UPROPERTY(EditAnywhere, Category = "Spawn")
	TSubclassOf<class Aluggage> LuggageClass;
	
	FTimerHandle SpawnTimerHandle;
	
	void SpawnLuggage();

};
