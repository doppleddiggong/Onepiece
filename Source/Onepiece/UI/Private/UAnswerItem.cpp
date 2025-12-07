// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "UAnswerItem.h"

#include "ALingoGameState.h"
#include "FReadData.h"
#include "UGameDataManager.h"
#include "Components/Image.h"
#include "Components/WidgetSwitcher.h"
#include "Components/TextBlock.h"

void UAnswerItem::InitInfo(	EQuestType QuestType, bool bCorrect,
	int32 InOrder, int32 Word1Code, int32 Word2Code)
{
	Txt_Order->SetText(FText::AsNumber(InOrder));
	WidgetSwitcherSymbol->SetActiveWidget(bCorrect ? Image_Correct : Image_Wrong);

	if( QuestType == EQuestType::Read )
	{
		FReadData ReadData;
		UGameDataManager::Get(GetWorld())->GetReadData(Word1Code, ReadData);

		UTexture2D* LoadedTexture = nullptr;
		if (ReadData.Texture.IsValid())
			LoadedTexture = ReadData.Texture.Get();
		else
			LoadedTexture = ReadData.Texture.LoadSynchronous();
		Image_Target1->SetBrushFromTexture(LoadedTexture, true);

		Txt_Target1_Kor->SetText( FText::FromString( ReadData.Word));
		Txt_Target1_Eng->SetText( FText::Format(
			FText::FromString(TEXT("[{0}]")),
			FText::FromString(TEXT("ENGLISH")) ));

		FColorData ColorData;
		UGameDataManager::Get(GetWorld())->GetColorData(Word2Code, ColorData);
		UTexture2D* ColorTexture = UGameDataManager::Get(this)->GetTexture(EResourceTextureType::Color);
		Image_Target2->SetBrushFromTexture(ColorTexture, true);
		Image_Target2->SetColorAndOpacity( ColorData.GetLinearColor());
		
		Txt_Target2_Kor->SetText( FText::FromString( ColorData.Desc));
		Txt_Target2_Eng->SetText( FText::Format(
			FText::FromString(TEXT("[{0}]")),
			FText::FromString(TEXT("ENGLISH")) ));
	}
}