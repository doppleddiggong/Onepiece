// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "FoodCourtManager.h"

#include "ADropper.h"
#include "ALingoGameState.h"
#include "Food.h"
#include "ListenAnswer.h"
#include "ULingoGameHelper.h"
#include "Kismet/GameplayStatics.h"

AFoodCourtManager::AFoodCourtManager()
{
	PrimaryActorTick.bCanEverTick = true;
	
	ConstructorHelpers::FClassFinder<AFood> foodClass(TEXT("/Game/CustomContents/Blueprints/Interactables/BP_Food.BP_Food_C"));
	if (foodClass.Succeeded())
	{
		FoodClass = foodClass.Class;
	}
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
			
			Dropper->SetSpawnClass(FoodClass);
			Dropper->RequestSpawn();
		}
	}
}

void AFoodCourtManager::SpawnListenAnswer()
{
	// 서버에서만 실행
	if (!HasAuthority()) return;

	ALingoGameState* GS = Cast<ALingoGameState>(GetWorld()->GetGameState());
	if (!GS) return;

	const TArray<FScenarioTargetData>& ScenarioData = GS->GetListenScenarioData().target_data;

	// 선택지 중복 안되도록 추리기
	TSet<FWordInfo> FoodInfos;
	TSet<FWordInfo> CityInfos;

	for (int32 i=0; i<ScenarioData.Num(); i++)
	{
		auto SD = ScenarioData[i];
		
		CityInfos.Add(SD.word1);
		FoodInfos.Add(SD.word2);
	}

	// 음식 선택지 스폰 및 데이터 전달
	int32 Index = 0;
	for (const FWordInfo& FoodInfo : FoodInfos)
	{
		// 스폰하기
		FVector SpawnLocation;
		GetCurrentSpawnLocation(Index, FoodSpawnLocation, 0, SpawnLocation);

		AListenAnswer* NewActor = GetWorld()->SpawnActor<AListenAnswer>(ListenAnswerClass, SpawnLocation, FRotator::ZeroRotator);
		SpawnedListenAnswers.Add(NewActor);
		
		// 데이터 전달
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this, NewActor, FoodInfo]
		{
			NewActor->AnswerData.AnswerType = EAnswerType::Food;
			NewActor->AnswerData.word1 = FoodInfo;

			// 로컬 위젯 업데이트
			NewActor->UpdateMesh();
			NewActor->UpdateNameWidget();

		}), 1.f, false);

		Index++;
	}

	Index = 0;
	// 도시 이름 선택지 스폰 및 데이터 전달
	for (const FWordInfo& CityInfo : CityInfos)
	{
		// 스폰하기
		FVector SpawnLocation;
		GetCurrentSpawnLocation(Index, CitySpawnLocation, 3, SpawnLocation);

		AListenAnswer* NewActor = GetWorld()->SpawnActor<AListenAnswer>(ListenAnswerClass, SpawnLocation, FRotator::ZeroRotator);
		SpawnedListenAnswers.Add(NewActor);
		
		// 데이터 전달
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this, NewActor, CityInfo]
		{
			NewActor->AnswerData.AnswerType = EAnswerType::City;
			NewActor->AnswerData.word1 = CityInfo;

			// 로컬 위젯 업데이트
			NewActor->UpdateMesh();
			NewActor->UpdateNameWidget();

		}), 1.f, false);

		Index++;
	}
}

void AFoodCourtManager::GetCurrentSpawnLocation(int32 Index, FVector InitialLocation, int32 Dir, FVector& OutSpawnLocation)
{
	// 2열로 배치
	int32 Row = Index / 2;
	int32 Col = Index % 2;

	// Dir 값에 따라 스폰 방향 변경
	// 0: 전방-우측, 1: 전방-좌측, 2: 후방-우측, 3: 후방-좌측
	FVector Offset;

	switch (Dir)
	{
	case 0:
		Offset = FVector(Row*SpawnDistance, Col*SpawnDistance, 0.0f);
		break;
	case 1:
		Offset = FVector(-Col*SpawnDistance, Row*SpawnDistance, 0.0f);
		break;
	case 2:
		Offset = FVector(-Row*SpawnDistance, -Col*SpawnDistance, 0.0f);
		break;
	case 3:
		Offset = FVector(Col*SpawnDistance, -Row*SpawnDistance, 0.0f);
		break;
	default:
		Offset = FVector(Row*SpawnDistance, Col*SpawnDistance, 0.0f);
		break;
	}

	OutSpawnLocation = InitialLocation + Offset;
}

void AFoodCourtManager::DisableAllListenAnswersText()
{
	for (AActor* Actor : SpawnedListenAnswers)
	{
		if (AListenAnswer* Answer = Cast<AListenAnswer>(Actor))
		{
			Answer->AnswerData.word1.name = "";
			Answer->UpdateNameWidget();
		}
	}
}

void AFoodCourtManager::HandleQuestScenarioDataUpdated()
{
	if (ALingoGameState* GS = ULingoGameHelper::GetLingoGameState(GetWorld()))
	{
		if (GS->GetCurrentQuestType() != EQuestType::Listen)
			return;

		SpawnListenAnswer();
	}
}

