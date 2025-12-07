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

	void StartSpawning();
	void SpawnLuggage();

	void InitHolder(struct FResponseReadScenario& ResponseData);
	
public:
	// Spawn
	UPROPERTY(EditAnywhere, Category = "Spawn")
	TSubclassOf<class Aluggage> LuggageClass;

	UPROPERTY(EditDefaultsOnly, Category = "Spawn")
	float SpawnTime = 2.f;
	
	FTimerHandle SpawnTimerHandle;
	
	int32 CurrentSpawnIndex = 0;
};
