// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "ULingoGameHelper.h"
#include "ALingoPlayerState.h"

ALingoPlayerState* ULingoGameHelper::GetLingoPlayerState(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
		return nullptr;

	UWorld* World = WorldContextObject->GetWorld();
	if (!World)
		return nullptr;

	APlayerController* PC = World->GetFirstPlayerController();
	if (!PC)
		return nullptr;

	return Cast<ALingoPlayerState>(PC->PlayerState);
}