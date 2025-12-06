// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "UPopup_ReadQuest.h"
#include "ALingoGameState.h"
#include "ALingoPlayerState.h"
#include "APlayerActor.h"
#include "UBroadcastManager.h"
#include "UKLingoNetworkSystem.h"
#include "ULingoGameHelper.h"
#include "UPopupManager.h"
#include "UTextureButton.h"
#include "UWordWidget.h"

#include "GameFramework/PlayerController.h"


void UPopup_ReadQuest::InitRead(const FResponseReadScenario& InScenarioData)
{
	if (Btn_Exit)
	{
		Btn_Exit->OnButtonClickedEvent.RemoveDynamic(this, &UPopup_ReadQuest::OnClickClose);
		Btn_Exit->OnButtonClickedEvent.AddDynamic(this, &UPopup_ReadQuest::OnClickClose);
	}

	if (auto BM = UBroadcastManager::Get(GetWorld()))
	{
		BM->OnUpdateQuestRole.RemoveDynamic(this, &UPopup_ReadQuest::InitQuestInfo);
		BM->OnUpdateQuestRole.AddDynamic(this, &UPopup_ReadQuest::InitQuestInfo);
	}

	this->QuestType = EQuestType::Read;
	this->ReadData = InScenarioData;

	if ( const auto PS = ULingoGameHelper::GetLingoPlayerState(GetWorld()) )
		InitQuestInfo(PS->QuestRole);
}


void UPopup_ReadQuest::InitListen(const FResponseListenScenario& InScenarioData)
{
	if (Btn_Exit)
	{
		Btn_Exit->OnButtonClickedEvent.RemoveDynamic(this, &UPopup_ReadQuest::OnClickClose);
		Btn_Exit->OnButtonClickedEvent.AddDynamic(this, &UPopup_ReadQuest::OnClickClose);
	}

	if (auto BM = UBroadcastManager::Get(GetWorld()))
	{
		BM->OnUpdateQuestRole.RemoveDynamic(this, &UPopup_ReadQuest::InitQuestInfo);
		BM->OnUpdateQuestRole.AddDynamic(this, &UPopup_ReadQuest::InitQuestInfo);
	}

	this->QuestType = EQuestType::Listen;
	this->ListenData = InScenarioData;

	if ( const auto PS = ULingoGameHelper::GetLingoPlayerState(GetWorld()) )
		InitQuestInfo(PS->QuestRole);
}

void UPopup_ReadQuest::InitQuestInfo(EQuestRole QuestRole)
{
	if ( QuestType == EQuestType::Read )
	{
		if ( QuestRole == EQuestRole::Both )
		{
			WordWidget->InitWordData(ReadData.full_data);
			ListenAudio(ReadData.full_data.Kor);
		}
		else if ( QuestRole == EQuestRole::OnlyQuestion1 )
		{
			WordWidget->InitWordData(ReadData.word_data1);
			ListenAudio(ReadData.word_data1.Kor);
		}
		else if ( QuestRole == EQuestRole::OnlyQuestion2 )
		{
			WordWidget->InitWordData(ReadData.word_data2);
			ListenAudio(ReadData.word_data2.Kor);
		}
	}
	else if ( QuestType == EQuestType::Listen )
	{
		if ( QuestRole == EQuestRole::Both )
		{
			WordWidget->InitWordData(ListenData.full_data);
			ListenAudio(ListenData.full_data.Kor);
		}
		else if ( QuestRole == EQuestRole::OnlyQuestion1 )
		{
			WordWidget->InitWordData(ListenData.word_data1);
			ListenAudio(ListenData.word_data1.Kor);
		}
		else if ( QuestRole == EQuestRole::OnlyQuestion2 )
		{
			WordWidget->InitWordData(ListenData.word_data2);
			ListenAudio(ListenData.word_data2.Kor);
		}
	}
}

void UPopup_ReadQuest::OnClickClose()
{
	if (UPopupManager* PopupMgr = UPopupManager::Get(GetWorld()))
	{
		PopupMgr->HideCurrentPopup();
	}
}

void UPopup_ReadQuest::ListenAudio(const FString& AudioText)
{
	if (bIsRequest)
		return;
	
	if (auto KLingoNetwork = UKLingoNetworkSystem::Get(GetWorld()))
	{
		bIsRequest = true;
		
		KLingoNetwork->RequestListenAudio(
			AudioText,
			FResponseListenAudioDelegate::CreateUObject(this, &UPopup_ReadQuest::OnResponseListenAudio)
		);
	}
}

void UPopup_ReadQuest::OnResponseListenAudio(FResponseListenAudio& ResponseData, bool bWasSuccessful)
{
	bIsRequest = false;
	
	if (bWasSuccessful)
	{
		if (auto PlayerActor = ULingoGameHelper::GetPlayerActor(this))
			PlayerActor->PlayTTSAudio(ResponseData.audio_base64);
	}
}