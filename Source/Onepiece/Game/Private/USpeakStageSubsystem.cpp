// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "USpeakStageSubsystem.h"
#include "ASpeakStageActor.h"
#include "NPCExaminer.h"
#include "GameLogging.h"
#include "Engine/World.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"

#define SPEAKSTAGEACTOR_PATH TEXT("/Game/CustomContents/Blueprints/Enviroment/BP_SpeakStageActor.BP_SpeakStageActor_C")
#define NPCEXAMINER_PATH	 TEXT("/Game/CustomContents/Blueprints/BP_NPCExaminer.BP_NPCExaminer_C")

USpeakStageSubsystem::USpeakStageSubsystem()
{
	// 기본 Blueprint 클래스 경로 설정
	SpeakStageClass = TSoftClassPtr<ASpeakStageActor>(FSoftObjectPath(SPEAKSTAGEACTOR_PATH));
	ExaminerClass = TSoftClassPtr<ANPCExaminer>(FSoftObjectPath(NPCEXAMINER_PATH));

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
	CreateSpeakStageActor();

	// 2. NPC Examiner 생성
	CreateNPCExaminer();

	bIsInitialized = true;
}

void USpeakStageSubsystem::BeginSpeakQuest(int32 InStageIndex)
{
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
	SpeakStageActor = Cast<ASpeakStageActor>(UGameplayStatics::GetActorOfClass(World, ASpeakStageActor::StaticClass()));

	if (SpeakStageActor)
		return;

	// 2. 없으면 Config에서 클래스 로드하여 생성
	if (SpeakStageClass.IsNull())
		return;

	// 클래스 로드
	UClass* LoadedClass = SpeakStageClass.LoadSynchronous();
	if (!LoadedClass)
		return;

	// SpeakStageActor 생성
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = nullptr;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	SpeakStageActor = World->SpawnActor<ASpeakStageActor>(
		LoadedClass,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		SpawnParams
	);

	if (SpeakStageActor)
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
		return;

	// 1. 먼저 월드에서 기존 NPCExaminer 찾기
	Examiner = Cast<ANPCExaminer>(UGameplayStatics::GetActorOfClass(World, ANPCExaminer::StaticClass()));

	if (Examiner)
	{
		// 기존 NPC에 SpeakStage 연결
		if (SpeakStageActor)
			Examiner->SetSpeakStage(SpeakStageActor);

		return;
	}

	// 2. 없으면 Config에서 클래스 로드하여 생성
	if (ExaminerClass.IsNull())
		return;

	// 클래스 로드
	UClass* LoadedClass = ExaminerClass.LoadSynchronous();
	if (!LoadedClass)
		return;

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
		// NPC에 SpeakStage 연결
		if (SpeakStageActor)
		{
			Examiner->SetSpeakStage(SpeakStageActor);
			PRINTLOG(TEXT("[SpeakStageSubsystem] Connected spawned Examiner to SpeakStage"));
		}
	}
	else
	{
		PRINTLOG(TEXT("[SpeakStageSubsystem] Failed to spawn Examiner NPC"));
	}
}