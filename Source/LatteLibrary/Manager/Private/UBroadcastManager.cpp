// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file UBroadcastManger.cpp
 * @brief UBroadcastManger 구현에 대한 Doxygen 주석을 제공합니다.
 */

#include "UBroadcastManager.h"

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