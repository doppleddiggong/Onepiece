// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file UVoiceRecording.cpp
 * @brief UVoiceRecording의 동작을 구현합니다.
 */
#include "UVoiceRecording.h"

#include "UBroadcastManager.h"
#include "UCircularProgressBar.h"
#include "Components/Image.h"

void UVoiceRecording::InitWidget()
{
	// 브로드캐스트 매니저에 이벤트 등록
	if (auto EventManager = UBroadcastManager::Get(GetWorld()))
	{
		EventManager->OnAudioCapture.RemoveDynamic(this, &UVoiceRecording::OnAudioCapture);
		EventManager->OnAudioCapture.AddDynamic(this, &UVoiceRecording::OnAudioCapture);

		EventManager->OnAudioSpectrum.RemoveDynamic(this, &UVoiceRecording::OnAudioSpectrum);
		EventManager->OnAudioSpectrum.AddDynamic(this, &UVoiceRecording::OnAudioSpectrum);
	}

	StartRecordingColor = FColor::FromHex(TEXT("CEF97CFF"));
	StopRecordingColor = FColor::FromHex(TEXT("A3A3A3FF"));
}

void UVoiceRecording::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateSpectrumVisual(InDeltaTime);
}

void UVoiceRecording::UpdateSpectrumVisual(float DeltaTime)
{
	if ( !Image_Symbol->IsVisible())
		return;

	const float Delta = FMath::Max(DeltaTime, 0.0f);
	const float TargetValue = SpectrumDisplayValue;
	const float CurrentValue = CircularProgressBar->Percent;
	const float InterpSpeed = TargetValue > CurrentValue ? SpectrumRiseSpeed : SpectrumDecaySpeed;
	const float NewPercent = FMath::FInterpConstantTo(CurrentValue, TargetValue, Delta, InterpSpeed);
	CircularProgressBar->SetPercent(FMath::Clamp(NewPercent, 0.0f, 1.0f));

	// Image_Symbol 스케일 업데이트
	const float Scale = FMath::Lerp(1.0f, 1.75f, NewPercent);
	Image_Symbol->SetRenderScale(FVector2D(Scale, Scale));
}

void UVoiceRecording::OnAudioCapture(bool bRecording)
{
	Image_Symbol->SetColorAndOpacity(bRecording ? StartRecordingColor : StopRecordingColor );

	if ( bRecording == false )
	{
		SpectrumDisplayValue = 0.0f;
		CircularProgressBar->SetPercent(0.0f);
	}
}

void UVoiceRecording::OnAudioSpectrum(float Spectrum)
{
	SpectrumDisplayValue = Spectrum;
}