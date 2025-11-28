// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Macro.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "ScoreManager.generated.h"

/**
 * 
 */
UCLASS()
class ONEPIECE_API UScoreManager : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	DEFINE_LOCALPLAYER_SUBSYSTEM_GETTER_INLINE(UScoreManager);

	UScoreManager();

	void GetTimeRank(float InTimeTaken, FString& Result);
};
