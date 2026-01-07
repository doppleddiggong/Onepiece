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

	UPROPERTY()
	TArray<AActor*> SpawnedListenAnswers;
	
	// 듣기 선택지 스폰 및 데이터 전달
	void SpawnListenAnswer();
	// 첫번째 스폰 위치 기준으로 나머지 스폰 위치 구하기
	void GetCurrentSpawnLocation(int32 Index, FVector InitialLocation, int32 Dir,
		FVector& OutSpawnLocation);
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Item")
	TSubclassOf<class AListenAnswer> ListenAnswerClass;
	
	// 음식 선택지 스폰 위치
	UPROPERTY(EditAnywhere, Category = "Spawn")
	FVector FoodSpawnLocation = FVector(2074.407733,-5199.513819,-4559.096513);

	// 도시 선택지 스폰 위치
	UPROPERTY(EditAnywhere, Category = "Spawn")
	FVector CitySpawnLocation = FVector(1064.733696,-5547.531142,-4124.149377);

	// 스폰 간격
	UPROPERTY(EditAnywhere, Category = "Spawn")
	float SpawnDistance = 150.f;
	
	// Food Class
	UPROPERTY(EditAnywhere, Category = "Food")
	TSubclassOf<AActor> FoodClass;

public:
	// 스폰된 요소들 텍스트 없애기
	void DisableAllListenAnswersText();
	
private:
	//---------------------------------------------------
	// Handler
	//---------------------------------------------------
	
	UFUNCTION()
	void HandleQuestScenarioDataUpdated();
};
