// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UVoiceRecording.generated.h"

/**
 * @brief 음성 녹음 상태를 표시하는 위젯
 * @details OnAudioCapture 이벤트를 받아 녹음 상태를 시각화합니다.
 */
UCLASS(BlueprintType, Blueprintable)
class ONEPIECE_API UVoiceRecording : public UUserWidget
{
	GENERATED_BODY()

public:
	/// @brief 위젯 초기화
	void InitWidget();

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	/// @brief 스펙트럼 시각화 업데이트
	void UpdateSpectrumVisual(float DeltaTime);

	/// @brief 오디오 캡처 상태 변경 이벤트
	/// @param bRecording true면 녹음 시작, false면 녹음 종료
	UFUNCTION(BlueprintCallable, Category = "VoiceRecording|AudioCapture")
	void OnAudioCapture(bool bRecording);

	/// @brief 오디오 스펙트럼 데이터 수신 이벤트
	/// @param Spectrum 스펙트럼 값 (0.0 ~ 1.0)
	UFUNCTION(BlueprintCallable, Category = "VoiceRecording|AudioCapture")
	void OnAudioSpectrum(float Spectrum);

protected:
	/// @brief 원형 프로그레스바 위젯
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "VoiceRecording|UI")
	TObjectPtr<class UCircularProgressBar> CircularProgressBar;

	/// @brief 스펙트럼 감쇠 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VoiceRecording|Audio", meta = (ClampMin = "0.0"))
	float SpectrumDecaySpeed = 6.0f;

	/// @brief 스펙트럼 상승 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VoiceRecording|Audio", meta = (ClampMin = "0.0"))
	float SpectrumRiseSpeed = 12.0f;

	/// @brief 스펙트럼 스무딩 비율
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VoiceRecording|Audio", meta = (ClampMin = "0.001", ClampMax = "1.0"))
	float SpectrumSmoothing = 0.2f;

private:
	/// @brief 스펙트럼 표시 값
	float SpectrumDisplayValue = 0.f;
};
