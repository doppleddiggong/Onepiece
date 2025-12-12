// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Delegates/Delegate.h"
#include "GameFramework/Actor.h"
#include "FoodCourtManager.generated.h"

/*
 * 파싱 받은 데이터를 각 식당과 음식에 배정
 */
UCLASS()
class ONEPIECE_API AFoodCourtManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFoodCourtManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	//---------------------------------------------------
	// Dropper
	//---------------------------------------------------
	
	UPROPERTY(VisibleAnywhere)
	class ADropper* Dropper;
	
	// 빈 음식 액터 스폰
	void SpawnFoodContainer();

public:
	//---------------------------------------------------
	// 선택지 스폰
	//---------------------------------------------------

	// 듣기 선택지 스폰 및 데이터 전달
	void SpawnListenAnswer();
	// 첫번째 스폰 위치 기준으로 나머지 스폰 위치 구하기
	void GetCurrentSpawnLocation(int32 Index, FVector& OutSpawnLocation);
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Item")
	TSubclassOf<class AListenAnswer> ListenAnswerClass;
	
	// 선택지 첫번째 스폰 위치
	UPROPERTY(EditAnywhere, Category = "Spawn")
	FVector FoodSpawnLocation = FVector(2561.899883,-5122.399085,-4636.869443);

	UPROPERTY(EditAnywhere, Category = "Spawn")
	FVector CitySpawnLocation;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	float SpawnDistance = 2.f;

private:
	//---------------------------------------------------
	// Handler
	//---------------------------------------------------
	
	UFUNCTION()
	void HandleQuestScenarioDataUpdated();
};
