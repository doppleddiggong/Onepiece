// Fill out your copyright notice in the Description page of Project Settings.


#include "ALingoPlayerState.h"
#include "ALingoGameMode.h"

#include "Net/UnrealNetwork.h"

ALingoPlayerState::ALingoPlayerState()
{
	PlayerRole = EPlayerRole::None;
	bCurrentMissionComplete = false;
	bIsOutOfOxygen = false;
}

void ALingoPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALingoPlayerState, PlayerRole);
	DOREPLIFETIME(ALingoPlayerState, bCurrentMissionComplete);
	DOREPLIFETIME(ALingoPlayerState, bIsOutOfOxygen);
}

void ALingoPlayerState::SetMissionComplete()
{
	if (HasAuthority())
	{
		bCurrentMissionComplete = true;

		// GameMode에 미션 완료 보고
		ALingoGameMode* GameMode = GetWorld()->GetAuthGameMode<ALingoGameMode>();
		if (GameMode)
		{
			APlayerController* PC = Cast<APlayerController>(GetOwner());
			if (PC)
			{
				GameMode->ReportMissionComplete(PC);
			}
		}
	}
}

void ALingoPlayerState::ResetForNextMission()
{
	if (HasAuthority())
	{
		bCurrentMissionComplete = false;
		UE_LOG(LogTemp, Log, TEXT("[PlayerState] reset for next mission"));
	}
}
