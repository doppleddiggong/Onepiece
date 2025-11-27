// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "ALingoGameState.h"

#include "Net/UnrealNetwork.h"
#include "APlayerActor.h"
#include "GameLogging.h"
#include "ULingoGameHelper.h"
#include "UBroadcastManager.h"
#include "UPopupManager.h"
#include "UPopup_MsgBox.h"
#include "UPopup_ReadQuest.h"
#include "ANetworkBroadcastActor.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

ALingoGameState::ALingoGameState()
{
	RemainMissionTime = 0.f;
	bIsTimerActive = false;

	GameState = EGameState::None;
	
	ScenarioIndex = 1;
	StageIndex = 0;
	ScenarioLevel = 1;

	PrimaryActorTick.bCanEverTick = true;
}

void ALingoGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALingoGameState, GameState);
	
	DOREPLIFETIME(ALingoGameState, RemainMissionTime);
	DOREPLIFETIME(ALingoGameState, bIsTimerActive);

	DOREPLIFETIME(ALingoGameState, ScenarioIndex);
	DOREPLIFETIME(ALingoGameState, StageIndex);
	DOREPLIFETIME(ALingoGameState, ScenarioLevel);
	DOREPLIFETIME(ALingoGameState, CurScenarioData);

	// Read Quest Data
	DOREPLIFETIME(ALingoGameState, QuestResult);
	DOREPLIFETIME(ALingoGameState, bQuestSuccess);
}

void ALingoGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// 서버에서만 타이머 처리
	if (!HasAuthority() || !bIsTimerActive)
		return;

	RemainMissionTime -= DeltaSeconds;

	if (RemainMissionTime <= 0.f)
	{
		RemainMissionTime = 0.f;
		OnMissionTimerEnd();
	}
}

void ALingoGameState::SetStageData(int InStageIndex, const FResponseScenario& InResponseData)
{
	PRINTLOG(TEXT("[GameState] SetStageData - StageIndex: %d, HasAuthority: %s"), 
		InStageIndex, HasAuthority() ? TEXT("true") : TEXT("false"));
	
	if ( InStageIndex == 1)
		GameState = EGameState::Stage1;
	else if ( InStageIndex == 2)
		GameState = EGameState::Stage2;
	else if ( InStageIndex == 3)
		GameState = EGameState::Stage3;
	else if ( InStageIndex == 4)
		GameState = EGameState::Stage4;
	
	PRINTLOG(TEXT("[GameState] GameState changed to: %d"), static_cast<int32>(GameState));
	
	this->StageIndex = InStageIndex;
	this->CurScenarioData = InResponseData;
	// 미션 타이머 시작
	this->StartMissionTimer( ULingoGameHelper::GetMissionPlayTime(ScenarioLevel) );

	// 모든 클라이언트(서버 포함)에 팝업 표시 요청
	if (HasAuthority())
	{
		Multicast_ShowReadQuestPopup(InStageIndex, InResponseData);

		// 모든 클라이언트에게 스테이지 시작 브로드캐스트
		if (ANetworkBroadcastActor* BroadcastActor = ANetworkBroadcastActor::Get(this))
		{
			BroadcastActor->SendStageStarted(InStageIndex, this);
			PRINTLOG(TEXT("[GameState] Broadcasting StageStarted - StageIndex: %d"), InStageIndex);
		}
	}
}

void ALingoGameState::StartMissionTimer(float TimeLimit)
{
	// 서버에서만 실행
	if (!HasAuthority())
		return;

	RemainMissionTime = TimeLimit;
	bIsTimerActive = true;

	// BroadcastManager를 통해 타이머 시작 알림
	if (UBroadcastManager* BroadcastManager = UBroadcastManager::Get(GetWorld()))
	{
		BroadcastManager->SendMissionTimerStateChanged(true);
	}

	PRINTLOG( TEXT("[GameState] Mission Timer Started - %.0f seconds"), TimeLimit);
}

void ALingoGameState::StopMissionTimer()
{
	if (!HasAuthority())
		return;

	bIsTimerActive = false;

	// BroadcastManager를 통해 타이머 중지 알림
	if (UBroadcastManager* BroadcastManager = UBroadcastManager::Get(GetWorld()))
	{
		BroadcastManager->SendMissionTimerStateChanged(false);
	}

	PRINTLOG( TEXT("[GameState] Mission Timer Stopped"));
}

void ALingoGameState::OnMissionTimerEnd()
{
	if (!HasAuthority())
		return;

	bIsTimerActive = false;

	// BroadcastManager를 통해 타이머 종료 알림
	if (UBroadcastManager* BroadcastManager = UBroadcastManager::Get(GetWorld()))
	{
		BroadcastManager->SendMissionTimerStateChanged(false);
	}

	auto EndMessage = ULingoGameHelper::GetStageEndMessage(StageIndex);

	PRINTLOG( TEXT("[GameState] Mission Timer Ended - Sending: %s"), *EndMessage);

	// 모든 플레이어에게 메시지 전송
	if (UWorld* World = GetWorld())
	{
		for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
		{
			if (APlayerController* PC = It->Get())
			{
				if (APlayerActor* PlayerActor = Cast<APlayerActor>(PC->GetPawn()))
				{
					PlayerActor->OnGameMessage(EndMessage);
				}
			}
		}
	}
}

void ALingoGameState::OnRep_QuestSuccess()
{
	// BroadcastManager를 통해 퀘스트 성공 이벤트 브로드캐스트
	if (UBroadcastManager* BroadcastManager = UBroadcastManager::Get(GetWorld()))
	{
		if (bQuestSuccess)
		{
			UPopupManager::Get(GetWorld())->ShowMsgBoxSimple(
				TEXT("Success"),
				TEXT("Quest Clear"),
				EMsgBoxType::OK);
		}
	}
}

void ALingoGameState::Multicast_ShowReadQuestPopup_Implementation(int InStageIndex, const FResponseScenario& InScenarioData)
{
	PRINTLOG(TEXT("[GameState] Multicast_ShowReadQuestPopup - StageIndex: %d, Role: %s"),
		InStageIndex, GetLocalRole() == ROLE_Authority ? TEXT("Server") : TEXT("Client"));

	// Stage1일 때만 Read Quest 팝업 표시
	if (InStageIndex == 1)
	{
		if (UPopupManager* PopupMgr = UPopupManager::Get(GetWorld()))
		{
			if (UPopup_ReadQuest* Popup = Cast<UPopup_ReadQuest>(PopupMgr->ShowPopup(EPopupType::ReadQuest)))
			{
				Popup->InitPopup(InScenarioData);
				PRINTLOG(TEXT("[GameState] Read Quest Popup displayed for Stage1"));
			}
		}
	}
}