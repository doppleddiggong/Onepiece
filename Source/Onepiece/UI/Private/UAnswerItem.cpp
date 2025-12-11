// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "UAnswerItem.h"

#include "ALingoGameState.h"
#include "FReadData.h"
#include "UGameDataManager.h"
#include "UWordItem.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/WidgetSwitcher.h"
#include "Components/TextBlock.h"


void UAnswerItem::InitInfo(	EQuestType QuestType, int32 InOrder,
	FScenarioTargetData TargetData, FScenarioTargetData CorrectData )
{
	const int32 TargetWord1Code = FCString::Atoi(*TargetData.word1.code);
	const int32 TargetWord2Code = FCString::Atoi(*TargetData.word2.code);

	const int32 CorrectWord1Code = FCString::Atoi(*CorrectData.word1.code);
	const int32 CorrectWord2Code = FCString::Atoi(*CorrectData.word2.code);
	
	const bool bIsCorrect =
		TargetData.word1.code == CorrectData.word1.code &&
		TargetData.word2.code == CorrectData.word2.code;
	
	this->InitInfo(QuestType, bIsCorrect, InOrder,
		TargetWord1Code, TargetWord2Code );

	TargetWord1->SetTextColor( TargetWord1Code == CorrectWord1Code ? FLinearColor::White : FLinearColor::Red );
	TargetWord2->SetTextColor( TargetWord2Code == CorrectWord2Code ? FLinearColor::White : FLinearColor::Red );
}

void UAnswerItem::InitInfo(	EQuestType QuestType, bool bCorrect,
	int32 InOrder, int32 Word1Code, int32 Word2Code)
{
	Txt_Order->SetText(FText::AsNumber(InOrder));
	if ( bCorrect )
	{
		Border_Order->SetBrushColor(FLinearColor::White);
		Txt_Order->SetColorAndOpacity(FLinearColor::Black);
	}
	else
	{
		Border_Order->SetBrushColor(FLinearColor::Red);
		Txt_Order->SetColorAndOpacity(FLinearColor::White);
	}
	
	if( QuestType == EQuestType::Read )
	{
		TargetWord1->InitInfo(EWordType::Animal, Word1Code);
		TargetWord2->InitInfo(EWordType::Color, Word2Code);
	}
	else if( QuestType == EQuestType::Listen )
	{
		TargetWord1->InitInfo(EWordType::Region, Word1Code);
		TargetWord2->InitInfo(EWordType::Food, Word2Code);
	}
}