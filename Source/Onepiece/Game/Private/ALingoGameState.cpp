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
#include "IMediaControls.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

ALingoGameState::ALingoGameState()
{
	RemainMissionTime = 0.f;
	bIsTimerActive = false;

	PrimaryActorTick.bCanEverTick = true;

	// TODO, Host가 방 개설후에 들어올 떄 적용하기
	RoomId = ULingoGameHelper::GetUnixTimestampInt64();
}

void ALingoGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALingoGameState, RemainMissionTime);
	DOREPLIFETIME(ALingoGameState, bIsTimerActive);

	DOREPLIFETIME(ALingoGameState, CurrentQuestData);

	DOREPLIFETIME(ALingoGameState, ReadScenarioData);
	DOREPLIFETIME(ALingoGameState, WrongReadAnswerList);
	DOREPLIFETIME(ALingoGameState, ReadResult);
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

void ALingoGameState::SetReadScenarioData( const FResponseReadScenario& InResponseData)
{
	CurrentQuestData.QuestType = EQuestType::Read;
	ReadScenarioData = InResponseData;
	
	// 미션 타이머 시작
	this->StartMissionTimer( ULingoGameHelper::GetMissionPlayTime() );

	if (HasAuthority())
		Multicast_ShowReadQuestPopup(ReadScenarioData);
}

void ALingoGameState::SetListenScenarioData( const FResponseListenScenario& InResponseData)
{
	CurrentQuestData.QuestType = EQuestType::Listen;
	ListenScenarioData = InResponseData;
	
	// 미션 타이머 시작
	this->StartMissionTimer( ULingoGameHelper::GetMissionPlayTime() );

	if (HasAuthority())
		Multicast_ShowListenQuestPopup(ListenScenarioData);
}

void ALingoGameState::StartMissionTimer(float InTimeLimit)
{
	// 서버에서만 실행
	if (!HasAuthority())
		return;

	this->TimeLimit = InTimeLimit;
	this->RemainMissionTime = InTimeLimit;
	this->bIsTimerActive = true;

	if (ANetworkBroadcastActor* BroadcastActor = ANetworkBroadcastActor::Get(this))
		BroadcastActor->SendUpdateMissionTimerState(true, InTimeLimit, this);
}

void ALingoGameState::StopMissionTimer()
{
	if (!HasAuthority())
		return;

	bIsTimerActive = false;

	// NetworkBroadcastActor를 통해 모든 클라이언트에 타이머 중지 알림
	if (ANetworkBroadcastActor* BroadcastActor = ANetworkBroadcastActor::Get(this))
	{
		BroadcastActor->SendUpdateMissionTimerState(false, 0.0f, this);
	}
}

void ALingoGameState::DecreaseMissionTimer(const float InValue)
{
	this->RemainMissionTime = FMath::Max(0.f, RemainMissionTime - InValue);
}

void ALingoGameState::UpdateRemainMissionTime(const float InTimeLimit)
{
	this->TimeLimit = InTimeLimit;
	this->RemainMissionTime = InTimeLimit;
}

void ALingoGameState::OnMissionTimerEnd()
{
	if (!HasAuthority())
		return;

	bIsTimerActive = false;

	// NetworkBroadcastActor를 통해 모든 클라이언트에 타이머 종료 알림
	if (ANetworkBroadcastActor* BroadcastActor = ANetworkBroadcastActor::Get(this))
		BroadcastActor->SendUpdateMissionTimerState(false, 0.0f, this);

	auto EndMessage = ULingoGameHelper::GetStageEndMessage(CurrentQuestData.QuestType);

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

void ALingoGameState::Multicast_ShowReadQuestPopup_Implementation(const FResponseReadScenario& InScenarioData)
{
	if (const auto Popup = UPopupManager::ShowPopupAs<UPopup_ReadQuest>(GetWorld(), EPopupType::ReadQuest))
	{
		Popup->InitRead(InScenarioData);
	}
}

void ALingoGameState::Multicast_ShowListenQuestPopup_Implementation(const FResponseListenScenario& InScenarioData)
{
	if (const auto Popup = UPopupManager::ShowPopupAs<UPopup_ReadQuest>(GetWorld(), EPopupType::ReadQuest))
	{
		Popup->InitListen(InScenarioData);
	}
}