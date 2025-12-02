// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "URichText.h"

#include "HyperLinkPluginBPLibrary.h"
#include "NetworkData.h"
#include "Components/RichTextBlock.h"
#include "Engine/DataTable.h"

#define TEXT_STYLE_PATH TEXT("/Game/CustomContents/MasterData/RichTextBlock/DT_TextSet.DT_TextSet")

void URichText::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (RichTxt)
	{
		if (auto StyleSet = LoadObject<UDataTable>(nullptr, TEXT_STYLE_PATH ))
			RichTxt->SetTextStyleSet(StyleSet);
	}

	ApplyStyle();
}

void URichText::NativeConstruct()
{
	Super::NativeConstruct();

	if (RichTxt)
	{
		if (UHyperLinkPluginBPLibrary* LinkDecorator = Cast<UHyperLinkPluginBPLibrary>(
			RichTxt->GetDecoratorByClass(UHyperLinkPluginBPLibrary::StaticClass())))
		{
			LinkDecorator->SetNativeClickHandler(FOnClickLink::CreateUObject(this, &URichText::OnClickLink));
		}
	}
}

void URichText::ApplyStyle()
{
	if (!RichTxt)
		return;

	// 기존 폰트 정보 가져오기 (없으면 기본)
	FSlateFontInfo NewFont = RichTxt->GetDefaultTextStyle().Font;
	NewFont.Size = FontSize;

	// 새로운 스타일 생성
	FTextBlockStyle NewStyle;
	NewStyle.SetFont(NewFont);
	NewStyle.SetColorAndOpacity(FSlateColor(TextColor));

	RichTxt->SetDefaultTextStyle(NewStyle);
	RichTxt->SetText(FText::FromString(DefaultText));

	// HyperLink Decorator 스타일도 함께 업데이트
	if (UHyperLinkPluginBPLibrary* LinkDecorator = Cast<UHyperLinkPluginBPLibrary>(
		RichTxt->GetDecoratorByClass(UHyperLinkPluginBPLibrary::StaticClass())))
	{
		FTextBlockStyle LinkTextStyle = LinkDecorator->Style.TextStyle;
		LinkTextStyle.SetFont(NewFont);
		LinkTextStyle.SetColorAndOpacity(FSlateColor(TextColor));
		LinkDecorator->Style.SetTextStyle(LinkTextStyle);
	}
}

void URichText::SetText(const FWordData& WordData)
{
	if (!IsValid(RichTxt))
		return;
		
	CachedPhonemeData = WordData.GetPhonemeData();

	FString TextString;
	for (int32 i = 0; i < CachedPhonemeData.Num(); ++i)
	{
		const FPhonemeData& Phoneme = CachedPhonemeData[i];
		TextString += Phoneme.ToRichTextString(i);
	}
	RichTxt->SetText(FText::FromString(TextString));
}

void URichText::SetText(const FString& InString) const
{
	if (!IsValid(RichTxt))
		return;
	
	RichTxt->SetText(FText::FromString(InString));
}

void URichText::SetText(const FText& InText) const
{
	RichTxt->SetText(InText);
}

void URichText::OnClickLink(const FString& LinkID, const FString& Content)
{
	const int32 Index = FCString::Atoi(*LinkID);

	if (CachedPhonemeData.IsValidIndex(Index))
	{
		if (OnClickHyperLink.IsBound())
			OnClickHyperLink.Execute(CachedPhonemeData[Index]);
	}
}
