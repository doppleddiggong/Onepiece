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

FString ULingoGameHelper::GetStageStartMessage(const EQuestType QuestType)
{
	switch (QuestType)
	{
	case EQuestType::Read:		return GameMessage::ReadStageStart;
	case EQuestType::Listen:	return GameMessage::ListenStageStart;
	case EQuestType::Write:		return GameMessage::WriteStageStart;
	case EQuestType::Speak:		return GameMessage::SpeakStageStart;

	default: return "GameStart";
	}
}

FString ULingoGameHelper::GetStageEndMessage(const EQuestType QuestType)
{
	switch (QuestType)
	{
		case EQuestType::Read:	return GameMessage::ReadStageEnd;
		case EQuestType::Listen:	return GameMessage::ListenStageEnd;
		case EQuestType::Write: return GameMessage::WriteStageEnd;
		case EQuestType::Speak: return GameMessage::SpeakStageEnd;

		default: return "GameEnd";
	}
}

int32 ULingoGameHelper::GetStageTypeIndex(const EQuestType QuestType)
{
	switch (QuestType)
	{
		case EQuestType::Read: return 1;
		case EQuestType::Listen: return 2;
		case EQuestType::Write: return 3;
		case EQuestType::Speak: return 4;

	default:
		return 1;
	}
}

float ULingoGameHelper::GetMissionPlayTime()
{
	return 300;
}

EResourceTextureType ULingoGameHelper::ConvertGradeScore(const float Score)
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

EResourceTextureType ULingoGameHelper::ConvertGradeString(const FString& Grade)
{
	if (Grade == "D") return EResourceTextureType::Rarity_D;
	if (Grade == "C") return EResourceTextureType::Rarity_C;
	if (Grade == "B") return EResourceTextureType::Rarity_B;
	if (Grade == "A") return EResourceTextureType::Rarity_A;
	if (Grade == "S") return EResourceTextureType::Rarity_S;

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

	return Cast<APlayerControl>(World->GetFirstPlayerController());
}

FString ULingoGameHelper::GetTimeRank(float InTimeTaken)
{
	if (InTimeTaken <= 300)
		return "C";
	else if (InTimeTaken <= 240)
		return "B";
	else if (InTimeTaken <= 180)
		return "A";
	else
		return "D";
}

FString ULingoGameHelper::GetAccuracyPercentage(int WrongCnt)
{
	// 정답률 계산
	const float Percentage = ((10.f - WrongCnt) / 10.f) * 100.f;
	const int32 RoundedPercentage = FMath::RoundToInt(Percentage);
	
	// FString으로 변환
	return FString::Printf(TEXT("%d%%"), RoundedPercentage);
}
