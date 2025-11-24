// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file UNetworkBroadcastLibrary.cpp
 * @brief Blueprint에서 NetworkBroadcastActor를 쉽게 사용하기 위한 Function Library 구현
 */

#include "UNetworkBroadcastLibrary.h"
#include "ANetworkBroadcastActor.h"
#include "GameLogging.h"

ANetworkBroadcastActor* UNetworkBroadcastLibrary::GetNetworkBroadcastActor(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		PRINTLOG(TEXT("NetworkBroadcastLibrary: WorldContextObject is null"));
		return nullptr;
	}

	return ANetworkBroadcastActor::Get(WorldContextObject);
}
