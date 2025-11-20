// Fill out your copyright notice in the Description page of Project Settings.


#include "ALingoGameState.h"

#include "Net/UnrealNetwork.h"

ALingoGameState::ALingoGameState()
{
	CurrentPhase = EGamePhase::WaitingToStart;
	CurrentMissionIndex = 0;
	MissionTimeRemaining = 0.f;
	MissionTimeLimit = 300.f;
}

void ALingoGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALingoGameState, CurrentPhase);
	DOREPLIFETIME(ALingoGameState, CurrentMissionIndex);
	DOREPLIFETIME(ALingoGameState, MissionTimeRemaining);
	DOREPLIFETIME(ALingoGameState, MissionTimeLimit);
}

void ALingoGameState::OnRep_CurrentPhase()
{
	if (CurrentPhase != PreviousPhase)
	{
		UE_LOG(LogTemp, Warning, TEXT("[HouseGameState] Phase changed from %s to %s"),
			*UEnum::GetValueAsString(PreviousPhase), *UEnum::GetValueAsString(CurrentPhase));

		OnPhaseChanged.Broadcast(CurrentPhase);
		PreviousPhase = CurrentPhase;
	}
}

bool ALingoGameState::AreAllPlayersMissionComplete() const
{
	if (PlayerArray.Num() == 0) return false;

	for (APlayerState* PS : PlayerArray)
	{
		ALingoPlayerState* HousePS = Cast<ALingoPlayerState>(PS);
		if (HousePS)
		{
			if (!HousePS->bCurrentMissionComplete)
			{
				return false;
			}
		}
	}
	
	return true;
}

FString ALingoGameState::GetFormattedTimer() const
{
	int32 Minutes = FMath::FloorToInt(MissionTimeRemaining / 60.f);
	int32 Seconds = FMath::FloorToInt(MissionTimeRemaining) % 60;

	return FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
}
