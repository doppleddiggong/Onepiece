// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "UImageButton.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Components/SizeBox.h"
#include "Engine/Texture2D.h"

void UImageButton::NativePreConstruct()
{
	Super::NativePreConstruct();
	ApplyStyle();
}

void UImageButton::NativeConstruct()
{
	Super::NativeConstruct();

	if (Button_Main)
	{
		Button_Main->OnHovered.AddDynamic(this, &UImageButton::HandleHovered);
		Button_Main->OnUnhovered.AddDynamic(this, &UImageButton::HandleUnhovered);
		Button_Main->OnPressed.AddDynamic(this, &UImageButton::HandlePressed);
		Button_Main->OnReleased.AddDynamic(this, &UImageButton::HandleReleased);
		Button_Main->OnClicked.AddDynamic(this, &UImageButton::HandleClicked);
	}
}

void UImageButton::ApplyStyle()
{
	// ========== 텍스트 스타일 적용 ==========
	if (Text_Label)
	{
		Text_Label->SetText(LabelText);

		FSlateFontInfo NewFont = Text_Label->GetFont();
		NewFont.OutlineSettings.OutlineSize = FontOutlineSize;
		NewFont.Size = FontSize;
		Text_Label->SetFont(NewFont);

		Text_Label->SetColorAndOpacity(NormalTextColor);
	}

	// ========== Border 배경 및 이미지 적용 ==========
	if (Border_BG)
	{
		// Border 이미지가 설정되어 있으면 이미지 사용
		if (BorderImage)
		{
			CachedBorderBrush.SetResourceObject(BorderImage);
			CachedBorderBrush.DrawAs = BorderDrawAs;
			CachedBorderBrush.Margin = BorderMargin;
			CachedBorderBrush.TintColor = FSlateColor(NormalButtonColor);
			Border_BG->SetBrush(CachedBorderBrush);
			bBorderBrushInitialized = true;
		}
		else
		{
			// 이미지가 없으면 색상만 사용
			Border_BG->SetBrushColor(NormalButtonColor);
			bBorderBrushInitialized = false;
		}
	}

	// ========== 고정 크기 적용 ==========
	if (SizeBox_Root)
	{
		if (bUseFixedSize)
		{
			SizeBox_Root->SetWidthOverride(FixedWidth);
			SizeBox_Root->SetHeightOverride(FixedHeight);
		}
		else
		{
			// 고정 크기를 사용하지 않으면 자동 크기 조정
			SizeBox_Root->ClearWidthOverride();
			SizeBox_Root->ClearHeightOverride();
		}
	}
}

void UImageButton::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// 스케일 보간
	CurrentScale = FMath::Vector2DInterpTo(CurrentScale, TargetScale, InDeltaTime, LerpSpeed);
	SetRenderScale(CurrentScale);
}

void UImageButton::HandleHovered()
{
	if (!bEnabled)
		return;

	TargetScale = FVector2D(1.06f, 1.06f);
	TargetBrightness = 1.15f;

	if (Text_Label)
	{
		Text_Label->SetColorAndOpacity(HoverTextColor);
	}

	if (Border_BG)
	{
		if (BorderImage && bBorderBrushInitialized)
		{
			// 이미지가 있으면 Tint 색상 변경
			CachedBorderBrush.TintColor = FSlateColor(HoverButtonColor);
			Border_BG->SetBrush(CachedBorderBrush);
		}
		else
		{
			Border_BG->SetBrushColor(HoverButtonColor);
		}
	}
}

void UImageButton::HandleUnhovered()
{
	if (!bEnabled)
		return;

	TargetScale = FVector2D(1.0f, 1.0f);
	TargetBrightness = 1.0f;

	if (Text_Label)
	{
		Text_Label->SetColorAndOpacity(NormalTextColor);
	}

	if (Border_BG)
	{
		if (BorderImage && bBorderBrushInitialized)
		{
			CachedBorderBrush.TintColor = FSlateColor(NormalButtonColor);
			Border_BG->SetBrush(CachedBorderBrush);
		}
		else
		{
			Border_BG->SetBrushColor(NormalButtonColor);
		}
	}
}

void UImageButton::HandlePressed()
{
	if (!bEnabled)
		return;

	TargetScale = FVector2D(0.97f, 0.97f);

	if (Text_Label)
	{
		Text_Label->SetColorAndOpacity(PressTextColor);
	}

	if (Border_BG)
	{
		if (BorderImage && bBorderBrushInitialized)
		{
			CachedBorderBrush.TintColor = FSlateColor(PressButtonColor);
			Border_BG->SetBrush(CachedBorderBrush);
		}
		else
		{
			Border_BG->SetBrushColor(PressButtonColor);
		}
	}
}

void UImageButton::HandleReleased()
{
	if (!bEnabled)
		return;

	if (Button_Main && Button_Main->IsHovered())
		HandleHovered();
	else
		HandleUnhovered();
}

void UImageButton::HandleClicked()
{
	if (!bEnabled)
		return;

	OnButtonClickedEvent.Broadcast();
}

void UImageButton::SetButtonEnabled(bool bInEnabled)
{
	bEnabled = bInEnabled;

	if (Button_Main)
	{
		Button_Main->SetIsEnabled(bInEnabled);
	}

	if (bInEnabled)
	{
		TargetScale = FVector2D(1.f, 1.f);
		TargetBrightness = 1.f;

		if (Border_BG)
		{
			if (BorderImage && bBorderBrushInitialized)
			{
				CachedBorderBrush.TintColor = FSlateColor(NormalButtonColor);
				Border_BG->SetBrush(CachedBorderBrush);
			}
			else
			{
				Border_BG->SetBrushColor(NormalButtonColor);
			}
		}

		if (Text_Label)
		{
			Text_Label->SetColorAndOpacity(NormalTextColor);
		}
	}
	else
	{
		TargetScale = FVector2D(1.f, 1.f);
		TargetBrightness = 0.4f;

		if (Border_BG)
		{
			if (BorderImage && bBorderBrushInitialized)
			{
				CachedBorderBrush.TintColor = FSlateColor(NormalButtonColor * 0.5f);
				Border_BG->SetBrush(CachedBorderBrush);
			}
			else
			{
				Border_BG->SetBrushColor(NormalButtonColor * 0.5f);
			}
		}

		if (Text_Label)
		{
			Text_Label->SetColorAndOpacity(NormalTextColor * 0.5f);
		}
	}
}
