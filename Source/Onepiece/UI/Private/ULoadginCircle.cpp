// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


/**
 * @file ULoadginCircle.cpp
 * @brief ULoadginCircle의 동작을 구현합니다.
 */
#include "ULoadginCircle.h"

#include "GameLogging.h"


#include "Blueprint/WidgetTree.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Engine/GameViewportClient.h"
#include "Engine/World.h"
#include "HAL/CriticalSection.h"
#include "Misc/DateTime.h"
#include "Misc/ScopeLock.h"
#include "TimerManager.h"

void ULoadginCircle::NativeConstruct()
{
	Super::NativeConstruct();

	UpdateVisibility(false);
}

void ULoadginCircle::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateLoadingSpinner(InDeltaTime);
}

void ULoadginCircle::Show()
{
	UpdateVisibility(true);
}

void ULoadginCircle::Hide()
{
	UpdateVisibility(false);
}

void ULoadginCircle::UpdateLoadingSpinner(float DeltaTime)
{
	if (!LoadingSpinner || !LoadingSpinner->IsVisible())
		return;

	const float NewAngle = LoadingSpinner->GetRenderTransformAngle() + (SpinnerRotationSpeed * DeltaTime);
	LoadingSpinner->SetRenderTransformAngle(NewAngle);
}

void ULoadginCircle::UpdateVisibility(bool bShouldShow)
{
	if (!RootOverlay)
	{
		return;
	}

	if (bShouldShow)
	{
		// 로딩 중일 때: 보이고, 터치 차단
		SetVisibility(ESlateVisibility::Visible);
		RootOverlay->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		// 로딩 완료 시: 안 보이지만 Hit Test 완전히 무시 (터치 이벤트 통과)
		SetVisibility(ESlateVisibility::HitTestInvisible);
		RootOverlay->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void ULoadginCircle::AddToGameViewport(int32 ZOrder)
{
	if (UWorld* World = GetWorld())
	{
		if (UGameViewportClient* ViewportClient = World->GetGameViewport())
		{
			if (IsInViewport())
				RemoveFromParent();

			AddToViewport(ZOrder);
		}
	}
}