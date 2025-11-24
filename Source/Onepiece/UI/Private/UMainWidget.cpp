// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file UMainWidget.cpp
 * @brief UMainWidget의 동작을 구현합니다.
 */
#include "UMainWidget.h"
#include "Input/Reply.h"
#include "ALingoGameState.h"
#include "GameLogging.h"
#include "ULingoGameHelper.h"
#include "Components/TextBlock.h"
#include "Engine/World.h"


UMainWidget::UMainWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
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
			PRINTLOG( TEXT("[MainWidget] GameState cached successfully"));
		}
		else
		{
			PRINTLOG( TEXT("[MainWidget] Failed to cache GameState"));
		}
	}
}

void UMainWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateTimerDisplay();
}

void UMainWidget::UpdateTimerDisplay()
{
	if (!CachedGameState || !RemainPlayTimeText)
		return;

	// GameState의 시간을 가져와서 UI 업데이트
	FString TimeText = ULingoGameHelper::GetFormatTimer(CachedGameState->RemainMissionTime);
	RemainPlayTimeText->SetText(FText::FromString(TimeText));
}