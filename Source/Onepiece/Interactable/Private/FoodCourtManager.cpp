// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "FoodCourtManager.h"

#include "ADropper.h"
#include "ALingoGameState.h"
#include "CityName.h"
#include "CityNameWidget.h"
#include "OrderKiosk.h"
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
				CNW->SetCityName(SD.word2.name);
			}
		}

		// 랜덤 키오스크 지정
		AOrderKiosk* RandomKiosk = GetRandomKiosk();
		if (RandomKiosk)
		{
			RandomKiosk->FoodCourtIdx = i;
			RandomKiosk->FoodData.word1 = SD.word1;
			RandomKiosk->FoodData.word2 = SD.word2;

			RandomKiosk->UpdateInteractableWidget(SD.word2.name);
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

class AOrderKiosk* AFoodCourtManager::GetRandomKiosk()
{
	// 키오스크 중 랜덤으로 하나 뽑기
	TArray<AActor*> AllKiosks;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOrderKiosk::StaticClass(), AllKiosks);

	TArray<AOrderKiosk*> Available;
	for (auto Actor : AllKiosks)
	{
		if (AOrderKiosk* Kiosk = Cast<AOrderKiosk>(Actor))
		{
			if (Kiosk->FoodCourtIdx == -1)
				Available.Add(Kiosk);
		}
	}

	if (Available.IsEmpty()) return nullptr;

	return Available[FMath::RandRange(0, Available.Num()-1)];
}

