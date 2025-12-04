// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file UAutoDespawnItem.cpp
 * @brief UAutoDespawnItem의 동작을 구현합니다.
 */
#include "UAutoDespawnItem.h"
#include "UResultStatWidget.h"
#include "Animation/WidgetAnimation.h"
#include "GameLogging.h"

void UAutoDespawnItem::NativeConstruct()
{
	Super::NativeConstruct();

	UWorld* World = GetWorld();
	if (!World)
	{
		PRINTLOG( TEXT("UAutoDespawnItem::NativeConstruct - World is null!"));
		return;
	}

	// Lifetime 후 FadeOut 시작
	World->GetTimerManager().SetTimer(
		LifetimeTimer,
		this,
		&UAutoDespawnItem::StartFadeOut,
		Lifetime,
		false
	);
}

void UAutoDespawnItem::NativeDestruct()
{
	// 타이머 정리
	if (UWorld* World = GetWorld())
	{
		if (LifetimeTimer.IsValid())
		{
			World->GetTimerManager().ClearTimer(LifetimeTimer);
		}
	}

	Super::NativeDestruct();
}

void UAutoDespawnItem::StartFadeOut()
{
	if (FadeOutAnim)
	{
		PlayAnimation(FadeOutAnim);

		// 애니메이션 완료 콜백 바인딩
		FWidgetAnimationDynamicEvent AnimFinishedDelegate;
		AnimFinishedDelegate.BindDynamic(this, &UAutoDespawnItem::RemoveSelf);
		BindToAnimationFinished(FadeOutAnim, AnimFinishedDelegate);
	}
	else
	{
		// 애니메이션 없으면 바로 제거
		PRINTLOG( TEXT("UAutoDespawnItem::StartFadeOut - FadeOutAnim is null, removing immediately"));
		RemoveSelf();
	}
}

void UAutoDespawnItem::RemoveSelf()
{
	// 이미 제거 중이면 리턴
	if (!IsInViewport())
		return;

	RemoveFromParent();
}
