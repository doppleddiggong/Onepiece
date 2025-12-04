// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "UWidgetResultItem.h"

#include "UCircularProgressBar.h"
#include "Components/WidgetSwitcher.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Border.h"
#include "UGameDataManager.h"

void UWidgetResultItem::NativePreConstruct()
{
	Super::NativePreConstruct();

	UpdateActivePanel();
	ApplyStyle();
}

void UWidgetResultItem::SetWidgetType(EResultItemWidgetType InType)
{
	WidgetType = InType;
	UpdateActivePanel();
}

/* -----------------------------
   패널 데이터 설정
 -----------------------------*/

void UWidgetResultItem::SetGradeValue(EResourceTextureType TextureType)
{
	if (!Image_Grade)
		return;

	// GameDataManager에서 TextureType에 해당하는 텍스처 가져오기
	UGameDataManager* DataManager = UGameDataManager::Get(this);
	if (!DataManager)
		return;

	UTexture2D* Texture = DataManager->GetTexture(TextureType);
	if (!Texture)
		return;

	// Image의 Brush 데이터 변경
	FSlateBrush Brush = Image_Grade->GetBrush();
	Brush.SetResourceObject(Texture);
	Image_Grade->SetBrush(Brush);
}

void UWidgetResultItem::SetScoreValue(float InValue)
{
	ScoreValue = InValue;

	if (Txt_Score)
	{
		Txt_Score->SetText(FText::AsNumber(InValue));
	}
}

void UWidgetResultItem::SetRateValue(float InPercent)
{
	RateValue = InPercent;

	if (Txt_Rate)
	{
		FString Str = FString::Printf(TEXT("%.1f%%"), InPercent * 100.f);
		Txt_Rate->SetText(FText::FromString(Str));
	}

	ImageProgress_Rate->SetPercent(InPercent);
}

void UWidgetResultItem::SetSymbolValue(float InValue)
{
	SymbolValue = InValue;

	if (Txt_ImageRate)
	{
		FString Str = FString::Printf(TEXT("%.1f%%"), InValue * 100.f);
		Txt_ImageRate->SetText(FText::FromString(Str));
	}
}

/* -----------------------------
   스타일 적용
 -----------------------------*/
void UWidgetResultItem::SetColorType(EColorStyleType InType)
{
	ColorType = InType;
	ApplyStyle();
}

void UWidgetResultItem::LoadStyleTableFromDataManager()
{
	UGameDataManager* DataManager = UGameDataManager::Get(this);
	if (!DataManager)
	{
		return;
	}

	// GameDataManager에서 모든 ColorStyleData 가져오기
	StyleTable = DataManager->GetAllColorStyleData();
}

void UWidgetResultItem::ApplyStyle()
{
	if (!StyleTable.Contains(ColorType))
		return;

	const auto& Style = StyleTable[ColorType];

	if (Image_BG)
		Image_BG->SetColorAndOpacity(Style.BGColor);

	if (Border_Title)
		Border_Title->SetBrushColor(Style.BorderColor);

	if (Text_Title)
		Text_Title->SetColorAndOpacity(Style.TitleColor);
}

/* -----------------------------
   WidgetSwitcher 제어
 -----------------------------*/
void UWidgetResultItem::UpdateActivePanel()
{
	if (!WidgetSwitcher)
		return;

	int32 Index = 0;

	switch (WidgetType)
	{
	case EResultItemWidgetType::Grade:
		Index = 0;
		break;

	case EResultItemWidgetType::Score:
		Index = 1;
		break;

	case EResultItemWidgetType::Rate:
		Index = 2;
		break;

	case EResultItemWidgetType::Symbol:
		Index = 3;
		break;
	}

	WidgetSwitcher->SetActiveWidgetIndex(Index);
}