// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "UBasePopup.h"
#include "UGameSoundManager.h"

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

	// 팝업 오픈 사운드 재생 (None이 아닐 때만)
	if (OpenAnimSound != EGameSoundType::None)
	{
		if (UGameSoundManager* SoundManager = UGameSoundManager::Get(this))
		{
			SoundManager->PlaySound2D(OpenAnimSound);
		}
	}

	// 블루프린트에서 정의한 애니메이션이 있으면 해당 애니메이션 재생
	if (BlueprintOpenAnimation)
	{
		PlayAnimation(BlueprintOpenAnimation);
		return;
	}

	// 블루프린트 애니메이션이 없으면 기본 스크립트 애니메이션 사용
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
	const float EasedAlpha = FEaseHelper::Ease(Alpha, OpenEaseType);
	const float NewScale = FMath::Lerp(OpenStartScale, OpenTargetScale, EasedAlpha);

	SetRenderScale(FVector2D(NewScale, NewScale));

	if (Alpha >= 1.0f)
	{
		bIsOpenAnim = false;
	}
}