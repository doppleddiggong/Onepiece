// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "LuggageManager.h"

#include "ALingoGameState.h"
#include "luggage.h"
#include "NetworkData.h"
#include "UGameDataManager.h"


// Sets default values
ALuggageManager::ALuggageManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ALuggageManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALuggageManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void ALuggageManager::SpawnLuggage()
{
	if (!HasAuthority()) return;

	ALingoGameState* GS = Cast<ALingoGameState>(GetWorld()->GetGameState());
	if (!GS) return;

	const TArray<FScenarioTargetData>& ScenarioData = GS->GetScenarioData().target_data;
	
	for (auto SD : ScenarioData)
	{
		Aluggage* NewLuggage = GetWorld()->SpawnActor<Aluggage>(LuggageClass, GetActorLocation(),
		FRotator::ZeroRotator);
	
		if (NewLuggage)
		{
			// 색상 적용
			int32 ColorIdx = FCString::Atoi(*SD.word2.code);
			NewLuggage->ApplyColorToMesh(ColorIdx);
			// 무늬 적용
			NewLuggage->ApplyPatternToMesh(SD.word1.name);
		}
	}
}

