// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "UPopup_Word.h"

#include "APlayerActor.h"
#include "UDialogManager.h"
#include "UImageButton.h"
#include "UKLingoNetworkSystem.h"
#include "ULingoGameHelper.h"
#include "UPopupManager.h"
#include "UTextureButton.h"
#include "Components/TextBlock.h"

void UPopup_Word::InitPopup(const FPhonemeData& InData)
{
	Data = InData;
	
	if (Btn_Close)
	{
		Btn_Close->OnButtonClickedEvent.RemoveDynamic(this, &UPopup_Word::OnClickClose);
		Btn_Close->OnButtonClickedEvent.AddDynamic(this, &UPopup_Word::OnClickClose);
	}

	if (Btn_Repeat)
	{
		Btn_Repeat->OnButtonClickedEvent.RemoveDynamic(this, &UPopup_Word::OnClickRepeat);
		Btn_Repeat->OnButtonClickedEvent.AddDynamic(this, &UPopup_Word::OnClickRepeat);
	}
	
	Txt_Kor->SetText( FText::FromString(InData.Kor));
	Txt_Proneme->SetText( FText::FromString(InData.Pronunciation));


	OnClickRepeat();
}

void UPopup_Word::OnClickClose()
{
	if (UPopupManager* PopupMgr = UPopupManager::Get(GetWorld()))
	{
		PopupMgr->HideCurrentPopup();
	}
}

void UPopup_Word::OnClickRepeat()
{
	if (auto KLingoNetwork = UKLingoNetworkSystem::Get(GetWorld()))
	{
		KLingoNetwork->RequestListenAudio(
			Data.Kor,
			FResponseListenAudioDelegate::CreateUObject(this, &UPopup_Word::OnResponseListenAudio)
		);
	}
}

void UPopup_Word::OnResponseListenAudio(FResponseListenAudio& ResponseData, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if (auto PlayerActor = ULingoGameHelper::GetPlayerActor(this))
		{
			PlayerActor->PlayTTSAudio(ResponseData.audio_base64);
		}
	}
}