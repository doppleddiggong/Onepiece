// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file UBroadcastManger.cpp
 * @brief UBroadcastManger 구현에 대한 Doxygen 주석을 제공합니다.
 */

#include "UBroadcastManager.h"
#include "GameLogging.h"

void UBroadcastManager::SendMessage(const FString& InMsg)
{
	OnMessage.Broadcast(InMsg);
}

void UBroadcastManager::SendNetworkWaitCount(int RequestCount )
{
	OnNetworkWaitCount.Broadcast(RequestCount);
}

void UBroadcastManager::SendHitStop(AActor* Target, const EDamageType Type)
{
	OnHitStop.Broadcast(Target, Type);
}

void UBroadcastManager::SendHitStopPair(
	AActor* Attacker, const EDamageType AttackerType,
	AActor* Target,   const EDamageType TargetType)
{
	OnHitStop.Broadcast(Attacker, AttackerType);
	OnHitStop.Broadcast(Target,   TargetType);
}

void UBroadcastManager::SendKnockback(AActor* Target, AActor* Instigator, EDamageType Type, float Resistance)
{
	OnKnockback.Broadcast(Target, Instigator, Type, Resistance);
}

void UBroadcastManager::SendAudioSpectrum(float Spectrum)
{
	OnAudioSpectrum.Broadcast(Spectrum);
}

void UBroadcastManager::SendAudioCapture(bool bRecording)
{
	OnAudioCapture.Broadcast(bRecording);
}

void UBroadcastManager::SendDoorMessage(int InDoorIndex, bool InOpen, AActor* EventInstigator)
{
	OnDoorMessage.Broadcast(InDoorIndex, InOpen, EventInstigator);
}

void UBroadcastManager::SendWeightSwitch(int InButtonIndex, bool InActive)
{
	OnWeightSwitch.Broadcast(InButtonIndex, InActive);
}

void UBroadcastManager::SendUpdateMissionTimerState(bool bIsActive, float TimeLimit)
{
	OnUpdateMissionTimerState.Broadcast(bIsActive, TimeLimit);
}

void UBroadcastManager::SendUpdateQuestRole(EQuestRole QuestRole)
{
	OnUpdateQuestRole.Broadcast(QuestRole);
}

void UBroadcastManager::SendMovie(const int& InGroup, bool InState)
{
	OnMovie.Broadcast(InGroup, InState);
}

void UBroadcastManager::SendTutorMessage(const FText& Message)
{
	OnTutorMessage.Broadcast(Message);
}

void UBroadcastManager::SendAddItemToBoxList(const TArray<FResultStatData>& ItemDataList)
{
	OnAddItemToBoxList.Broadcast(ItemDataList);
}

void UBroadcastManager::SendTeleport(const FTransform& TargetTransform)
{
	OnTeleport.Broadcast(TargetTransform);
}

void UBroadcastManager::SendTutorialStepChanged(APlayerController* Player, ETutorialStep NewStep)
{
	OnTutorialStepChanged.Broadcast(Player, NewStep);
}

void UBroadcastManager::SendShowTutorialMessage(const FText& Message)
{
	OnShowTutorialMessage.Broadcast(Message);
}

void UBroadcastManager::SendHideTutorialMessage()
{
	OnHideTutorialMessage.Broadcast();
}
