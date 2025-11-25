// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "UHoverButton.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"

void UHoverButton::NativePreConstruct()
{
    Super::NativePreConstruct();
    ApplyStyle();
}

void UHoverButton::NativeConstruct()
{
    Super::NativeConstruct();
   
    if (Button_Main)
    {
        Button_Main->OnHovered.AddDynamic(this, &UHoverButton::HandleHovered);
        Button_Main->OnUnhovered.AddDynamic(this, &UHoverButton::HandleUnhovered);
        Button_Main->OnPressed.AddDynamic(this, &UHoverButton::HandlePressed);
        Button_Main->OnReleased.AddDynamic(this, &UHoverButton::HandleReleased);
        Button_Main->OnClicked.AddDynamic(this, &UHoverButton::HandleClicked);
    }
}

void UHoverButton::ApplyStyle()
{
    if (!Text_Label)
        return;

    // 텍스트 반영
    Text_Label->SetText(LabelText);

    // 폰트 크기 반영
    FSlateFontInfo NewFont = Text_Label->GetFont();
    NewFont.Size = FontSize;
    Text_Label->SetFont(NewFont);

    // Normal 상태 기본 세팅
    Text_Label->SetColorAndOpacity(NormalTextColor);
}

void UHoverButton::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    // 스케일 보간
    CurrentScale = FMath::Vector2DInterpTo(CurrentScale, TargetScale, InDeltaTime, LerpSpeed);
    SetRenderScale(CurrentScale);

    // 밝기 보간
    CurrentBrightness = FMath::FInterpTo(CurrentBrightness, TargetBrightness, InDeltaTime, LerpSpeed);

    if (Border_BG)
        Border_BG->SetBrushColor(FLinearColor(CurrentBrightness, CurrentBrightness, CurrentBrightness));
}

void UHoverButton::HandleHovered()
{
    if (!bEnabled)
        return;
    
    TargetScale = FVector2D(1.06f, 1.06f);   // Hover 확대
    TargetBrightness = 1.15f;
    Text_Label->SetColorAndOpacity(HoverTextColor);   // ← 추가
}

void UHoverButton::HandleUnhovered()
{
    if (!bEnabled)
        return;
    
    TargetScale = FVector2D(1.0f, 1.0f);
    TargetBrightness = 1.0f;
    Text_Label->SetColorAndOpacity(NormalTextColor);  // ← 추가
}

void UHoverButton::HandlePressed()
{
    if (!bEnabled)
        return;
    
    TargetScale = FVector2D(0.97f, 0.97f);      // 눌리는 효과
    Text_Label->SetColorAndOpacity(PressTextColor);   // ← 추가
}

void UHoverButton::HandleReleased()
{
    if (!bEnabled)
        return;

    if (Button_Main->IsHovered())
        HandleHovered();
    else
        HandleUnhovered();
}

void UHoverButton::HandleClicked()
{
    if (!bEnabled)
        return;
    
    OnButtonClickedEvent.Broadcast();
}

void UHoverButton::SetButtonEnabled(bool bInEnabled)
{
    bEnabled = bInEnabled;

    Button_Main->SetIsEnabled(bInEnabled);

    if (bInEnabled)
    {
        TargetScale = FVector2D(1.f, 1.f);
        TargetBrightness = 1.f;
    }
    else
    {
        TargetScale = FVector2D(1.f, 1.f);
        TargetBrightness = 0.4f;      // Disable 톤 다운
    }
}