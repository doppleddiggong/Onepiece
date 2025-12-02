// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "UPopup_ReadQuest.h"
#include "ALingoGameState.h"
#include "ALingoPlayerState.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerController.h"
#include "GameLogging.h"
#include "UBroadcastManager.h"
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
	PRINTLOG(TEXT("[ReadQuestWidget] InitPopup quest"));

	// 중복 바인딩 방지: 기존 바인딩 제거 후 재바인딩
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
		WordWidget->InitWordData(ScenarioData.full_data);
	else if ( QuestRole == EQuestRole::OnlyQuestion1 )
		WordWidget->InitWordData(ScenarioData.word_data1);
	else if ( QuestRole == EQuestRole::OnlyQuestion2 )
		WordWidget->InitWordData(ScenarioData.word_data2);
}

void UPopup_ReadQuest::OnClickClose()
{
	// PopupManager를 통해 팝업 닫기 (마우스 커서 처리 포함)
	if (UPopupManager* PopupMgr = UPopupManager::Get(GetWorld()))
	{
		PopupMgr->HideCurrentPopup();
	}
}