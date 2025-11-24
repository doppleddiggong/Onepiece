// Fill out your copyright notice in the Description page of Project Settings.


#include "ALingoGameState.h"

#include "Net/UnrealNetwork.h"
#include "APlayerActor.h"
#include "GameLogging.h"
#include "ULingoGameHelper.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

ALingoGameState::ALingoGameState()
{
	RemainMissionTime = 0.f;
	bIsTimerActive = false;

	ScenarioIndex = 1;
	StageIndex = 1;
	ScenarioLevel = 1;

	PrimaryActorTick.bCanEverTick = true;
}

void ALingoGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALingoGameState, RemainMissionTime);
	DOREPLIFETIME(ALingoGameState, bIsTimerActive);

	DOREPLIFETIME(ALingoGameState, ScenarioIndex);
	DOREPLIFETIME(ALingoGameState, StageIndex);
	DOREPLIFETIME(ALingoGameState, ScenarioLevel);
	DOREPLIFETIME(ALingoGameState, CurScenarioData);
}

void ALingoGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// 서버에서만 타이머 처리
	if (!HasAuthority() || !bIsTimerActive)
		return;

	RemainMissionTime -= DeltaSeconds;

	if (RemainMissionTime <= 0.f)
	{
		RemainMissionTime = 0.f;
		OnMissionTimerEnd();
	}
}

void ALingoGameState::SetStageData(int InStageIndex, const FResponseScenario& InResponseData)
{
	this->StageIndex = InStageIndex;
	this->CurScenarioData = InResponseData;
	// 미션 타이머 시작
	this->StartMissionTimer( ULingoGameHelper::GetMissionPlayTime(ScenarioLevel) );
}

void ALingoGameState::StartMissionTimer(float TimeLimit)
{
	// 서버에서만 실행
	if (!HasAuthority())
		return;

	RemainMissionTime = TimeLimit;
	bIsTimerActive = true;

	PRINTLOG( TEXT("[GameState] Mission Timer Started - %.0f seconds"), TimeLimit);
}

void ALingoGameState::StopMissionTimer()
{
	if (!HasAuthority())
		return;

	bIsTimerActive = false;
	PRINTLOG( TEXT("[GameState] Mission Timer Stopped"));
}

void ALingoGameState::OnMissionTimerEnd()
{
	if (!HasAuthority())
		return;

	bIsTimerActive = false;
	auto EndMessage = ULingoGameHelper::GetStageEndMessage(StageIndex);

	PRINTLOG( TEXT("[GameState] Mission Timer Ended - Sending: %s"), *EndMessage);

	// 모든 플레이어에게 메시지 전송
	if (UWorld* World = GetWorld())
	{
		for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
		{
			if (APlayerController* PC = It->Get())
			{
				if (APlayerActor* PlayerActor = Cast<APlayerActor>(PC->GetPawn()))
				{
					PlayerActor->OnGameMessage(EndMessage);
				}
			}
		}
	}
}