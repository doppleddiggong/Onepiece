// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "ALobbyLevelScriptActor.h"

#include "FComponentHelper.h"
#include "GameLogging.h"
#include "LobbyWidget.h"
#include "UKLingoNetworkSystem.h"
#include "UPopupManager.h"
#include "UPopup_Interview.h"
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

		this->RequestInterviewHello();
	}
}

void ALobbyLevelScriptActor::RequestInterviewHello()
{
	if (auto KLingoNetwork = UKLingoNetworkSystem::Get(GetWorld()))
	{
		KLingoNetwork->RequestInterviewHello( FResponseInterviewHelloDelegate::CreateUObject(this, &ALobbyLevelScriptActor::OnResponseInterviewHello) );
	}
	else
	{
		PRINTLOG(TEXT("UKLingoNetworkSystem not found!"));
	}
}

void ALobbyLevelScriptActor::OnResponseInterviewHello(FResponseInterviewHello& ResponseData, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if (const auto PopupMgr = UPopupManager::Get(GetWorld()))
			if (const auto Popup = Cast<UPopup_Interview>(PopupMgr->ShowPopup(EPopupType::Interview)))
				Popup->InitPopup(ResponseData);
	}
	else
	{
		PRINTLOG(TEXT("--- InterviewHello Questions FAILED ---"));
	}
}
