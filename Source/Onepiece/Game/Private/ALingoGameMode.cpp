// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "ALingoGameMode.h"
#include "ALingoGameState.h"
#include "ALingoPlayerState.h"
#include "TimerManager.h"

ALingoGameMode::ALingoGameMode()
{
	// GameState와 PlayerState 클래스 설정
	GameStateClass = ALingoGameState::StaticClass();
	PlayerStateClass = ALingoPlayerState::StaticClass();

	// Tick 비활성화 (Timer 기반으로 동작)
	PrimaryActorTick.bCanEverTick = false;
}