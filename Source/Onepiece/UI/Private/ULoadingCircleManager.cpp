// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file ULoadingCircleManager.cpp
 * @brief ULoadingCircleManager의 동작을 구현합니다.
 */
#include "ULoadingCircleManager.h"

#include "FComponentHelper.h"
#include "GameLogging.h"
#include "ULoadginCircle.h"

#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"
#include "Engine/GameInstance.h"
#include "Engine/GameViewportClient.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Onepiece/Onepiece.h"

#define LOADINGCIRCLEWIDGET_PATH TEXT("/Game/CustomContents/UI/WBP_LoadingCircle.WBP_LoadingCircle_C")

ULoadingCircleManager::ULoadingCircleManager()
{
	CircleWidgetClass = FComponentHelper::LoadClass<ULoadginCircle>(LOADINGCIRCLEWIDGET_PATH);
}

void ULoadingCircleManager::EnsureWidgetForWorld(UWorld* World)
{
	if (World == nullptr || !World->IsGameWorld())
		return;

	// 위젯이 유효하고 같은 월드이며, 뷰포트에 추가되어 있는지 확인
	const bool bIsValid = IsValid(CircleWidget);
	const bool bSameWorld = bIsValid && CircleWidget->GetWorld() == World;
	const bool bInViewport = bIsValid && CircleWidget->IsInViewport();

	if (bIsValid && bSameWorld && bInViewport)
		return;

	// 기존 위젯이 있으면 정리
	if (CircleWidget)
	{
		if (CircleWidget->IsInViewport())
			CircleWidget->RemoveFromParent();

		CircleWidget = nullptr;
	}

	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (LocalPlayer == nullptr)
		return;

	// 멀티플레이 대응: PlayerController 사용
	APlayerController* PC = LocalPlayer->GetPlayerController(World);
	if (PC == nullptr)
		return;

	if (!CircleWidgetClass)
		return;

	if (ULoadginCircle* NewWidget = CreateWidget<ULoadginCircle>(PC, CircleWidgetClass))
	{
		// Game Viewport에 추가하여 레벨 전환 시에도 유지
		NewWidget->AddToGameViewport(GameLayer::LoadingCircle);
		CircleWidget = NewWidget;

		// 위젯 재생성 시 현재 카운트에 따라 표시 상태 복원
		if (LoadingCount > 0)
		{
			CircleWidget->Show();
		}
		else
		{
			CircleWidget->Hide();
		}
	}
}

void ULoadingCircleManager::Show()
{
	LoadingCount++;

	PRINTLOG(TEXT("[LoadingCircleManager] Show - Count: %d → %d"), LoadingCount - 1, LoadingCount);

	if (UWorld* World = GetWorld())
	{
		EnsureWidgetForWorld(World);

		if (CircleWidget && LoadingCount > 0)
		{
			CircleWidget->Show();
		}
		else if (!CircleWidget)
		{
			PRINTLOG(TEXT("[LoadingCircleManager] Show FAILED - Widget is nullptr!"));
		}
	}
}

void ULoadingCircleManager::Hide()
{
	const int32 OldCount = LoadingCount;
	LoadingCount = FMath::Max(0, LoadingCount - 1);

	PRINTLOG(TEXT("[LoadingCircleManager] Hide - Count: %d → %d"), OldCount, LoadingCount);

	if (OldCount == 0)
	{
		PRINTLOG(TEXT("[LoadingCircleManager] WARNING: Hide called but count was already 0!"));
	}

	if (UWorld* World = GetWorld())
	{
		EnsureWidgetForWorld(World);

		if (CircleWidget)
		{
			if (LoadingCount == 0)
			{
				CircleWidget->Hide();
			}
		}
		else
		{
			PRINTLOG(TEXT("[LoadingCircleManager] Hide FAILED - Widget is nullptr!"));
		}
	}
}

int32 ULoadingCircleManager::GetLoadingCount() const
{
	return LoadingCount;
}