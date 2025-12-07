// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "FoodCourtManager.h"

#include "ADropper.h"
#include "ALingoGameState.h"
#include "CityName.h"
#include "CityNameWidget.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AFoodCourtManager::AFoodCourtManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AFoodCourtManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFoodCourtManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFoodCourtManager::SetFoodCourtInfo()
{
	ALingoGameState* GS = Cast<ALingoGameState>(GetWorld()->GetGameState());
	if (!GS) return;

	const TArray<FScenarioTargetData>& ScenarioData = GS->GetListenScenarioData().target_data;

	for (int32 i=0; i<ScenarioData.Num(); i++)
	{
		auto SD = ScenarioData[i];

		// 푸드코트 식당 이름 지정
		ACityName* CityName = FindCityNameByIdx(i);
		if (CityName)
		{
			UUserWidget* CityNameWidget = CityName->WidgetComp->GetWidget();
			if (UCityNameWidget* CNW = Cast<UCityNameWidget>(CityNameWidget))
			{
				CNW->SetCityName(SD.word1.name);
			}
		}

		// 음식 이름 지정
		ADropper* Dropper = FindDropperByIdx(i);
		if (Dropper)
		{
			FFoodData tmpData;
			tmpData.word = SD.word2;
			tmpData.SpawnIndex = i;
			
			Dropper->SetFoodSpawnData(tmpData);
			Dropper->SetSpawnClass( LoadClass<AActor>(nullptr, TEXT("/Game/CustomContents/Blueprints/Interactables/BP_Food.BP_Food_C")));
			Dropper->RequestSpawn();
		}
	}
}

ACityName* AFoodCourtManager::FindCityNameByIdx(int32 InIdx)
{
	TArray<AActor*> CityNames;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACityName::StaticClass(), CityNames);

	for (auto CityName : CityNames)
	{
		if (ACityName* CN = Cast<ACityName>(CityName))
		{
			if (CN->Index == InIdx)
			{
				return CN;
			}
		}
	}
	return nullptr;
}

class ADropper* AFoodCourtManager::FindDropperByIdx(int32 InIdx)
{
	TArray<AActor*> Droppers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADropper::StaticClass(), Droppers);

	for (auto Dropper : Droppers)
	{
		if (ADropper* Dpp = Cast<ADropper>(Dropper))
		{
			if (Dpp->DropperIndex == InIdx)
			{
				return Dpp;
			}
		}
	}
	return nullptr;
}

