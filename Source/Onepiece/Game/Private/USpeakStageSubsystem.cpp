// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "USpeakStageSubsystem.h"
#include "ASpeakStageActor.h"
#include "NPCExaminer.h"
#include "GameLogging.h"
#include "Engine/World.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"

USpeakStageSubsystem::USpeakStageSubsystem()
{
	// 기본 Blueprint 클래스 경로 설정
	SpeakStageClass = TSoftClassPtr<ASpeakStageActor>(FSoftObjectPath(TEXT("/Game/CustomContents/Blueprints/Enviroment/BP_SpeakStageActor.BP_SpeakStageActor_C")));
	ExaminerClass = TSoftClassPtr<ANPCExaminer>(FSoftObjectPath(TEXT("/Game/CustomContents/Blueprints/BP_NPCExaminer.BP_NPCExaminer_C")));

	// NPC 기본 스폰 위치 (입국 심사대 앞)
	ExaminerSpawnTransform = FTransform(
		FRotator(0.0f, 180.0f, 0.0f),  // 180도 회전 (플레이어 방향)
		FVector(0.0f, 500.0f, 100.0f),  // Y축으로 500, Z축으로 100
		FVector(1.0f, 1.0f, 1.0f)       // 기본 스케일
	);
}

void USpeakStageSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	bIsInitialized = false;

	PRINTLOG(TEXT("[SpeakStageSubsystem] Initialize called"));


}

void USpeakStageSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	this->CreateSpeakStageSystem();
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

	// 1. 먼저 월드에서 기존 SpeakStageActor 찾기
	SpeakStage = Cast<ASpeakStageActor>(UGameplayStatics::GetActorOfClass(World, ASpeakStageActor::StaticClass()));

	if (SpeakStage)
	{
		PRINTLOG(TEXT("[SpeakStageSubsystem] Found existing SpeakStageActor in world: %s"), *SpeakStage->GetName());
		return;
	}

	// 2. 없으면 Config에서 클래스 로드하여 생성
	if (SpeakStageClass.IsNull())
	{
		PRINTLOG(TEXT("[SpeakStageSubsystem] SpeakStageClass is not set in config, skipping spawn"));
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
	SpawnParams.Owner = nullptr;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	SpeakStage = World->SpawnActor<ASpeakStageActor>(
		LoadedClass,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		SpawnParams
	);

	if (SpeakStage)
	{
		PRINTLOG(TEXT("[SpeakStageSubsystem] SpeakStageActor spawned successfully"));
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

	// 1. 먼저 월드에서 기존 NPCExaminer 찾기
	Examiner = Cast<ANPCExaminer>(UGameplayStatics::GetActorOfClass(World, ANPCExaminer::StaticClass()));

	if (Examiner)
	{
		PRINTLOG(TEXT("[SpeakStageSubsystem] Found existing NPCExaminer in world: %s"), *Examiner->GetName());

		// 기존 NPC에 SpeakStage 연결
		if (SpeakStage)
		{
			Examiner->SetSpeakStage(SpeakStage);
			PRINTLOG(TEXT("[SpeakStageSubsystem] Connected existing Examiner to SpeakStage"));
		}
		return;
	}

	// 2. 없으면 Config에서 클래스 로드하여 생성
	if (ExaminerClass.IsNull())
	{
		PRINTLOG(TEXT("[SpeakStageSubsystem] ExaminerClass is not set in config, skipping spawn"));
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
	NPCSpawnParams.Owner = nullptr;
	NPCSpawnParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	Examiner = World->SpawnActor<ANPCExaminer>(
		LoadedClass,
		ExaminerSpawnTransform,
		NPCSpawnParams
	);

	if (Examiner)
	{
		PRINTLOG(TEXT("[SpeakStageSubsystem] Examiner NPC spawned successfully"));

		// NPC에 SpeakStage 연결
		if (SpeakStage)
		{
			Examiner->SetSpeakStage(SpeakStage);
			PRINTLOG(TEXT("[SpeakStageSubsystem] Connected spawned Examiner to SpeakStage"));
		}
	}
	else
	{
		PRINTLOG(TEXT("[SpeakStageSubsystem] Failed to spawn Examiner NPC"));
	}
}
