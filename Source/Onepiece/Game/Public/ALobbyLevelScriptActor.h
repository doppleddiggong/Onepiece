// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "ALobbyLevelScriptActor.generated.h"

UCLASS()
class ONEPIECE_API ALobbyLevelScriptActor : public ALevelScriptActor
{
	GENERATED_BODY()

public:
	ALobbyLevelScriptActor();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void RequestInterviewHello();
	UFUNCTION()
	void OnResponseInterviewHello(FResponseInterviewHello& ResponseData, bool bWasSuccessful);

private:
	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<class UUserWidget> LobbyWidgetClass;

	UPROPERTY()
	class ULobbyWidget* LobbyWidget;
};
