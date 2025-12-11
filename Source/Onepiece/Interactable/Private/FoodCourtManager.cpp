// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "FoodCourtManager.h"

#include "ADropper.h"
#include "ALingoGameState.h"
#include "CityName.h"
#include "CityNameWidget.h"
#include "OrderKiosk.h"
#include "ULingoGameHelper.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"


AFoodCourtManager::AFoodCourtManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AFoodCourtManager::BeginPlay()
{
	Super::BeginPlay();

	if (ALingoGameState* GS = ULingoGameHelper::GetLingoGameState(GetWorld()))
	{
		GS->OnQuestScenarioDataUpdated.AddDynamic(this, &AFoodCourtManager::HandleQuestScenarioDataUpdated);
		HandleQuestScenarioDataUpdated();
	}
}

void AFoodCourtManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

// Called every frame
void AFoodCourtManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFoodCourtManager::SpawnFoodContainer()
{
	ALingoGameState* GS = Cast<ALingoGameState>(GetWorld()->GetGameState());
	if (GS)
	{
		// Food 스폰
		AActor* FoundActor = UGameplayStatics::GetActorOfClass(GetWorld(), ADropper::StaticClass());
		if (FoundActor)
		{
			Dropper = Cast<ADropper>(FoundActor);
			
			Dropper->SetSpawnClass( LoadClass<AActor>(nullptr, TEXT("/Game/CustomContents/Blueprints/Interactables/BP_Food.BP_Food_C")));
			Dropper->RequestSpawn();
		}
	}
}

void AFoodCourtManager::SetFoodCourtInfo()
{
	// 서버에서만 실행
	if (!HasAuthority()) return;

	ALingoGameState* GS = Cast<ALingoGameState>(GetWorld()->GetGameState());
	if (!GS) return;

	const TArray<FScenarioTargetData>& ScenarioData = GS->GetListenScenarioData().target_data;

	for (int32 i=0; i<ScenarioData.Num(); i++)
	{
		auto SD = ScenarioData[i];

		// 1초 타이머 - 액터들이 클라이언트에 리플리케이트되길 대기
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this, i, SD]
		{
			// 푸드코트 식당 이름 지정
			ACityName* CityName = FindCityNameByIdx(i);
			if (CityName)
			{
				CityName->SetCityName(SD.word2.name);
				// 서버는 OnRep이 호출되지 않으므로 직접 위젯 업데이트
				CityName->OnRep_FoodCourtInfo();
			}

			// 키오스크 지정
			AOrderKiosk* CurrentKiosk = FindKioskNameByIdx(i);
			if (CurrentKiosk)
			{
				CurrentKiosk->FoodCourtIdx = i;
				CurrentKiosk->FoodData.word1 = SD.word1;
				CurrentKiosk->FoodData.word2 = SD.word2;

				// 서버 로컬 위젯 업데이트
				CurrentKiosk->UpdateInteractableWidget(SD.word2.name);
			}
			
		}), 1.f, false);
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

class AOrderKiosk* AFoodCourtManager::FindKioskNameByIdx(int32 InIdx)
{
	TArray<AActor*> AllKiosks;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOrderKiosk::StaticClass(), AllKiosks);

	for (auto Kiosk : AllKiosks)
	{
		if (AOrderKiosk* Kio = Cast<AOrderKiosk>(Kiosk))
		{
			if (Kio->Index == InIdx)
			{
				return Kio;
			}
		}
	}
	return nullptr;
}

/*
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
*/
void AFoodCourtManager::HandleQuestScenarioDataUpdated()
{
	if (ALingoGameState* GS = ULingoGameHelper::GetLingoGameState(GetWorld()))
	{
		if (GS->GetCurrentQuestType() != EQuestType::Listen)
			return;

		SetFoodCourtInfo();
	}
}

