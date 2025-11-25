// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "UTextureButton.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"

void UTextureButton::NativePreConstruct()
{
	Super::NativePreConstruct();
	ApplyTextures();
}

void UTextureButton::NativeConstruct()
{
	Super::NativeConstruct();

	if (Button_Main)
	{
		Button_Main->OnClicked.AddDynamic(this, &UTextureButton::HandleClicked);
		Button_Main->OnHovered.AddDynamic(this, &UTextureButton::HandleHovered);
		Button_Main->OnUnhovered.AddDynamic(this, &UTextureButton::HandleUnhovered);
		Button_Main->OnPressed.AddDynamic(this, &UTextureButton::HandlePressed);
		Button_Main->OnReleased.AddDynamic(this, &UTextureButton::HandleReleased);
	}

	// 초기 상태는 Normal
	ApplySymbolColor(NormalSymbolColor);
}

void UTextureButton::ApplyTextures()
{
	// 버튼 배경 텍스쳐 설정
	if (Button_Main && ButtonTexture)
	{
		FSlateBrush Brush;
		Brush.SetResourceObject(ButtonTexture);
		
		FButtonStyle Style = Button_Main->GetStyle();
		Style.Normal = Brush;
		Style.Hovered = Brush;
		Style.Pressed = Brush;
		Button_Main->SetStyle(Style);
	}

	// 심볼 이미지 텍스쳐 설정
	if (Image_Symbol && SymbolTexture)
	{
		Image_Symbol->SetBrushFromTexture(SymbolTexture);
		ApplySymbolColor(NormalSymbolColor);
	}

	// 버튼 크기 설정
	SetButtonSize(ButtonWidth, ButtonHeight);
}

void UTextureButton::SetButtonTexture(UTexture2D* InTexture)
{
	ButtonTexture = InTexture;

	if (Button_Main && ButtonTexture)
	{
		FSlateBrush Brush;
		Brush.SetResourceObject(ButtonTexture);
		
		FButtonStyle Style = Button_Main->GetStyle();
		Style.Normal = Brush;
		Style.Hovered = Brush;
		Style.Pressed = Brush;
		Button_Main->SetStyle(Style);
	}
}

void UTextureButton::SetSymbolTexture(UTexture2D* InTexture)
{
	SymbolTexture = InTexture;

	if (Image_Symbol && SymbolTexture)
	{
		Image_Symbol->SetBrushFromTexture(SymbolTexture);
	}
}

void UTextureButton::SetButtonSize(float InWidth, float InHeight)
{
	ButtonWidth = InWidth;
	ButtonHeight = InHeight;

	if (Button_Main)
	{
		// Button을 SizeBox로 감싸서 크기 조정
		// 또는 RenderTransform 사용
		// UMG 구조에 따라 SizeBox가 있다면 해당 SizeBox의 크기를 설정
		// 여기서는 SetRenderTransformPivot와 SetRenderScale을 사용하지 않고
		// 위젯 블루프린트에서 SizeBox를 사용하는 것을 권장
	}
}

void UTextureButton::SetButtonEnabled(bool bInEnabled)
{
	bEnabled = bInEnabled;

	if (Button_Main)
	{
		Button_Main->SetIsEnabled(bInEnabled);
	}

	// Disabled 상태 색상 적용
	if (!bEnabled)
	{
		ApplySymbolColor(DisabledSymbolColor);
	}
	else
	{
		ApplySymbolColor(NormalSymbolColor);
	}
}

void UTextureButton::ApplySymbolColor(const FLinearColor& Color)
{
	if (Image_Symbol)
	{
		Image_Symbol->SetColorAndOpacity(Color);
	}
}

void UTextureButton::HandleClicked()
{
	if (!bEnabled)
		return;

	OnButtonClickedEvent.Broadcast();
}

void UTextureButton::HandleHovered()
{
	if (!bEnabled)
		return;

	ApplySymbolColor(HoverSymbolColor);
}

void UTextureButton::HandleUnhovered()
{
	if (!bEnabled)
		return;

	ApplySymbolColor(NormalSymbolColor);
}

void UTextureButton::HandlePressed()
{
	if (!bEnabled)
		return;

	ApplySymbolColor(PressSymbolColor);
}

void UTextureButton::HandleReleased()
{
	if (!bEnabled)
		return;

	// Released 후 마우스가 여전히 버튼 위에 있는지 확인
	if (Button_Main->IsHovered())
	{
		ApplySymbolColor(HoverSymbolColor);
	}
	else
	{
		ApplySymbolColor(NormalSymbolColor);
	}
}
