// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file UStateWidget.cpp
 * @brief UStateWidget의 동작을 구현합니다.
 */
#include "UStateWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "TimerManager.h"
#include "UBroadcastManager.h"
#include "Misc/DateTime.h"


void UStateWidget::InitWidget()
{
    if (SpectrumProgressBar)
        SpectrumProgressBar->SetVisibility(ESlateVisibility::Hidden);

    Txt_UserId->SetText(FText::FromString(""));
    Txt_UserName->SetText(FText::FromString(""));
    
    if (auto EventManager = UBroadcastManager::Get(GetWorld()))
    {
        EventManager->OnAudioCapture.AddDynamic(this, &UStateWidget::OnAudioCapture);
        EventManager->OnAudioSpectrum.AddDynamic(this, &UStateWidget::OnAudioSpectrum);
    }
}

void UStateWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    UpdateSpectrumVisual(InDeltaTime);
}

void UStateWidget::UpdateSpectrumVisual(float DeltaTime)
{
    if (!SpectrumProgressBar || !SpectrumProgressBar->IsVisible())
        return;

    const float Delta = FMath::Max(DeltaTime, 0.0f);
    const float TargetValue = SpectrumDisplayValue;
    const float CurrentValue = SpectrumProgressBar->GetPercent();
    const float InterpSpeed = TargetValue > CurrentValue ? SpectrumRiseSpeed : SpectrumDecaySpeed;
    const float NewPercent = FMath::FInterpConstantTo(CurrentValue, TargetValue, Delta, InterpSpeed);
    SpectrumProgressBar->SetPercent(FMath::Clamp(NewPercent, 0.0f, 1.0f));
}

void UStateWidget::UpdateUserName(int32 InUserId, const FString& InUserName)
{
    const FString Result = FString::Format(TEXT("[{0}]"),{ InUserId });
    Txt_UserId->SetText(FText::FromString(Result));
    Txt_UserName->SetText(FText::FromString(InUserName));
}

void UStateWidget::OnAudioCapture(bool bRecording)
{
    if (!SpectrumProgressBar)
        return;

    SpectrumProgressBar->SetVisibility(bRecording ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void UStateWidget::OnAudioSpectrum(float Spectrum)
{
    SpectrumDisplayValue = Spectrum;
}