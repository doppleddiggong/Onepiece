// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file UStateWidget.cpp
 * @brief UStateWidget의 동작을 구현합니다.
 */
#include "UStateWidget.h"

#include "ALingoPlayerState.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "TimerManager.h"
#include "UBroadcastManager.h"
#include "ULingoGameHelper.h"
#include "Misc/DateTime.h"

void UStateWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

void UStateWidget::NativeDestruct()
{
    if (UWorld* World = GetWorld())
        World->GetTimerManager().ClearTimer(UpdateTimerHandle);

    Super::NativeDestruct();
}

void UStateWidget::InitWidget()
{
    if (LoadingSpinner)
        LoadingSpinner->SetVisibility(ESlateVisibility::Hidden);

    if (SpectrumProgressBar)
        SpectrumProgressBar->SetVisibility(ESlateVisibility::Hidden);

    if (UserNameText)
    {
        if (auto PS = ULingoGameHelper::GetLingoPlayerState(this))
            UserNameText->SetText( FText::FromString(PS->GetUserName()));
        else
            UserNameText->SetText(FText::GetEmpty());
    }

    if (auto EventManager = UBroadcastManager::Get(GetWorld()))
    {
        EventManager->OnNetworkWaitCount.AddDynamic(this, &UStateWidget::OnNetworkWaitCount);
        EventManager->OnAudioCapture.AddDynamic(this, &UStateWidget::OnAudioCapture);
        EventManager->OnAudioSpectrum.AddDynamic(this, &UStateWidget::OnAudioSpectrum);
        EventManager->OnUpdateUserName.AddDynamic(this, &UStateWidget::OnUpdateUserName);
    }
}

void UStateWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    UpdateSpectrumVisual(InDeltaTime);
    UpdateLoadingSpinner(InDeltaTime);
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

void UStateWidget::UpdateLoadingSpinner(float DeltaTime)
{
    if (!LoadingSpinner || !LoadingSpinner->IsVisible())
        return;

    const float NewAngle = LoadingSpinner->GetRenderTransformAngle() + (SpinnerRotationSpeed * DeltaTime);
    LoadingSpinner->SetRenderTransformAngle(NewAngle);
}

void UStateWidget::OnNetworkWaitCount(int NetworkWaitCount)
{
    if (!LoadingSpinner)
        return;

    LoadingSpinner->SetVisibility(NetworkWaitCount > 0 ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
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

void UStateWidget::OnUpdateUserName(FString UserName)
{
    UserNameText->SetText(FText::FromString(UserName));
}