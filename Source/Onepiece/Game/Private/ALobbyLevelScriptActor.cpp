// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "ALobbyLevelScriptActor.h"

#include "FComponentHelper.h"
#include "LobbyWidget.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

#define LOBBYWIDGET_PATH TEXT("/Game/CustomContents/UI/WBP_Lobby.WBP_Lobby_C")

ALobbyLevelScriptActor::ALobbyLevelScriptActor()
{
	PrimaryActorTick.bCanEverTick = false;

	// LobbyWidget 클래스 자동 로드
	LobbyWidgetClass = FComponentHelper::LoadClass<ULobbyWidget>(LOBBYWIDGET_PATH);
}

void ALobbyLevelScriptActor::BeginPlay()
{
	Super::BeginPlay();

	if ( auto PC = UGameplayStatics::GetPlayerController(this, 0) )
	{
		if (LobbyWidgetClass)
		{
			LobbyWidget = CreateWidget<ULobbyWidget>(PC, LobbyWidgetClass);
			if (LobbyWidget)
				LobbyWidget->AddToViewport();
		}
	}
}