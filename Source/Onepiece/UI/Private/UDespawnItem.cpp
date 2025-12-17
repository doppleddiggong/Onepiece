// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "UDespawnItem.h"
#include "UResultStatWidget.h"
#include "Animation/WidgetAnimation.h"
#include "GameLogging.h"

void UDespawnItem::NativeConstruct()
{
	Super::NativeConstruct();

	UWorld* World = GetWorld();
	if (!World)
	{
		PRINTLOG( TEXT("UAutoDespawnItem::NativeConstruct - World is null!"));
		return;
	}
}

void UDespawnItem::NativeDestruct()
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

void UDespawnItem::InitData(const FResultStatData& InData)
{
	ItemWidget->InitData(InData);
}

void UDespawnItem::StartFadeOut()
{
	if (FadeOutAnim)
	{
		PlayAnimation(FadeOutAnim);

		// 애니메이션 완료 콜백 바인딩
		FWidgetAnimationDynamicEvent AnimFinishedDelegate;
		AnimFinishedDelegate.BindDynamic(this, &UDespawnItem::RemoveSelf);
		BindToAnimationFinished(FadeOutAnim, AnimFinishedDelegate);
	}
	else
	{
		// 애니메이션 없으면 바로 제거
		RemoveSelf();
	}
}

void UDespawnItem::RemoveSelf()
{
	// 이미 제거 중이면 리턴
	if (!IsInViewport())
		return;

	RemoveFromParent();
}