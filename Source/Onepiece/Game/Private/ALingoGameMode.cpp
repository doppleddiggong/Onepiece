// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "ALingoGameMode.h"
#include "ALingoGameState.h"
#include "ALingoPlayerState.h"
#include "APlayerControl.h"
#include "luggage.h"
#include "UBroadcastManager.h"
#include "GameLogging.h"
#include "TimerManager.h"
#include "TutorialComponent.h"
#include "ULingoGameHelper.h"

#include "Engine/World.h"
#include "GameFramework/PlayerState.h"

ALingoGameMode::ALingoGameMode()
{
	// GameState와 PlayerState 클래스 설정
	GameStateClass = ALingoGameState::StaticClass();
	PlayerStateClass = ALingoPlayerState::StaticClass();

	// Tick 비활성화 (Timer 기반으로 동작)
	PrimaryActorTick.bCanEverTick = false;
}

void ALingoGameMode::UpdateQuestRole()
{
	auto PSList = ULingoGameHelper::GetLingoPlayerStateList(GetWorld());
	const int32 PlayerCount = PSList.Num();

	if (PlayerCount == 1)
	{
		PSList[0]->QuestRole = EQuestRole::Both;
	}
	else if (PlayerCount >= 2)
	{
		PSList[0]->QuestRole = EQuestRole::OnlyQuestion1;
		PSList[1]->QuestRole = EQuestRole::OnlyQuestion2;
	}
}

void ALingoGameMode::BeginReadQuest( const FResponseReadScenario& InResponseData)
{
	if (!HasAuthority())
		return;

	UpdateQuestRole();

	if (auto GS = GetGameState<ALingoGameState>())
	{
		GS->SetReadScenarioData(InResponseData);

		GS->SetAllCompassVisibility(false);
		GS->SetCompassVisibilityByTag("ReadQuest", true);
	}
}

void ALingoGameMode::BeginListenQuest(const FResponseListenScenario& InResponseData)
{
	if (!HasAuthority())
		return;

	UpdateQuestRole();

	if (auto GS = GetGameState<ALingoGameState>())
	{
		GS->SetListenScenarioData(InResponseData);
		
		GS->SetAllCompassVisibility(false);
		GS->SetCompassVisibilityByTag("ListenQuest", true);
	}
}

void ALingoGameMode::HandleLuggageSelection(APlayerState* Player, Aluggage* luggage)
{
	if (!HasAuthority())
		return;

	if (!Player || !luggage)
		return;

	ALingoPlayerState* LingoPlayerState = Cast<ALingoPlayerState>(Player);
	if (!LingoPlayerState)
		return;

	// 정답 판정
	if ( ValidateAnswer(LingoPlayerState, luggage) )
	{
		HandleCorrectAnswer(LingoPlayerState);
	}
	else
	{
		// 틀린 항목 확인
		bool bSymbolCorrect = (LingoPlayerState->SelectedWord1 == luggage->Target1);
		bool bColorCorrect = (LingoPlayerState->SelectedWord2 == luggage->Target2);

		HandleWrongAnswer(LingoPlayerState, bSymbolCorrect, bColorCorrect);
	}
}

bool ALingoGameMode::ValidateAnswer(ALingoPlayerState* Player, Aluggage* Luggage)
{
	if (!Player || !Luggage)
		return false;

	const bool bSymbolCorrect = (Player->SelectedWord1 == Luggage->Target1);
	const bool bColorCorrect = (Player->SelectedWord2 == Luggage->Target2);

	PRINTLOG(TEXT("[GameMode] ValidateAnswer - Symbol: %s vs %s (%s), Color: %s vs %s (%s)"),
		*Player->SelectedWord1, *Luggage->Target1, bSymbolCorrect ? TEXT("Correct") : TEXT("Wrong"),
		*Player->SelectedWord2, *Luggage->Target2, bColorCorrect ? TEXT("Correct") : TEXT("Wrong"));

	return bSymbolCorrect && bColorCorrect;
}

void ALingoGameMode::HandleCorrectAnswer(ALingoPlayerState* Player)
{
	if (!HasAuthority() )
		return;

	if (!Player)
		return;

	ALingoGameState* LingoGameState = GetGameState<ALingoGameState>();
	if (!LingoGameState)
		return;

	// 타이머 중지
	LingoGameState->StopMissionTimer();
}

void ALingoGameMode::HandleWrongAnswer(ALingoPlayerState* Player, bool bSymbolCorrect, bool bColorCorrect)
{
	if (!HasAuthority())
		return;

	if (!Player)
		return;

	ALingoGameState* LingoGameState = GetGameState<ALingoGameState>();
	if (!LingoGameState)
		return;

	// 타이머 패널티
	constexpr float Penalty = 30.f;
	LingoGameState->DecreaseMissionTimer(Penalty);
	
	// 틀린 항목 판정
	if (!bSymbolCorrect)
	{
		Player->SelectedWord1 = TEXT("");
		Player->bWrongWord1 = true;
		PRINTLOG(TEXT("[GameMode] Symbol was wrong - Cleared selection"));
	}

	if (!bColorCorrect)
	{
		Player->SelectedWord2 = TEXT("");
		Player->bWrongWord2 = true;
		PRINTLOG(TEXT("[GameMode] Color was wrong - Cleared selection"));
	}

	// 시도 횟수 증가
	Player->AttemptCount++;

	PRINTLOG(TEXT("[GameMode] HandleWrongAnswer - Attempt count: %d"), Player->AttemptCount);
}