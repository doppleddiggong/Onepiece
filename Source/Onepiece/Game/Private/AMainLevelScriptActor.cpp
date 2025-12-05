// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "AMainLevelScriptActor.h"

#include "GameLogging.h"
#include "NetworkData.h"
#include "UKLingoNetworkSystem.h"
#include "UPopupManager.h"
#include "UPopup_Interview.h"


AMainLevelScriptActor::AMainLevelScriptActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AMainLevelScriptActor::BeginPlay()
{
	Super::BeginPlay();

	this->RequestInterviewHello();
}

void AMainLevelScriptActor::RequestInterviewHello()
{
	if (auto KLingoNetwork = UKLingoNetworkSystem::Get(GetWorld()))
	{
		KLingoNetwork->RequestInterviewHello( FResponseInterviewHelloDelegate::CreateUObject(this, &AMainLevelScriptActor::OnResponseInterviewHello) );
	}
	else
	{
		PRINTLOG(TEXT("UKLingoNetworkSystem not found!"));
	}
}

void AMainLevelScriptActor::OnResponseInterviewHello(FResponseInterviewHello& ResponseData, bool bWasSuccessful)
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