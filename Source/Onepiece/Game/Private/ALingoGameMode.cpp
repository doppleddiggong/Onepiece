// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "ALingoGameMode.h"
#include "ALingoGameState.h"
#include "ALingoPlayerState.h"
#include "luggage.h"
#include "UBroadcastManager.h"
#include "GameLogging.h"
#include "TimerManager.h"
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

//--------------------------------------------------------------//
// Read Quest Implementation
//--------------------------------------------------------------//

void ALingoGameMode::StartReadQuest()
{
	if (!HasAuthority())
		return;

	ALingoGameState* LingoGameState = GetGameState<ALingoGameState>();
	if (!LingoGameState)
	{
		PRINTLOG(TEXT("[GameMode] StartReadQuest - GameState is null"));
		return;
	}

	// 플레이어 수 확인 (싱글/멀티 판정)
	int32 PlayerCount = 0;
	TArray<ALingoPlayerState*> LingoPlayerStates;

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (APlayerController* PC = It->Get())
		{
			if (ALingoPlayerState* PS = PC->GetPlayerState<ALingoPlayerState>())
			{
				LingoPlayerStates.Add(PS);
				PlayerCount++;
			}
		}
	}

	PRINTLOG(TEXT("[GameMode] StartReadQuest - Player count: %d"), PlayerCount);

	// 역할 할당
	if (PlayerCount == 1)
	{
		// 싱글 플레이: Both
		if (LingoPlayerStates.Num() > 0)
		{
			LingoPlayerStates[0]->QuestRole = EReadQuestRole::Both;
			PRINTLOG(TEXT("[GameMode] Player 0 assigned role: Both"));
		}
	}
	else if (PlayerCount >= 2)
	{
		// 멀티 플레이: OnlyQuestion1, OnlyQuestion2
		if (LingoPlayerStates.Num() >= 1)
		{
			LingoPlayerStates[0]->QuestRole = EReadQuestRole::OnlyQuestion1;
			PRINTLOG(TEXT("[GameMode] Player 0 assigned role: OnlyQuestion1"));
		}
		if (LingoPlayerStates.Num() >= 2)
		{
			LingoPlayerStates[1]->QuestRole = EReadQuestRole::OnlyQuestion2;
			PRINTLOG(TEXT("[GameMode] Player 1 assigned role: OnlyQuestion2"));
		}
	}

	// BroadcastManager를 통해 퀘스트 시작 이벤트 브로드캐스트
	if (UBroadcastManager* BroadcastManager = UBroadcastManager::Get(GetWorld()))
	{
		// 추후 BroadcastManager에 퀘스트 시작 이벤트 추가 필요
		PRINTLOG(TEXT("[GameMode] StartReadQuest - Quest started"));
	}
}

void ALingoGameMode::HandleCarrierSelection(APlayerState* Player, Aluggage* Carrier)
{
	if (!HasAuthority())
		return;

	if (!Player || !Carrier)
	{
		PRINTLOG(TEXT("[GameMode] HandleCarrierSelection - Invalid parameters"));
		return;
	}

	ALingoPlayerState* LingoPlayerState = Cast<ALingoPlayerState>(Player);
	if (!LingoPlayerState)
	{
		PRINTLOG(TEXT("[GameMode] HandleCarrierSelection - PlayerState is not ALingoPlayerState"));
		return;
	}

	// 정답 판정
	bool bIsCorrect = ValidateAnswer(LingoPlayerState, Carrier);

	if (bIsCorrect)
	{
		HandleCorrectAnswer(LingoPlayerState);
	}
	else
	{
		// 틀린 항목 확인
		bool bSymbolCorrect = (LingoPlayerState->SelectedSymbol == Carrier->Target1);
		bool bColorCorrect = (LingoPlayerState->SelectedColor == Carrier->Target2);

		HandleWrongAnswer(LingoPlayerState, bSymbolCorrect, bColorCorrect);
	}
}

bool ALingoGameMode::ValidateAnswer(ALingoPlayerState* Player, Aluggage* Carrier)
{
	if (!Player || !Carrier)
		return false;

	bool bSymbolCorrect = (Player->SelectedSymbol == Carrier->Target1);
	bool bColorCorrect = (Player->SelectedColor == Carrier->Target2);

	PRINTLOG(TEXT("[GameMode] ValidateAnswer - Symbol: %s vs %s (%s), Color: %s vs %s (%s)"),
		*Player->SelectedSymbol, *Carrier->Target1, bSymbolCorrect ? TEXT("Correct") : TEXT("Wrong"),
		*Player->SelectedColor, *Carrier->Target2, bColorCorrect ? TEXT("Correct") : TEXT("Wrong"));

	return bSymbolCorrect && bColorCorrect;
}

void ALingoGameMode::HandleCorrectAnswer(ALingoPlayerState* Player)
{
	if (!HasAuthority() || !Player)
		return;

	ALingoGameState* LingoGameState = GetGameState<ALingoGameState>();
	if (!LingoGameState)
		return;

	// QuestResult 업데이트
	LingoGameState->QuestResult.bSuccess = true;
	LingoGameState->QuestResult.RemainTime = LingoGameState->GetRemainMissionTime();
	LingoGameState->QuestResult.AttemptCount = Player->AttemptCount;
	LingoGameState->QuestResult.SelectedSymbol = Player->SelectedSymbol;
	LingoGameState->QuestResult.SelectedColor = Player->SelectedColor;

	// 성공 플래그 설정
	LingoGameState->bQuestSuccess = true;

	// 타이머 중지
	LingoGameState->StopMissionTimer();

	PRINTLOG(TEXT("[GameMode] HandleCorrectAnswer - Quest completed successfully"));

	// BroadcastManager를 통해 성공 이벤트 브로드캐스트
	if (UBroadcastManager* BroadcastManager = UBroadcastManager::Get(GetWorld()))
	{
		// 추후 BroadcastManager에 퀘스트 성공 이벤트 추가 필요
	}
}

void ALingoGameMode::HandleWrongAnswer(ALingoPlayerState* Player, bool bSymbolCorrect, bool bColorCorrect)
{
	if (!HasAuthority() || !Player)
		return;

	ALingoGameState* LingoGameState = GetGameState<ALingoGameState>();
	if (!LingoGameState)
		return;

	// 타이머 패널티 (30초 감소)
	const float Penalty = 30.f;
	LingoGameState->RemainMissionTime = FMath::Max(0.f, LingoGameState->RemainMissionTime - Penalty);

	PRINTLOG(TEXT("[GameMode] HandleWrongAnswer - Penalty applied: %.0f seconds, Remaining: %.0f seconds"),
		Penalty, LingoGameState->RemainMissionTime);

	// 틀린 항목 판정
	if (!bSymbolCorrect)
	{
		Player->SelectedSymbol = TEXT("");
		Player->bSymbolWrong = true;
		PRINTLOG(TEXT("[GameMode] Symbol was wrong - Cleared selection"));
	}

	if (!bColorCorrect)
	{
		Player->SelectedColor = TEXT("");
		Player->bColorWrong = true;
		PRINTLOG(TEXT("[GameMode] Color was wrong - Cleared selection"));
	}

	// 시도 횟수 증가
	Player->AttemptCount++;

	PRINTLOG(TEXT("[GameMode] HandleWrongAnswer - Attempt count: %d"), Player->AttemptCount);
}