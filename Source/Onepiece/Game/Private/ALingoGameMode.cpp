// Fill out your copyright notice in the Description page of Project Settings.


#include "ALingoGameMode.h"

// #include "ASequencePlayer.h"
#include "ULingoGameInstance.h"
#include "ALingoGameState.h"
// #include "FireMan.h"
// #include "PeopleBase.h"
#include "GameLogging.h"
#include "TimerManager.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

ALingoGameMode::ALingoGameMode()
{
	// GameState와 PlayerState 클래스 설정
	GameStateClass = ALingoGameState::StaticClass();
	PlayerStateClass = ALingoPlayerState::StaticClass();

	// // Default Pawn Classes 설정
	// FirefighterPawnClass = AFireMan::StaticClass();
	// CitizenPawnClass = APeopleBase::StaticClass();

	// Tick 비활성화 (Timer 기반으로 동작)
	PrimaryActorTick.bCanEverTick = false;
}

UClass* ALingoGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	// GameInstance에서 PlayerController의 Role 조회
	APlayerController* PC = Cast<APlayerController>(InController);
	if (!PC)
	{
		PRINTLOG( TEXT("[HouseGameMode] Controller is not a PlayerController, using default"));
		return Super::GetDefaultPawnClassForController(InController);
	}

	ULingoGameInstance* GameInstance = GetGameInstance<ULingoGameInstance>();
	if (!GameInstance)
	{
		PRINTLOG( TEXT("[HouseGameMode] FireGameInstance not found! Using default pawn."));
		return Super::GetDefaultPawnClassForController(InController);
	}

	// Role 조회
	EPlayerRole CurrentRole = GameInstance->GetPlayerRole(PC);

	// Role에 따라 Pawn Class 결정
	UClass* SelectedPawnClass = nullptr;

	switch (CurrentRole)
	{
	case EPlayerRole::Firefighter:
		SelectedPawnClass = FirefighterPawnClass;
		PRINTLOG( TEXT("[HouseGameMode] Spawning Firefighter pawn for %s"), *PC->GetName());
		break;

	case EPlayerRole::Citizen:
		SelectedPawnClass = CitizenPawnClass;
		PRINTLOG( TEXT("[HouseGameMode] Spawning Citizen pawn for %s"), *PC->GetName());
		break;

	default:
		PRINTLOG( TEXT("[HouseGameMode] No role assigned for %s, using Citizen as default"), *PC->GetName());
		SelectedPawnClass = CitizenPawnClass;
		break;
	}

	// Pawn Class가 설정되지 않았으면 기본값 사용
	if (!SelectedPawnClass)
	{
		PRINTLOG( TEXT("[HouseGameMode] Selected pawn class is null! Using default."));
		return Super::GetDefaultPawnClassForController(InController);
	}

	// PlayerState에도 Role 저장 (동기화)
	ALingoPlayerState* HousePS = PC->GetPlayerState<ALingoPlayerState>();
	if (HousePS && CurrentRole != EPlayerRole::None)
	{
		HousePS->PlayerRole = CurrentRole;
		PRINTLOG( TEXT("[HouseGameMode] Set PlayerState role to %s"),
			*UEnum::GetValueAsString(CurrentRole));
	}

	return SelectedPawnClass;
}

void ALingoGameMode::BeginPlay()
{
	Super::BeginPlay();

	// 초기 상태 설정
	ALingoGameState* HouseGS = GetGameState<ALingoGameState>();
	if (HouseGS)
	{
		HouseGS->CurrentPhase = EGamePhase::WaitingToStart;
		HouseGS->CurrentMissionIndex = 0;
		HouseGS->MissionTimeRemaining = 0.f;

		PRINTLOG( TEXT("[HouseGameMode] Waiting to start"));

		// 임시로 여기
		SetAllPlayersStart();
		
		// 시작 시네마틱
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([&]
		{
			ChangeGamePhase(EGamePhase::Intro);

		}), 1.f, false);
	}
}

void ALingoGameMode::StartGame()
{
	ALingoGameState* HouseGS = GetGameState<ALingoGameState>();
	if (!HouseGS) return;

	ChangeGamePhase(EGamePhase::GameStart);
	
	HouseGS->MissionTimeRemaining = HouseGS->MissionTimeLimit;
	
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([&]
	{
		StartMission(0);
		
	}),1.f, false);

	GetWorldTimerManager().SetTimer(MissionTimerHandle, this, &ALingoGameMode::UpdateMissionTimer,
		1.f, true);
}

void ALingoGameMode::UpdateMissionTimer()
{
	ALingoGameState* HouseGS = GetGameState<ALingoGameState>();
	if (!HouseGS) return;

	HouseGS->MissionTimeRemaining -= 1.f;

	if (HouseGS->MissionTimeRemaining <= 0.f)
	{
		PRINTLOG( TEXT("[HouseGameMode] Mission failed."));
		FailMission();
		return;
	}

	/*
	int32 TimeLeft = FMath::FloorToInt(HouseGS->MissionTimeRemaining);
	if (TimeLeft == 30 || TimeLeft == 10)
	{
		UE_LOG(LogTemp, Warning, TEXT("[HouseGameMode] %d seconds remaining!"), TimeLeft);
	}
	*/
}

void ALingoGameMode::ReportMissionComplete(APlayerController* Player)
{
	if (!Player) return;

	ALingoPlayerState* HousePS = Player->GetPlayerState<ALingoPlayerState>();
	if (!HousePS) return;

	// PlayerState에서 이미 bCurrentMissionComplete를 설정했으므로 체크만 수행
	PRINTLOG( TEXT("[HouseGameMode] %s reported mission complete"), *Player->GetName());

	CheckMissionProgress();
}

void ALingoGameMode::StartMission(int32 MissionIndex)
{
	ALingoGameState* HouseGS = GetGameState<ALingoGameState>();
	if (!HouseGS) return;

	if (MissionIndex >= TotalMissions) return;

	//ChangeGamePhase(EGamePhase::MissionStart);
	
	// 미션 시작
	HouseGS->CurrentMissionIndex = MissionIndex;
	
	// 모든 플레이어 미션 상태 리셋
	for (APlayerState* PS : HouseGS->PlayerArray)
	{
		ALingoPlayerState* HousePS = Cast<ALingoPlayerState>(PS);
		if (HousePS)
		{
			HousePS->ResetForNextMission();
		}
	}

	ChangeGamePhase(EGamePhase::MissionStart);
}

void ALingoGameMode::CheckMissionProgress()
{
	ALingoGameState* HouseGS = GetGameState<ALingoGameState>();
	if (!HouseGS)
		return;

	PRINTLOG( TEXT("[CheckMissionProgress] Called. Current mission: %d"), HouseGS->CurrentMissionIndex); 
	
	if (HouseGS->AreAllPlayersMissionComplete())
	{
		PRINTLOG( TEXT("[CheckMissionProgress] All missions complete, advancing..."));
		AdvanceToNextMission();
	}
}

void ALingoGameMode::AdvanceToNextMission()
{
	ALingoGameState* HouseGS = GetGameState<ALingoGameState>();
	if (!HouseGS) return;
	
	int32 NextMissionIndex = HouseGS->CurrentMissionIndex + 1;

	if (NextMissionIndex >= TotalMissions)
	{
		// 승리!
		ChangeGamePhase(EGamePhase::Outro);

		GetWorldTimerManager().ClearTimer(MissionTimerHandle);
		return;
	}
	
	// 다음 미션 시작
	ChangeGamePhase(EGamePhase::MissionComplete);

	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this, NextMissionIndex]
	{
		StartMission(NextMissionIndex);
		
	}),1.f, false);
}

void ALingoGameMode::FailMission()
{
	ChangeGamePhase(EGamePhase::GameOver);
	
	GetWorldTimerManager().ClearTimer(MissionTimerHandle);
}

void ALingoGameMode::Victory()
{
	// 승리!
	ChangeGamePhase(EGamePhase::Victory);
}

void ALingoGameMode::ChangeGamePhase(EGamePhase NewState)
{
	ALingoGameState* HouseGS = GetGameState<ALingoGameState>();
	if (!HouseGS) return;
	
	// 클라이언트쪽 상태 변경
	HouseGS->CurrentPhase = NewState;

	// 서버쪽 상태 변경 브로드캐스트
	if (HasAuthority())
	{
		HouseGS->OnPhaseChanged.Broadcast(NewState);
		HouseGS->PreviousPhase = NewState;
	}
}

void ALingoGameMode::SetAllPlayersStart()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (!PC || !PC->GetPawn()) continue;

		ALingoPlayerState* HousePS = PC->GetPlayerState<ALingoPlayerState>();
		if (!HousePS) continue;

		EPlayerRole CurrentRole = HousePS->PlayerRole;
		FName PlayerStartTag;

		// Role에 따라 찾을 Tag 결정
		switch (CurrentRole)
		{
		case EPlayerRole::Firefighter:
			PlayerStartTag = FName("Firefighter");
			break;
		case EPlayerRole::Citizen:
			PlayerStartTag = FName("Citizen");
			break;
		default:
			PRINTLOG(TEXT("[HouseGameMode] No role assigned, skipping spawn"));
			continue;
		}

		// Tag로 PlayerStart 찾기
		TArray<AActor*> FoundStarts;
		UGameplayStatics::GetAllActorsOfClassWithTag(this, APlayerStart::StaticClass(), PlayerStartTag, FoundStarts);

		if (FoundStarts.Num() > 0)
		{
			APlayerStart* PlayerStart = Cast<APlayerStart>(FoundStarts[0]);
			if (PlayerStart)
			{
				PC->GetPawn()->SetActorTransform(PlayerStart->GetActorTransform());
				PC->GetPawn()->SetActorHiddenInGame(false);
			}
		}
		else
		{
			PRINTLOG(TEXT("[HouseGameMode] PlayerStart with tag '%s' not found!"), *PlayerStartTag.ToString());
		}
	}
}


