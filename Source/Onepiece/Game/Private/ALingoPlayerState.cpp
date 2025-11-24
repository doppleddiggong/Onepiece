// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "ALingoPlayerState.h"
#include "UBroadcastManager.h"

ALingoPlayerState::ALingoPlayerState()
{
}

void ALingoPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ALingoPlayerState::SetToken(FString InToken)
{
	this->AccessToken = InToken;
}

void ALingoPlayerState::SetUserName(FString InUserName)
{
	this->UserName = InUserName;

	// BroadcastManager를 통해 모든 구독자에게 알림
	if (UBroadcastManager* BroadcastManager = UBroadcastManager::Get(GetWorld()))
	{
		BroadcastManager->SendUserNameChanged(UserName);
	}
}
	