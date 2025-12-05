// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "AMainLevelScriptActor.generated.h"

struct FResponseInterviewHello;

UCLASS()
class ONEPIECE_API AMainLevelScriptActor : public ALevelScriptActor
{
	GENERATED_BODY()

public:
	AMainLevelScriptActor();

protected:
	virtual void BeginPlay() override;

private:
	void RequestInterviewHello();
	void OnResponseInterviewHello(FResponseInterviewHello& ResponseData, bool bWasSuccessful);;
};
