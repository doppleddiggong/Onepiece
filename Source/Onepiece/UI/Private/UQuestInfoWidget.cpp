// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "UQuestInfoWidget.h"
#include "ALingoPlayerState.h"
#include "UBroadcastManager.h"

#include "ULingoGameHelper.h"
#include "Components/TextBlock.h"



void UQuestInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (auto BM = UBroadcastManager::Get(GetWorld()))
		BM->OnUpdateQuestRole.AddDynamic(this, &UQuestInfoWidget::InitQuestInfo);
}

void UQuestInfoWidget::InitQuestInfo(EQuestRole QuestRole)
{
	auto GS = ULingoGameHelper::GetLingoGameState(GetWorld());
	if ( !GS->IsQuestIng() )
		return;

	if ( QuestRole == EQuestRole::Both )
	{
		Txt_Message->SetText( FText::FromString(GS->ReadScenarioData.full_data.Kor));
	}
	else if ( QuestRole == EQuestRole::OnlyQuestion1 )
	{
		Txt_Message->SetText( FText::FromString(GS->ReadScenarioData.word_data1.Kor));
	}
	else if ( QuestRole == EQuestRole::OnlyQuestion2 )
	{
		Txt_Message->SetText( FText::FromString(GS->ReadScenarioData.word_data2.Kor));
	}
}
