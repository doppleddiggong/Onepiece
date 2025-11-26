// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "UPopup_ReadQuest.h"
#include "ALingoGameState.h"
#include "ALingoPlayerState.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerController.h"
#include "GameLogging.h"
#include "ULingoGameHelper.h"
#include "UPopupManager.h"
#include "UTextureButton.h"
#include "UWordWidget.h"

class ALingoPlayerState;

void UPopup_ReadQuest::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Exit)
	{
		Btn_Exit->OnButtonClickedEvent.RemoveDynamic(this, &UPopup_ReadQuest::OnClickClose);
		Btn_Exit->OnButtonClickedEvent.AddDynamic(this, &UPopup_ReadQuest::OnClickClose);
	}
}

void UPopup_ReadQuest::InitPopup(const FResponseScenario& InScenarioData)
{
	PRINTLOG(TEXT("[ReadQuestWidget] InitPopup quest"));

	this->ScenarioData = InScenarioData;

	if ( const auto PS = ULingoGameHelper::GetLingoPlayerState(GetWorld()) )
	{
		if ( PS->QuestRole == EReadQuestRole::Both )
		{
			WordWidget->InitWordData(InScenarioData.full_data);
		}
		else if ( PS->QuestRole == EReadQuestRole::OnlyQuestion1 )
		{
			WordWidget->InitWordData(InScenarioData.word_data1);
		}
		else if ( PS->QuestRole == EReadQuestRole::OnlyQuestion2 )
		{
			WordWidget->InitWordData(InScenarioData.word_data2);
		}
	}
}

void UPopup_ReadQuest::OnClickClose()
{
	// PopupManager를 통해 팝업 닫기 (마우스 커서 처리 포함)
	if (UPopupManager* PopupMgr = UPopupManager::Get(GetWorld()))
	{
		PopupMgr->HideCurrentPopup();
	}
}