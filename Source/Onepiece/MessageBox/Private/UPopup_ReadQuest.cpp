// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "UPopup_ReadQuest.h"
#include "ALingoGameState.h"
#include "ALingoPlayerState.h"
#include "APlayerActor.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerController.h"
#include "GameLogging.h"
#include "UBroadcastManager.h"
#include "UKLingoNetworkSystem.h"
#include "ULingoGameHelper.h"
#include "UPopupManager.h"
#include "UTextureButton.h"
#include "UWordWidget.h"

void UPopup_ReadQuest::NativeConstruct()
{
	Super::NativeConstruct();
}

void UPopup_ReadQuest::InitPopup(const FResponseScenario& InScenarioData)
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

	this->ScenarioData = InScenarioData;

	if ( const auto PS = ULingoGameHelper::GetLingoPlayerState(GetWorld()) )
		InitQuestInfo(PS->QuestRole);
}

void UPopup_ReadQuest::InitQuestInfo(EQuestRole QuestRole)
{
	if ( QuestRole == EQuestRole::Both )
	{
		WordWidget->InitWordData(ScenarioData.full_data);
		ListenAudio(ScenarioData.full_data.Kor);
	}
	else if ( QuestRole == EQuestRole::OnlyQuestion1 )
	{
		WordWidget->InitWordData(ScenarioData.word_data1);
		ListenAudio(ScenarioData.word_data1.Kor);
	}
	else if ( QuestRole == EQuestRole::OnlyQuestion2 )
	{
		WordWidget->InitWordData(ScenarioData.word_data2);
		ListenAudio(ScenarioData.word_data2.Kor);
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