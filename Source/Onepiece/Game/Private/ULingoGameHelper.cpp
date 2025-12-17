// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "ULingoGameHelper.h"

#include "ALingoGameMode.h"
#include "ALingoGameState.h"
#include "ALingoPlayerState.h"
#include "APlayerActor.h"
#include "APlayerControl.h"
#include "ASpeakStageActor.h"
#include "AWheatly.h"
#include "FColorStyleData.h"
#include "FResourceTextureData.h"
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

bool ULingoGameHelper::IsLocalPlayerPawn(const UObject* WorldContextObject)
{
	const UWorld* World = WorldContextObject->GetWorld();
	if (!World)
		return false;

	const APlayerController* PC = World->GetFirstPlayerController();
	if (!PC)
		return false;

	const APawn* Pawn = PC->GetPawn();
	return Pawn && Pawn->IsLocallyControlled();
}

APawn* ULingoGameHelper::GetLocalPawn(const UObject* WorldContextObject)
{
	if (const UWorld* World = WorldContextObject->GetWorld())
	{
		if (APlayerController* PC = World->GetFirstPlayerController())
		{
			APawn* Pawn = PC->GetPawn();
			if (Pawn && Pawn->IsLocallyControlled())
			{
				return Pawn;
			}
		}
	}
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
	const int32 TotalMilliseconds = FMath::FloorToInt(InRemainTime * 1000.f);

	const int32 Minutes       = TotalMilliseconds / 60000;
	const int32 Seconds       = (TotalMilliseconds / 1000) % 60;
	const int32 Milliseconds  = (TotalMilliseconds % 1000) / 10;

	// 00:00.00
	return FString::Printf(TEXT("%02d:%02d.%02d"), Minutes, Seconds, Milliseconds);
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

ASpeakStageActor* ULingoGameHelper::GetSpeakStageActor(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
		return nullptr;

	UWorld* World = WorldContextObject->GetWorld();
	if (!World)
		return nullptr;

	return Cast<ASpeakStageActor>(UGameplayStatics::GetActorOfClass(World, ASpeakStageActor::StaticClass()));
}

AWheatly* ULingoGameHelper::GetWheatly(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
		return nullptr;

	UWorld* World = WorldContextObject->GetWorld();
	if (!World)
		return nullptr;

	return Cast<AWheatly>(UGameplayStatics::GetActorOfClass(World, AWheatly::StaticClass()));
}

FString ULingoGameHelper::GetPlayerNameFromState(const ALingoPlayerState* PlayerState)
{
	if (!PlayerState)
		return TEXT("");

	// PlayerState의 Owner(PlayerController)를 통해 UserInfo 이름 가져오기
	if (APlayerController* PC = Cast<APlayerController>(PlayerState->GetOwner()))
	{
		if (APlayerControl* PlayerControl = Cast<APlayerControl>(PC))
		{
			FString UserName = PlayerControl->GetUserName();
			if (!UserName.IsEmpty())
				return UserName;
		}
	}

	// Fallback: PlayerState의 기본 이름
	return PlayerState->GetPlayerName();
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

FLinearColor ULingoGameHelper::GetRankColor(float InScore)
{
	if (InScore >= 90.0f)
	{
		// 1. Excellent (90 ~ 100): Green - 성공
		return FLinearColor::Green; 
	}
	else if (InScore >= 80.0f)
	{
	// 2. Good (80 ~ 89): LightGreen - 양호
		return FLinearColor(0.56f, 1.0f, 0.56f, 1.0f); 
	}
	else if (InScore >= 60.0f)
	{
		// 3. Average (60 ~ 79): Yellow - 주의
		return FLinearColor::Yellow;
	}
	else if (InScore >= 40.0f)
	{
	// 4. Poor (40 ~ 59): Orange - 경고
		return FLinearColor(1.0f, 0.5f, 0.0f, 1.0f); 
	}
	else 
	{
		// 5. Fail (0 ~ 39): Red - 심각한 실패
		return FLinearColor::Red;
	}
}

EColorStyleType ULingoGameHelper::GetRankColorType(float InScore)
{
	// 안정성 확보를 위해 입력값 범위 클램프 (0.0f ~ 100.0f)
	InScore = FMath::Clamp(InScore, 0.0f, 100.0f);

	if (InScore >= 90.0f)
	{
		// 1. Excellent (90 ~ 100): Green - 성공
		return EColorStyleType::Green; 
	}
	else if (InScore >= 80.0f)
	{
		// 2. Good (80 ~ 89): LightGreen - 양호
		return EColorStyleType::LightGreen; 
	}
	else if (InScore >= 60.0f)
	{
		// 3. Average (60 ~ 79): Yellow - 주의
		return EColorStyleType::Yellow;
	}
	else if (InScore >= 40.0f)
	{
		// 4. Poor (40 ~ 59): Orange - 경고
		return EColorStyleType::Orange;
	}
	else 
	{
		// 5. Fail (0 ~ 39): Red - 심각한 실패
		return EColorStyleType::Red;
	}
}


FString ULingoGameHelper::GetAccuracyPercentage(int WrongCnt)
{
	// 정답률 계산
	const float Percentage = ((10.f - WrongCnt) / 10.f) * 100.f;
	const int32 RoundedPercentage = FMath::RoundToInt(Percentage);
	
	// FString으로 변환
	return FString::Printf(TEXT("%d%%"), RoundedPercentage);
}
