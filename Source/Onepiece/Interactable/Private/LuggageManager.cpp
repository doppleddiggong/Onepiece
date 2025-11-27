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

void ALuggageManager::StartSpawning()
{
	if (!HasAuthority()) return;
	
	CurrentSpawnIndex = 0;
	
	// 첫 번째 luggage는 즉시 스폰
	SpawnLuggage();
	
	// 나머지는 타이머로 스폰
	GetWorldTimerManager().SetTimer(
		SpawnTimerHandle, 
		this, 
		&ALuggageManager::SpawnLuggage, 
		SpawnTime, 
		true  // 반복
	);
}

void ALuggageManager::SpawnLuggage()
{
	ALingoGameState* GS = Cast<ALingoGameState>(GetWorld()->GetGameState());
	if (!GS) return;

	const TArray<FScenarioTargetData>& ScenarioData = GS->GetScenarioData().target_data;
	
	// 모두 스폰했으면 타이머 중지
	if (CurrentSpawnIndex >= ScenarioData.Num())
	{
		GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
		return;
	}

	// 현재 인덱스의 luggage 스폰
	auto SD = ScenarioData[CurrentSpawnIndex];
	Aluggage* NewLuggage = GetWorld()->SpawnActor<Aluggage>(
		LuggageClass, 
		GetActorLocation(),
		FRotator::ZeroRotator
	);

	if (NewLuggage)
	{
		// 상자 정보 지정
		NewLuggage->SetLuggageInfo(CurrentSpawnIndex, SD.word2.name, SD.word1.name);

		// 인덱스로 상자 색&무늬 지정
		int32 ColorIdx = FCString::Atoi(*SD.word2.code);
		NewLuggage->ApplyColorToMesh(ColorIdx);
		int32 PatternIdx = FCString::Atoi(*SD.word1.code);
		NewLuggage->ApplyPatternToMesh(PatternIdx);
	}

	CurrentSpawnIndex++;
}

