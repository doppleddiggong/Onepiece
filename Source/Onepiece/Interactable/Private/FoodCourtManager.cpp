// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "FoodCourtManager.h"

#include "ADropper.h"
#include "ALingoGameState.h"
#include "CityName.h"
#include "CityNameWidget.h"
#include "ListenAnswer.h"
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
		GetCurrentSpawnLocation(Index, SpawnLocation);

		AListenAnswer* NewActor = GetWorld()->SpawnActor<AListenAnswer>(ListenAnswerClass, SpawnLocation, FRotator::ZeroRotator);

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

	// 도시 이름 선택지 스폰 및 데이터 전달
	for (const FWordInfo& CityInfo : CityInfos)
	{
		// 스폰하기
		FVector SpawnLocation;
		GetCurrentSpawnLocation(Index, SpawnLocation);

		AListenAnswer* NewActor = GetWorld()->SpawnActor<AListenAnswer>(ListenAnswerClass, SpawnLocation, FRotator::ZeroRotator);

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

void AFoodCourtManager::GetCurrentSpawnLocation(int32 Index, FVector& OutSpawnLocation)
{
	// 2열로 배치
	int32 Row = Index / 2;
	int32 Col = Index % 2;

	FVector Offset = FVector(Row*SpawnDistance, Col*SpawnDistance, 0.0f);

	OutSpawnLocation = AnswerSpawnLocation + Offset;
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

