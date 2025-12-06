// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "FoodCourtManager.h"

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

	const TArray<FScenarioTargetData>& ScenarioData = GS->GetScenarioData().target_data;

	for (int32 i=0; i<ScenarioData.Num(); i++)
	{
		auto SD = ScenarioData[i];

		// 푸드코트 식당 이름 지정
		ACityName* CityName = FindCityName(i);
		if (!CityName)
		{
			UE_LOG(LogTemp, Warning, TEXT("FindCityName failed for index %d"), i);
			continue;
		}

		if (!CityName->WidgetComp)
		{
			UE_LOG(LogTemp, Warning, TEXT("CityName->WidgetComp is null for index %d"), i);
			continue;
		}

		UUserWidget* CityNameWidget = CityName->WidgetComp->GetWidget();
		if (UCityNameWidget* CNW = Cast<UCityNameWidget>(CityNameWidget))
		{
			CNW->SetCityName(SD.word1.name);
		}
	}
}

ACityName* AFoodCourtManager::FindCityName(int32 InIdx)
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

