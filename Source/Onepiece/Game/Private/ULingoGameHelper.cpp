// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "ULingoGameHelper.h"

#include "ALingoGameState.h"
#include "ALingoPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Onepiece/Onepiece.h"

// GameState 가져오기
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

FString ULingoGameHelper::GetFormatTimer(const float InRemainTime )
{
	const int32 Minutes = FMath::FloorToInt(InRemainTime / 60.f);
	const int32 Seconds = FMath::FloorToInt(InRemainTime) % 60;

	return FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
}