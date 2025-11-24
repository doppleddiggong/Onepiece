// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "ALingoPlayerState.h"

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