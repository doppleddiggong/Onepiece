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

#include "NetworkData.h"
#include "ULingoGameHelper.h"

// Speak Stage System
#include "USpeakStageSubsystem.h"

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

void ALingoGameMode::BeginReadQuest(int32 InStageIndex, const FResponseScenario& InResponseData)
{
	if (!HasAuthority())
		return;

	// --- 1. 역할 할당 로직 ---
	auto PSList = ULingoGameHelper::GetLingoPlayerStateList(GetWorld());
	const int32 PlayerCount = PSList.Num();
	PRINTLOG(TEXT("[GameMode] BeginReadQuest - Player count: %d"), PlayerCount);

	if (PlayerCount == 1)
	{
		// 싱글 플레이: Both
		if (PSList.IsValidIndex(0))
		{
			PSList[0]->QuestRole = EQuestRole::Both;
			PRINTLOG(TEXT("[GameMode] Player 0 assigned role: Both"));
		}
	}
	else if (PlayerCount >= 2)
	{
		// 멀티 플레이: OnlyQuestion1, OnlyQuestion2
		if (PSList.IsValidIndex(0))
		{
			PSList[0]->QuestRole = EQuestRole::OnlyQuestion1;
			PRINTLOG(TEXT("[GameMode] Player 0 assigned role: OnlyQuestion1"));
		}
		if (PSList.IsValidIndex(1))
		{
			PSList[1]->QuestRole = EQuestRole::OnlyQuestion2;
			PRINTLOG(TEXT("[GameMode] Player 1 assigned role: OnlyQuestion2"));
		}
	}

	// --- 2. GameState에 데이터 설정 ---
	if (auto GS = GetGameState<ALingoGameState>())
	{
		GS->SetStageData(InStageIndex, 1, InResponseData);
		PRINTLOG(TEXT("[GameMode] SetStageData called on GameState."));
	}
	else
	{
		PRINTLOG(TEXT("[GameMode] BeginReadQuest - GameState is null"));
	}
}

void ALingoGameMode::BeginListenQuest(int32 InStageIndex, const FResponseScenario& InResponseData)
{
	if (auto GS = GetGameState<ALingoGameState>())
	{
		GS->SetStageData(InStageIndex, 2, InResponseData);
		PRINTLOG(TEXT("[GameMode] SetStageData called on GameState."));
	}
	else
	{
		PRINTLOG(TEXT("[GameMode] BeginListenQuest - GameState is null"));
	}
}

//--------------------------------------------------------------//
// Speak Quest Implementation
//--------------------------------------------------------------//

void ALingoGameMode::BeginSpeakQuest(int32 InStageIndex)
{
	if (!HasAuthority())
		return;

	// Subsystem에 위임
	if (USpeakStageSubsystem* Subsystem = GetWorld()->GetSubsystem<USpeakStageSubsystem>())
	{
		Subsystem->BeginSpeakQuest(InStageIndex);
	}
	else
	{
		PRINTLOG(TEXT("[LingoGameMode] BeginSpeakQuest - SpeakStageSubsystem not found"));
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
		bool bSymbolCorrect = (LingoPlayerState->SelectedWord1 == Carrier->Target1);
		bool bColorCorrect = (LingoPlayerState->SelectedWord2 == Carrier->Target2);

		HandleWrongAnswer(LingoPlayerState, bSymbolCorrect, bColorCorrect);
	}
}

bool ALingoGameMode::ValidateAnswer(ALingoPlayerState* Player, Aluggage* Carrier)
{
	if (!Player || !Carrier)
		return false;

	bool bSymbolCorrect = (Player->SelectedWord1 == Carrier->Target1);
	bool bColorCorrect = (Player->SelectedWord2 == Carrier->Target2);

	PRINTLOG(TEXT("[GameMode] ValidateAnswer - Symbol: %s vs %s (%s), Color: %s vs %s (%s)"),
		*Player->SelectedWord1, *Carrier->Target1, bSymbolCorrect ? TEXT("Correct") : TEXT("Wrong"),
		*Player->SelectedWord2, *Carrier->Target2, bColorCorrect ? TEXT("Correct") : TEXT("Wrong"));

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
	LingoGameState->QuestResult.SelectedSymbol = Player->SelectedWord1;
	LingoGameState->QuestResult.SelectedColor = Player->SelectedWord2;

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