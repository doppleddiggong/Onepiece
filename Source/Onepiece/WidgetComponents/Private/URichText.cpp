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

	ApplyStyleWithFontSize(FontSize, false);
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

void URichText::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// 텍스트가 변경되었거나 위젯 크기가 변경된 경우에만 폰트 크기 조정
	const float CurrentWidth = MyGeometry.GetLocalSize().X;
	if (FMath::Abs(CurrentWidth - LastWidgetWidth) > 1.0f)
	{
		LastWidgetWidth = CurrentWidth;
		bNeedsFontSizeUpdate = true;
		StabilizationFrames = 0;
	}

	// 폰트 크기 업데이트가 필요하고 아직 안정화되지 않은 경우에만 실행
	if (bNeedsFontSizeUpdate && StabilizationFrames < 3)
	{
		UpdateFontSizeToFitWidth(MyGeometry);
		StabilizationFrames++;
		
		// 3프레임 동안 안정화 시도 후 중단
		if (StabilizationFrames >= 3)
		{
			bNeedsFontSizeUpdate = false;
		}
	}
}

void URichText::ApplyStyleWithFontSize(int32 InFontSize, bool bPreserveText)
{
	if (!RichTxt)
		return;

	const FText PreviousText = RichTxt->GetText();

	// 기존 폰트 정보 가져오기 (없으면 기본)
	FSlateFontInfo NewFont = RichTxt->GetDefaultTextStyle().Font;
	NewFont.Size = InFontSize;

	// 새로운 스타일 생성
	FTextBlockStyle NewStyle;
	NewStyle.SetFont(NewFont);
	NewStyle.SetColorAndOpacity(FSlateColor(TextColor));

	RichTxt->SetDefaultTextStyle(NewStyle);

	if (!bPreserveText && !DefaultText.IsEmpty() && PreviousText.IsEmpty())
	{
		RichTxt->SetText(FText::FromString(DefaultText));
	}
	else if (bPreserveText)
	{
		RichTxt->SetText(PreviousText);
	}

	// HyperLink Decorator 스타일도 함께 업데이트
	if (UHyperLinkPluginBPLibrary* LinkDecorator = Cast<UHyperLinkPluginBPLibrary>(
			RichTxt->GetDecoratorByClass(UHyperLinkPluginBPLibrary::StaticClass())))
	{
		FTextBlockStyle LinkTextStyle = LinkDecorator->Style.TextStyle;
		LinkTextStyle.SetFont(NewFont);
		LinkTextStyle.SetColorAndOpacity(FSlateColor(TextColor));
		LinkDecorator->Style.SetTextStyle(LinkTextStyle);
	}

	CurrentFontSize = InFontSize;
}

void URichText::UpdateFontSizeToFitWidth(const FGeometry& MyGeometry)
{
	if (!bAutoFitToWidth || !RichTxt)
		return;

	const float AvailableWidth = MyGeometry.GetLocalSize().X;
	if (AvailableWidth <= 0.0f)
		return;

	const FVector2D DesiredSize = RichTxt->GetDesiredSize();
	const float DesiredWidth = DesiredSize.X;

	int32 TargetFontSize = FontSize;

	// Threshold 적용: 여유 공간을 2픽셀 정도 남겨서 미세한 변동 방지
	const float WidthThreshold = 2.0f;
	if (DesiredWidth > AvailableWidth + WidthThreshold && DesiredWidth > 0.0f)
	{
		// 약간 여유를 둔 스케일 계산 (98%만 사용)
		const float Scale = (AvailableWidth * 0.98f) / DesiredWidth;
		const int32 ScaledSize = FMath::FloorToInt(static_cast<float>(FontSize) * Scale);
		TargetFontSize = FMath::Clamp(ScaledSize, MinAutoFontSize, FontSize);
	}

	// 폰트 크기 차이가 1 이상일 때만 적용 (미세한 차이 무시)
	if (FMath::Abs(TargetFontSize - CurrentFontSize) >= 1)
	{
		ApplyStyleWithFontSize(TargetFontSize, true);
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
	
	// 텍스트가 변경되었으므로 폰트 크기 재조정 필요
	bNeedsFontSizeUpdate = true;
	StabilizationFrames = 0;
}

void URichText::SetText(const FString& InString)
{
	if (!IsValid(RichTxt))
		return;
	
	RichTxt->SetText(FText::FromString(InString));
	
	// 텍스트가 변경되었으므로 폰트 크기 재조정 필요
	bNeedsFontSizeUpdate = true;
	StabilizationFrames = 0;
}

void URichText::SetText(const FText& InText)
{
	if (!IsValid(RichTxt))
		return;
		
	RichTxt->SetText(InText);
	
	// 텍스트가 변경되었으므로 폰트 크기 재조정 필요
	bNeedsFontSizeUpdate = true;
	StabilizationFrames = 0;
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
