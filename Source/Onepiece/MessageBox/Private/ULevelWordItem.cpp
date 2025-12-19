// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "ULevelWordItem.h"
#include "UGameDataManager.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/ScaleBox.h"
#include "Components/SizeBox.h"

void ULevelWordItem::InitInfo( EWordType WordType, int32 WordCode )
{
	FString DisplayText;

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

		DisplayText = ReadData.Eng;
	}
	else if( WordType == EWordType::Color )
	{
		FColorData ColorData;
		UGameDataManager::Get(GetWorld())->GetColorData(WordCode, ColorData);

		UTexture2D* ColorTexture = UGameDataManager::Get(this)->GetTexture(EResourceTextureType::Color);
		Image_Target->SetBrushFromTexture(ColorTexture, true);
		Image_Target->SetColorAndOpacity( ColorData.GetLinearColor());

		DisplayText = ColorData.Eng;
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

		DisplayText = ListenData.Eng;
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

		DisplayText = ListenData.Eng;
	}

	// 텍스트 처리 방식 적용
	if (bUseScaleBox)
	{
		// 방법 1: ScaleBox 사용 (자동 크기 조절)
		ApplyScaleBoxMode();
		Txt_Title->SetText(FText::FromString(DisplayText));
	}
	else
	{
		// 방법 2: 단순 절삭 방식
		ApplyTruncateMode(DisplayText);
	}
}

void ULevelWordItem::SetItemColor(bool bIsCurrentLevel)
{
	FLinearColor TargetColor = bIsCurrentLevel ? FLinearColor::Black : FLinearColor(0.5f, 0.5f, 0.5f, 1.0f);

	if (Txt_Title)
	{
		Txt_Title->SetColorAndOpacity(TargetColor);
	}
}

void ULevelWordItem::ApplyScaleBoxMode()
{
	// 방법 1: ScaleBox를 사용한 자동 크기 조절
	if (ScaleBox)
	{
		ScaleBox->SetStretch(EStretch::ScaleToFit);
	}
}

void ULevelWordItem::ApplyTruncateMode(const FString& Text)
{
	// 방법 2: 단순 절삭 방식
	FString TruncatedText = TruncateText(Text);
	Txt_Title->SetText(FText::FromString(TruncatedText));
}

FString ULevelWordItem::TruncateText(const FString& OriginalText, int32 MaxLength)
{
	if (OriginalText.Len() <= MaxLength)
	{
		return OriginalText;
	}

	// MaxLength를 넘어가면 그냥 잘라내고 "..." 추가
	return OriginalText.Left(MaxLength - 3) + TEXT("...");
}
