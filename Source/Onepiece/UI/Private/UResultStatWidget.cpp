// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "UResultStatWidget.h"

#include "UCircularProgressBar.h"
#include "Components/WidgetSwitcher.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Border.h"
#include "UGameDataManager.h"

#define COLORSTYLEDATA_PATH  TEXT("/Game/CustomContents/MasterData/DT_ColorStyleData.DT_ColorStyleData")

void UResultStatWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	// StyleTable이 비어있으면 자동으로 로드
	if (StyleTable.Num() == 0)
	{
		LoadStyleTable();
	}

	UpdateWidgetPanel();
	ApplyStyle();
}

void UResultStatWidget::InitData(const FResultStatData& InData)
{
	// 색상 타입 설정
	SetColorType(InData.ColorType);

	// 위젯 타입 설정
	SetWidgetType(InData.WidgetType);

	// 타이틀 텍스트 설정
	SetTitleText(InData.TitleText);
	
	// 타입별 데이터 설정
	switch (InData.WidgetType)
	{
	case EResultItemWidgetType::Grade:
		SetGradeValue(InData.GradeTextureType);
		break;

	case EResultItemWidgetType::Score:
		SetScoreValue(InData.ScoreValue);
		break;

	case EResultItemWidgetType::Rate:
		SetRateValue(InData.RatePercent);
		break;

	case EResultItemWidgetType::Symbol:
		SetSymbolValue(InData.SymbolTextureType, InData.SymbolValue);
		break;
	}
}

/* -----------------------------
   패널 데이터 설정
 -----------------------------*/
void UResultStatWidget::SetWidgetType(const EResultItemWidgetType InType)
{
	WidgetType = InType;
	UpdateWidgetPanel();
}

void UResultStatWidget::SetColorType(const EColorStyleType InType)
{
	ColorType = InType;
	ApplyStyle();
}

void UResultStatWidget::SetTitleText(const FText InText)
{
	Text_Title->SetText(InText);
}


void UResultStatWidget::SetGradeValue(const EResourceTextureType TextureType)
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

void UResultStatWidget::SetScoreValue(const float InValue)
{
	ScoreValue = InValue;

	if (Txt_Score)
	{
		Txt_Score->SetText(FText::AsNumber(InValue));
	}
}

void UResultStatWidget::SetRateValue(const float InPercent)
{
	RateValue = InPercent;

	if (Txt_Rate)
	{
		FString Str = FString::Printf(TEXT("%.1f%%"), InPercent * 100.f);
		Txt_Rate->SetText(FText::FromString(Str));
	}

	ImageProgress_Rate->SetPercent(InPercent);
}

void UResultStatWidget::SetSymbolValue(const EResourceTextureType TextureType, const FString& InValue)
{
	SymbolValue = InValue;

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
	FSlateBrush Brush = Image_Symbol->GetBrush();
	Brush.SetResourceObject(Texture);
	Image_Symbol->SetBrush(Brush);
	
	if (Txt_ImageRate)
		Txt_ImageRate->SetText(FText::FromString(InValue));
}

/* -----------------------------
   스타일 적용
 -----------------------------*/
void UResultStatWidget::LoadStyleTable()
{
	if (auto DM = UGameDataManager::Get(this))
	{
		// 런타임: GameDataManager에서 가져오기
		StyleTable = DM->GetAllColorStyleData();
		return;
	}

#if WITH_EDITOR
	// 에디터 모드: DataTable 직접 로드
	if (auto ColorStyleTable = LoadObject<UDataTable>(nullptr, COLORSTYLEDATA_PATH ))
	{
		for (const FName& RowName : ColorStyleTable->GetRowNames())
		{
			if (FColorStyleData* Row = ColorStyleTable->FindRow<FColorStyleData>(RowName, TEXT("")))
			{
				FString EnumString = RowName.ToString();
				EColorStyleType TempColorType = static_cast<EColorStyleType>(
					StaticEnum<EColorStyleType>()->GetValueByNameString(EnumString)
				);
				StyleTable.Add(TempColorType, *Row);
			}
		}
	}
#endif
}

void UResultStatWidget::ApplyStyle()
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
void UResultStatWidget::UpdateWidgetPanel() const
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