// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "ULingoGameHelper.h"

#include "ALingoGameMode.h"
#include "ALingoGameState.h"
#include "ALingoPlayerState.h"
#include "APlayerActor.h"
#include "APlayerControl.h"
#include "FResourceTextureData.h"
#include "USpeakStageSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Onepiece/Onepiece.h"


int64 ULingoGameHelper::GetUnixTimestampInt64()
{
	return FDateTime::UtcNow().ToUnixTimestamp();
}

int32 ULingoGameHelper::GetUserId(const UObject* WorldContextObject)
{
	if ( auto PC = GetPlayerControl(WorldContextObject) )
		return PC->GetUserId();
	return 0;
}

int ULingoGameHelper::GetMultiPlayerCount(const UObject* WorldContextObject)
{
	ALingoGameState* GS = Cast<ALingoGameState>(UGameplayStatics::GetGameState(WorldContextObject));
	int32 NumPlayers = GS ? GS->PlayerArray.Num() : 0;
	return NumPlayers;
}

bool ULingoGameHelper::IsMultiPlay(const UObject* WorldContextObject)
{
	return GetMultiPlayerCount(WorldContextObject) > 1;
}

ALingoGameMode* ULingoGameHelper::GetLingoGameMode(const UObject* WorldContextObject)
{
	return WorldContextObject->GetWorld()->GetAuthGameMode<ALingoGameMode>();	
}

ALingoGameState* ULingoGameHelper::GetLingoGameState(const UObject* WorldContextObject)
{
	return Cast<ALingoGameState>(UGameplayStatics::GetGameState(WorldContextObject));
}

ALingoPlayerState* ULingoGameHelper::GetLingoPlayerState(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
		return nullptr;

	UWorld* World = WorldContextObject->GetWorld();
	if (!World)
		return nullptr;

	APlayerController* PC = World->GetFirstPlayerController();
	if (!PC)
		return nullptr;

	return Cast<ALingoPlayerState>(PC->PlayerState);
}

ALingoPlayerState* ULingoGameHelper::GetLingoPlayerStateByPC(const AController* PC)
{
	if (PC)
	{
		return PC->GetPlayerState<ALingoPlayerState>();
	}
	else
	{
		return nullptr;
	}
}


TArray<ALingoPlayerState*> ULingoGameHelper::GetLingoPlayerStateList(const UObject* WorldContextObject)
{
	TArray<ALingoPlayerState*> PlayerStateList;
	for (FConstPlayerControllerIterator It = WorldContextObject->GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (APlayerController* PC = It->Get())
		{
			if (ALingoPlayerState* PS = PC->GetPlayerState<ALingoPlayerState>())
			{
				PlayerStateList.Add(PS);
			}
		}
	}

	return PlayerStateList;
}

ASpeakStageActor* ULingoGameHelper::GetSpeakStageActor(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
		return nullptr;

	UWorld* World = WorldContextObject->GetWorld();
	if (!World)
		return nullptr;

	USpeakStageSubsystem* SpeakSubsystem = World->GetSubsystem<USpeakStageSubsystem>();
	if (SpeakSubsystem && SpeakSubsystem->IsInitialized())
		return SpeakSubsystem->GetSpeakStage();

	return nullptr;
}

FString ULingoGameHelper::GetStageStartMessage(const int StageIndex)
{
	switch (StageIndex)
	{
	case 1:	return GameMessage::Stage1Start;
	case 2:	return GameMessage::Stage2Start;
	case 3: return GameMessage::Stage3Start;
	case 4: return GameMessage::Stage4Start;
	default: return GameMessage::GameStart;
	}
}

FString ULingoGameHelper::GetStageEndMessage(const int StageIndex)
{
	switch (StageIndex)
	{
	case 1:	return GameMessage::Stage1End;
	case 2:	return GameMessage::Stage2End;
	case 3: return GameMessage::Stage3End;
	case 4: return GameMessage::Stage4End;
	default: return GameMessage::GameEnd;
	}
}

float ULingoGameHelper::GetMissionPlayTime(const int Level)
{
	switch (Level)
	{
	case 1:	return 300;
	case 2:	return 240;
	case 3: return 180;
	default: return 180;
	}	
}

EResourceTextureType ULingoGameHelper::ConvertGradeType(const float Score)
{
	if (Score >= 90.0f)
		return EResourceTextureType::Rarity_S;
	else if (Score >= 80.0f)
		return EResourceTextureType::Rarity_A;
	else if (Score >= 70.0f)
		return EResourceTextureType::Rarity_B;
	else if (Score >= 60.0f)
		return EResourceTextureType::Rarity_C;
	else                                                                                                                                                                                                                          
		return EResourceTextureType::Rarity_D;
}


FString ULingoGameHelper::GetFormatTimer(const float InRemainTime )
{
	const int32 Minutes = FMath::FloorToInt(InRemainTime / 60.f);
	const int32 Seconds = FMath::FloorToInt(InRemainTime) % 60;

	return FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
}

void ULingoGameHelper::ShowMouseCursor(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
		return;

	UWorld* World = WorldContextObject->GetWorld();
	if (!World)
		return;

	APlayerController* PC = World->GetFirstPlayerController();
	if (!PC)
		return;

	PC->bShowMouseCursor = true;
	PC->SetInputMode(FInputModeGameAndUI());
}

void ULingoGameHelper::HideMouseCursor(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
		return;

	UWorld* World = WorldContextObject->GetWorld();
	if (!World)
		return;

	APlayerController* PC = World->GetFirstPlayerController();
	if (!PC)
		return;

	PC->bShowMouseCursor = false;
	PC->SetInputMode(FInputModeGameOnly());
}

APlayerActor* ULingoGameHelper::GetPlayerActor(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
		return nullptr;

	UWorld* World = WorldContextObject->GetWorld();
	if (!World)
		return nullptr;

	APlayerController* PC = World->GetFirstPlayerController();
	if (!PC)
		return nullptr;

	return Cast<APlayerActor>(PC->GetPawn());
}


APlayerControl* ULingoGameHelper::GetPlayerControl(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
		return nullptr;

	UWorld* World = WorldContextObject->GetWorld();
	if (!World)
		return nullptr;

	APlayerController* PC = World->GetFirstPlayerController();
	if (!PC)
		return nullptr;

	return Cast<APlayerControl>(PC->GetPawn());
}