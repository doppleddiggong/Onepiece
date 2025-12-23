// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file UVoiceRecording.cpp
 * @brief UVoiceRecording의 동작을 구현합니다.
 */
#include "UVoiceRecording.h"

#include "UBroadcastManager.h"
#include "UCircularProgressBar.h"

void UVoiceRecording::InitWidget()
{
	// 초기에는 숨김 상태로 시작
	SetVisibility(ESlateVisibility::Hidden);

	// 브로드캐스트 매니저에 이벤트 등록
	if (auto EventManager = UBroadcastManager::Get(GetWorld()))
	{
		EventManager->OnAudioCapture.AddDynamic(this, &UVoiceRecording::OnAudioCapture);
		EventManager->OnAudioSpectrum.AddDynamic(this, &UVoiceRecording::OnAudioSpectrum);
	}
}

void UVoiceRecording::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateSpectrumVisual(InDeltaTime);
}

void UVoiceRecording::UpdateSpectrumVisual(float DeltaTime)
{
	if (!CircularProgressBar || !CircularProgressBar->IsVisible())
		return;

	const float Delta = FMath::Max(DeltaTime, 0.0f);
	const float TargetValue = SpectrumDisplayValue;
	const float CurrentValue = CircularProgressBar->Percent;
	const float InterpSpeed = TargetValue > CurrentValue ? SpectrumRiseSpeed : SpectrumDecaySpeed;
	const float NewPercent = FMath::FInterpConstantTo(CurrentValue, TargetValue, Delta, InterpSpeed);
	CircularProgressBar->SetPercent(FMath::Clamp(NewPercent, 0.0f, 1.0f));
}

void UVoiceRecording::OnAudioCapture(bool bRecording)
{
	if (!CircularProgressBar)
		return;

	if (bRecording)
	{
		// StartAudioRecording: 흰색(알파 100%)으로 보임
		SetVisibility(ESlateVisibility::Visible);
		SetRenderOpacity(1.0f);
		CircularProgressBar->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		// StopAudioRecording: 흰색(알파 25%)으로 보임
		SetRenderOpacity(0.25f);
	}
}

void UVoiceRecording::OnAudioSpectrum(float Spectrum)
{
	SpectrumDisplayValue = Spectrum;
}
