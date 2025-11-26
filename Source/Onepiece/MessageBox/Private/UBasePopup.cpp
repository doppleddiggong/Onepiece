// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "UBasePopup.h"

void UBasePopup::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bIsOpenAnim)
	{
		UpdateAnimation(InDeltaTime);
	}
}

void UBasePopup::OpenAnimation()
{
	if (!IsInViewport())
	{
		AddToViewport();
	}

	SetRenderTransformPivot(OpenPivot);
	SetRenderScale(FVector2D(OpenStartScale, OpenStartScale));

	bIsOpenAnim = OpenDuration > KINDA_SMALL_NUMBER;
	OpenElapsedTime = 0.0f;

	if (!bIsOpenAnim)
	{
		SetRenderScale(FVector2D(OpenTargetScale, OpenTargetScale));
	}
}

void UBasePopup::UpdateAnimation(float InDeltaTime)
{
	if (!bIsOpenAnim)
	{
		return;
	}

	OpenElapsedTime += InDeltaTime;

	const float Alpha = FMath::Clamp(OpenElapsedTime / OpenDuration, 0.0f, 1.0f);
	const float EasedAlpha = FMath::InterpEaseOut(0.0f, 1.0f, Alpha, OpenEaseExponent);
	const float NewScale = FMath::Lerp(OpenStartScale, OpenTargetScale, EasedAlpha);

	SetRenderScale(FVector2D(NewScale, NewScale));

	if (Alpha >= 1.0f)
	{
		bIsOpenAnim = false;
	}
}