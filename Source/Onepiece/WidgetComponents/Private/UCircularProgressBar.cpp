// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "UCircularProgressBar.h"

#include "Components/Image.h"

void UCircularProgressBar::NativePreConstruct()
{
	Super::NativePreConstruct();
	ApplyStyle();
}

void UCircularProgressBar::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// 애니메이션 업데이트
	if (bIsAnimating)
	{
		AnimElapsedTime += InDeltaTime;

		// 진행률 계산 (0.0 ~ 1.0)
		float Progress = FMath::Clamp(AnimElapsedTime / AnimDuration, 0.0f, 1.0f);

		// EaseOutSine 적용
		float EasedProgress = easeOutSine(Progress);

		// Start에서 Target으로 Lerp
		Percent = FMath::Lerp(AnimStartPercent, AnimTargetPercent, EasedProgress);

		// 스타일 적용
		ApplyStyle();

		// 애니메이션 완료
		if (Progress >= 1.0f)
		{
			bIsAnimating = false;
			Percent = AnimTargetPercent; // 정확한 값으로 설정
			ApplyStyle();
		}
	}
}

void UCircularProgressBar::SetPercent(const float InPercent)
{
	// 애니메이션 중단
	bIsAnimating = false;

	Percent = FMath::Clamp(InPercent, 0.0f, 1.0f);
	ApplyStyle();
}

void UCircularProgressBar::StartProgress(const float Start, const float End, const float Duration)
{
	// 애니메이션 설정
	AnimStartPercent = FMath::Clamp(Start, 0.0f, 1.0f);
	AnimTargetPercent = FMath::Clamp(End, 0.0f, 1.0f);
	AnimDuration = FMath::Max(Duration, 0.01f); // 최소 0.01초
	AnimElapsedTime = 0.0f;

	// 시작 값으로 설정
	Percent = AnimStartPercent;
	ApplyStyle();

	// 애니메이션 시작
	bIsAnimating = true;
}

void UCircularProgressBar::StopProgress()
{
	bIsAnimating = false;
}

void UCircularProgressBar::ApplyStyle()
{
	if (!Img_CircularBar)
		return;

	auto DynamicMaterial = Img_CircularBar->GetDynamicMaterial();
	if (DynamicMaterial)
	{
		// Percent 값 설정
		DynamicMaterial->SetScalarParameterValue(TEXT("Percent"), Percent);

		// Percent에 따라 색상 Lerp
		FLinearColor FilledColor;
		if (Percent <= 0.5f)
		{
			// 0% ~ 50%: LowColor → MidColor
			float Alpha = Percent * 2.0f; // 0.0 ~ 1.0으로 정규화
			FilledColor = FLinearColor::LerpUsingHSV(LowColor, MidColor, Alpha);
		}
		else
		{
			// 50% ~ 100%: MidColor → HighColor
			float Alpha = (Percent - 0.5f) * 2.0f; // 0.0 ~ 1.0으로 정규화
			FilledColor = FLinearColor::LerpUsingHSV(MidColor, HighColor, Alpha);
		}

		// FilledColor 파라미터 설정
		DynamicMaterial->SetVectorParameterValue(TEXT("FilledColor"), FilledColor);
	}
}