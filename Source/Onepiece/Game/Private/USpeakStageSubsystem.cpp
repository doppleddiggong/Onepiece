// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "USpeakStageSubsystem.h"
#include "ASpeakStageActor.h"
#include "NPCExaminer.h"
#include "GameLogging.h"
#include "Engine/World.h"
#include "GameFramework/PlayerState.h"

void USpeakStageSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	bIsInitialized = false;

	PRINTLOG(TEXT("[SpeakStageSubsystem] Initialize called"));

	// SpeakStage 시스템 생성
	CreateSpeakStageSystem();
}

void USpeakStageSubsystem::Deinitialize()
{
	PRINTLOG(TEXT("[SpeakStageSubsystem] Deinitialize called"));

	// 정리 작업
	SpeakStage = nullptr;
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
	{
		PRINTLOG(TEXT("[SpeakStageSubsystem] CreateSpeakStageSystem - World is nullptr"));
		return;
	}

	// 서버에서만 생성
	if (!World->IsNetMode(NM_Client))
	{
		// 1. SpeakStageActor 생성
		CreateSpeakStageActor();

		// 2. NPC Examiner 생성
		CreateNPCExaminer();

		bIsInitialized = true;
		PRINTLOG(TEXT("[SpeakStageSubsystem] SpeakStage system created successfully"));
	}
	else
	{
		PRINTLOG(TEXT("[SpeakStageSubsystem] Skipping creation on client"));
	}
}

void USpeakStageSubsystem::BeginSpeakQuest(int32 InStageIndex)
{
	if (!SpeakStage)
	{
		PRINTLOG(TEXT("[SpeakStageSubsystem] BeginSpeakQuest - SpeakStage is null"));
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		PRINTLOG(TEXT("[SpeakStageSubsystem] BeginSpeakQuest - World is nullptr"));
		return;
	}

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
	SpeakStage->PlayStart(Players);

	PRINTLOG(TEXT("[SpeakStageSubsystem] BeginSpeakQuest - Stage %d started with %d players"),
	         InStageIndex, Players.Num());
}

//----------------------------------------------------------
// Private Implementation
//----------------------------------------------------------

void USpeakStageSubsystem::CreateSpeakStageActor()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// SpeakStageClass가 설정되지 않은 경우 경고
	if (SpeakStageClass.IsNull())
	{
		PRINTLOG(TEXT("[SpeakStageSubsystem] SpeakStageClass is not set in config"));
		return;
	}

	// 클래스 로드
	UClass* LoadedClass = SpeakStageClass.LoadSynchronous();
	if (!LoadedClass)
	{
		PRINTLOG(TEXT("[SpeakStageSubsystem] Failed to load SpeakStageClass"));
		return;
	}

	// SpeakStageActor 생성
	FActorSpawnParameters SpawnParams;
	SpawnParams.Name = FName(TEXT("SpeakStageActor"));
	SpawnParams.Owner = World->GetAuthGameMode();

	SpeakStage = World->SpawnActor<ASpeakStageActor>(
		LoadedClass,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		SpawnParams
	);

	if (SpeakStage)
	{
		PRINTLOG(TEXT("[SpeakStageSubsystem] SpeakStageActor created successfully"));
	}
	else
	{
		PRINTLOG(TEXT("[SpeakStageSubsystem] Failed to spawn SpeakStageActor"));
	}
}

void USpeakStageSubsystem::CreateNPCExaminer()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// ExaminerClass가 설정되지 않은 경우 경고
	if (ExaminerClass.IsNull())
	{
		PRINTLOG(TEXT("[SpeakStageSubsystem] ExaminerClass is not set in config"));
		return;
	}

	// 클래스 로드
	UClass* LoadedClass = ExaminerClass.LoadSynchronous();
	if (!LoadedClass)
	{
		PRINTLOG(TEXT("[SpeakStageSubsystem] Failed to load ExaminerClass"));
		return;
	}

	// NPC Examiner 생성
	FActorSpawnParameters NPCSpawnParams;
	NPCSpawnParams.Owner = World->GetAuthGameMode();
	NPCSpawnParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	Examiner = World->SpawnActor<ANPCExaminer>(
		LoadedClass,
		ExaminerSpawnTransform,
		NPCSpawnParams
	);

	if (Examiner && SpeakStage)
	{
		// NPC에 SpeakStage 연결
		Examiner->SetSpeakStage(SpeakStage);
		PRINTLOG(TEXT("[SpeakStageSubsystem] Examiner NPC created and connected to SpeakStage"));
	}
	else if (Examiner)
	{
		PRINTLOG(TEXT("[SpeakStageSubsystem] Examiner NPC created (but no SpeakStage)"));
	}
	else
	{
		PRINTLOG(TEXT("[SpeakStageSubsystem] Failed to spawn Examiner NPC"));
	}
}
