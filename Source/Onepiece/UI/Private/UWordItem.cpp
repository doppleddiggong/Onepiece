// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "UWordItem.h"
#include "FReadData.h"
#include "FListenData.h"
#include "UGameDataManager.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UWordItem::InitInfo( EWordType WordType, int32 WordCode )
{
	if( WordType == EWordType::Animal )
	{
		FReadData ReadData;
		UGameDataManager::Get(GetWorld())->GetReadData(WordCode, ReadData);

		UTexture2D* LoadedTexture = nullptr;
		if (ReadData.Texture.IsValid())
			LoadedTexture = ReadData.Texture.Get();
		else
			LoadedTexture = ReadData.Texture.LoadSynchronous();
			
		Image_Target->SetBrushFromTexture(LoadedTexture, true);
		Image_Target->SetColorAndOpacity( FLinearColor::White );

		Txt_Kor->SetText( FText::FromString( ReadData.Word));
		Txt_Eng->SetText( FText::Format( FText::FromString(TEXT("[{0}]")), FText::FromString( ReadData.Eng) ));
	}
	else if( WordType == EWordType::Color )
	{
		FColorData ColorData;
		UGameDataManager::Get(GetWorld())->GetColorData(WordCode, ColorData);
		
		UTexture2D* ColorTexture = UGameDataManager::Get(this)->GetTexture(EResourceTextureType::Color);
		Image_Target->SetBrushFromTexture(ColorTexture, true);
		Image_Target->SetColorAndOpacity( ColorData.GetLinearColor());
		
		Txt_Kor->SetText( FText::FromString( ColorData.Desc));
		Txt_Eng->SetText( FText::Format( FText::FromString(TEXT("[{0}]")), FText::FromString( ColorData.Eng) ));
	}
	else if( WordType == EWordType::Region )
	{
		FListenData ListenData;
		UGameDataManager::Get(GetWorld())->GetListenData(WordCode, ListenData);

		UTexture2D* LoadedTexture = nullptr;
		if (ListenData.Texture.IsValid())
			LoadedTexture = ListenData.Texture.Get();
		else
			LoadedTexture = ListenData.Texture.LoadSynchronous();
			
		Image_Target->SetBrushFromTexture(LoadedTexture, true);
		Image_Target->SetColorAndOpacity( FLinearColor::White );

		Txt_Kor->SetText( FText::FromString( ListenData.Word));
		Txt_Eng->SetText( FText::Format( FText::FromString(TEXT("[{0}]")), FText::FromString(ListenData.Eng) ));
	}
	else if( WordType == EWordType::Food )
	{
		FListenData ListenData;
		UGameDataManager::Get(GetWorld())->GetListenData(WordCode, ListenData);

		UTexture2D* LoadedTexture = nullptr;
		if (ListenData.Texture.IsValid())
			LoadedTexture = ListenData.Texture.Get();
		else
			LoadedTexture = ListenData.Texture.LoadSynchronous();
		
		Image_Target->SetBrushFromTexture(LoadedTexture, true);
		Image_Target->SetColorAndOpacity( FLinearColor::White );
		Txt_Kor->SetText( FText::FromString( ListenData.Word ));
		Txt_Eng->SetText( FText::Format( FText::FromString(TEXT("[{0}]")), FText::FromString(ListenData.Eng)));
	}
}
