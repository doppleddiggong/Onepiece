// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "UPopup_SpeakQuest.h"

#include "APlayerActor.h"
#include "UImageButton.h"
#include "UKLingoNetworkSystem.h"
#include "ULingoGameHelper.h"
#include "UPopupManager.h"

void UPopup_SpeakQuest::InitPopup(const FOnMsgBoxOkDelegate& InOkDelegate)
{
	// 중복 바인딩 방지: 기존 바인딩 제거 후 재바인딩
	if (Btn_Ok)
	{
		Btn_Ok->OnButtonClickedEvent.RemoveDynamic(this, &UPopup_SpeakQuest::OnClickOk);
		Btn_Ok->OnButtonClickedEvent.AddDynamic(this, &UPopup_SpeakQuest::OnClickOk);
	}

	OnOkDelegate = InOkDelegate;


	RequestListenAudio( TEXT("Please answer the officer’s question.")); 
}

void UPopup_SpeakQuest::RequestListenAudio(const FString& AudioText)
{
	if (bIsRequest)
		return;

	if (auto KLingoNetwork = UKLingoNetworkSystem::Get(GetWorld()))
	{
		bIsRequest = true;

		KLingoNetwork->RequestListenAudio(
			AudioText,
			FResponseListenAudioDelegate::CreateUObject(this, &UPopup_SpeakQuest::OnResponseListenAudio)
		);
	}
}

void UPopup_SpeakQuest::OnResponseListenAudio(FResponseListenAudio& ResponseData, bool bWasSuccessful)
{
	bIsRequest = false;

	if (bWasSuccessful)
	{
		if (auto PlayerActor = ULingoGameHelper::GetPlayerActor(this))
			PlayerActor->PlayTTSAudio(ResponseData.audio_base64);
	}
}


void UPopup_SpeakQuest::OnClickOk()
{
	// PopupManager를 통해 팝업 닫기 (마우스 커서 처리 포함)
	if (UPopupManager* PopupMgr = UPopupManager::Get(GetWorld()))
	{
		PopupMgr->HideCurrentPopup();
	}

	if (OnOkDelegate.IsBound())
		OnOkDelegate.Execute();
}
