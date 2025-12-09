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
	void SetFoodCourtInfo();
	class ACityName* FindCityNameByIdx(int32 InIdx);
	class AOrderKiosk* FindKioskNameByIdx(int32 InIdx);
	//class AOrderKiosk* GetRandomKiosk();

private:
	UFUNCTION()
	void HandleQuestScenarioDataUpdated();
};
