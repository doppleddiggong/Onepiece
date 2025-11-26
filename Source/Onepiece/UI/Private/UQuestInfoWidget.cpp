// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "UQuestInfoWidget.h"
#include "ALingoPlayerState.h"

#include "ULingoGameHelper.h"
#include "Components/TextBlock.h"

void UQuestInfoWidget::InitQuestInfo()
{
	auto GS = ULingoGameHelper::GetLingoGameState(GetWorld());
	if ( !GS->IsQuestIng() )
		return;

	if ( const auto PS = ULingoGameHelper::GetLingoPlayerState(GetWorld()) )
	{
		if ( PS->QuestRole == EReadQuestRole::Both )
		{
			Txt_Message->SetText( FText::FromString(GS->CurScenarioData.full_data.Kor));
		}
		else if ( PS->QuestRole == EReadQuestRole::OnlyQuestion1 )
		{
			Txt_Message->SetText( FText::FromString(GS->CurScenarioData.word_data1.Kor));
		}
		else if ( PS->QuestRole == EReadQuestRole::OnlyQuestion2 )
		{
			Txt_Message->SetText( FText::FromString(GS->CurScenarioData.word_data2.Kor));
		}
	}	
}
