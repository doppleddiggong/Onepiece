// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "AMainLevelScriptActor.h"

#include "GameLogging.h"
#include "NetworkData.h"
#include "UKLingoNetworkSystem.h"
#include "UPopupManager.h"
#include "UPopup_Interview.h"
#include "UPopup_InterviewHello.h"
#include "UPopup_LevelSelect.h"


AMainLevelScriptActor::AMainLevelScriptActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AMainLevelScriptActor::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, [this]()
	{
		StartProcess();
	}, 1.0f, false);
}

void AMainLevelScriptActor::StartProcess()
{
	// Host(서버)인지 확인
	if (HasAuthority())
	{
		// Host인 경우: 레벨 선택 팝업 표시
		if (auto Popup = UPopupManager::ShowPopupAs<UPopup_LevelSelect>(GetWorld(), EPopupType::LevelSelect))
		{
			// 팝업 초기화
			Popup->InitPopup();

			// 팝업이 닫힐 때 RequestInterviewHello 호출
			Popup->OnPopupClosed.BindLambda([this]()
			{
				this->RequestInterviewHello();
			});
		}
		else
		{
			PRINTLOG(TEXT("Failed to show LevelSelect popup"));
		}
	}
	else
	{
		// Host가 아닌 경우: 바로 RequestInterviewHello 호출
		this->RequestInterviewHello();
	}
}

void AMainLevelScriptActor::RequestInterviewHello()
{
	// "오늘 보지 않기" 체크
	if (UPopup_InterviewHello::ShouldSkipInterviewToday(GetWorld()))
		return;

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
		if (auto Popup = UPopupManager::ShowPopupAs<UPopup_InterviewHello>(GetWorld(), EPopupType::InterviewHello))
			Popup->InitPopup(ResponseData);
	}
	else
	{
		PRINTLOG(TEXT("--- InterviewHello Questions FAILED ---"));
	}
}