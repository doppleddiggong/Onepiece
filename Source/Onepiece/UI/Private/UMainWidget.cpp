// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file UMainWidget.cpp
 * @brief UMainWidget의 동작을 구현합니다.
 */
#include "UMainWidget.h"
#include "Input/Reply.h"
#include "UPlayTimer.h"
#include "UStateWidget.h"
#include "ALingoGameState.h"
#include "UBroadcastManager.h"
#include "GameLogging.h"
#include "ULingoGameHelper.h"
#include "Engine/World.h"

UMainWidget::UMainWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UMainWidget::StartMissionTimer()
{
	PlayTimer->SetVisibility(ESlateVisibility::Visible);
}

void UMainWidget::StopMissionTimer()
{
	PlayTimer->SetVisibility(ESlateVisibility::Hidden);
}

void UMainWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// GameState 참조 가져오기
	if (UWorld* World = GetWorld())
	{
		CachedGameState = World->GetGameState<ALingoGameState>();

		if (CachedGameState)
		{
			PRINTLOG(TEXT("[MainWidget] GameState cached successfully"));
		}
		else
		{
			PRINTLOG(TEXT("[MainWidget] Failed to cache GameState"));
		}
	}

	// BroadcastManager 이벤트 구독
	if (UBroadcastManager* BroadcastManager = UBroadcastManager::Get(GetWorld()))
	{
		BroadcastManager->OnMissionTimerStateChanged.AddDynamic(this, &UMainWidget::OnMissionTimerStateChanged);
		PRINTLOG(TEXT("[MainWidget] Subscribed to OnMissionTimerStateChanged"));
	}

	StateWidget->InitWidget();

	StopMissionTimer();
}

void UMainWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateTimerDisplay();
}

void UMainWidget::UpdateTimerDisplay()
{
	if (!CachedGameState || !PlayTimer)
		return;

	// GameState의 시간을 가져와서 PlayTimer 업데이트
	FString TimeText = ULingoGameHelper::GetFormatTimer(CachedGameState->RemainMissionTime);
	PlayTimer->UpdateTimerText(TimeText);
}

void UMainWidget::OnMissionTimerStateChanged(bool bIsActive)
{
	PRINTLOG(TEXT("[MainWidget] Mission Timer State Changed - bIsActive: %s"), bIsActive ? TEXT("true") : TEXT("false"));

	if (bIsActive)
	{
		StartMissionTimer();
	}
	else
	{
		StopMissionTimer();
	}
}