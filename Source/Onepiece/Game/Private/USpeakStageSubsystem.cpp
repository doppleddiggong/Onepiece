// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "USpeakStageSubsystem.h"
#include "ASpeakStageActor.h"
#include "AWheatly.h"
#include "NPCExaminer.h"
#include "GameLogging.h"
#include "Engine/World.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"

USpeakStageSubsystem::USpeakStageSubsystem()
{
}

void USpeakStageSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	bIsInitialized = false;
}

void USpeakStageSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	this->CreateSpeakStageSystem();
}

void USpeakStageSubsystem::Deinitialize()
{
	SpeakStageActor = nullptr;
	Examiner = nullptr;
	bIsInitialized = false;

	Super::Deinitialize();
}

//----------------------------------------------------------
// Public Interface
//----------------------------------------------------------

void USpeakStageSubsystem::CreateSpeakStageSystem()
{
	UWorld* World = GetWorld();
	if (!World)
		return;

	// 이미 초기화되었으면 중복 생성 방지
	if (bIsInitialized)
		return;

	// 특정 맵에서만 생성: Map1, Lvl_ThirdPerson
	FString MapName = World->GetMapName();
	MapName.RemoveFromStart(World->StreamingLevelsPrefix);  // PIE prefix 제거

	if (!MapName.Equals(TEXT("Map1")) && !MapName.Equals(TEXT("Lvl_ThirdPerson")))
	{
		// 다른 맵에서는 초기화 완료로 표시
		bIsInitialized = true;  
		return;
	}

	// 클라이언트 월드에서는 생성하지 않음
	if (World->IsNetMode(NM_Client))
	{
		return;
	}

	// 1. SpeakStageActor 생성
	InitActor();

	bIsInitialized = true;
}

void USpeakStageSubsystem::BeginSpeakQuest(int32 InStageIndex)
{
	// Deprecated: This logic is obsolete. The quest is now started via direct interaction with AWheatly,
	// which calls ASpeakStageActor::StartStageForPlayer for a single user.
	/*
	if (!SpeakStageActor)
		return;

	UWorld* World = GetWorld();
	if (!World)
		return;

	// 모든 플레이어 수집
	TArray<APlayerState*> Players;
	for (auto It = World->GetPlayerControllerIterator(); It; ++It)
	{
		if (APlayerController* PC = It->Get())
		{
			if (APlayerState* PS = PC->GetPlayerState<APlayerState>())
			{
				Players.Add(PS);
			}
		}
	}

	// PlayStart 호출 (명시적 초기화)
	SpeakStageActor->PlayStart(Players);
	*/
}

//----------------------------------------------------------
// Private Implementation
//----------------------------------------------------------

void USpeakStageSubsystem::InitActor()
{
	if( auto World = GetWorld() )
	{
		SpeakStageActor = Cast<ASpeakStageActor>(UGameplayStatics::GetActorOfClass(World, ASpeakStageActor::StaticClass()));

		Examiner = Cast<AWheatly>(UGameplayStatics::GetActorOfClass(World, AWheatly::StaticClass()));

		if (Examiner && SpeakStageActor)
			Examiner->SetSpeakStage(SpeakStageActor);
	}
}