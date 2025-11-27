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
#include "ALingoPlayerState.h"
#include "UBroadcastManager.h"
#include "GameLogging.h"
#include "ULingoGameHelper.h"
#include "UQuestInfoWidget.h"
#include "Engine/World.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"

#define AIM_TEXTURE_PATH TEXT("/Game/CustomContents/UI/UITexture/HookAim.HookAim")
#define NOAIM_TEXTURE_PATH TEXT("/Game/CustomContents/UI/UITexture/NoHookAim.NoHookAim")

UMainWidget::UMainWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UTexture2D> AimTextureFinder(AIM_TEXTURE_PATH);
	if (AimTextureFinder.Succeeded())
		HookAimTexture = AimTextureFinder.Object;
	static ConstructorHelpers::FObjectFinder<UTexture2D> NoAimTextureFinder(NOAIM_TEXTURE_PATH);
	if (NoAimTextureFinder.Succeeded())
		HookNoAimTexture = NoAimTextureFinder.Object;
}


void UMainWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// GameState 참조 가져오기
	if (UWorld* World = GetWorld())
		CachedGameState = World->GetGameState<ALingoGameState>();

	if (auto BM = UBroadcastManager::Get(GetWorld()))
		BM->OnUpdateMissionTimerState.AddDynamic(this, &UMainWidget::OnUpdateMissionTimerState);

	StateWidget->InitWidget();
	QuestInfoWidget->SetVisibility( ESlateVisibility::Collapsed );

	// 훅 타겟 인디케이터 초기 숨김
	if (HookTargetIndicator)
		HookTargetIndicator->SetVisibility(ESlateVisibility::Hidden);

	StopMissionTimer();
}

void UMainWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateTimerDisplay();
}

void UMainWidget::StartMissionTimer() const
{
	PlayTimer->SetVisibility(ESlateVisibility::Visible);
}

void UMainWidget::StopMissionTimer() const
{
	PlayTimer->SetVisibility(ESlateVisibility::Hidden);
}

void UMainWidget::UpdateTimerDisplay()
{
	if (!CachedGameState || !PlayTimer)
		return;
	
	PlayTimer->UpdateTimerText(ULingoGameHelper::GetFormatTimer(CachedGameState->RemainMissionTime));
}

void UMainWidget::OnUpdateMissionTimerState(bool bIsActive, float TimeLimit)
{
	if (!CachedGameState)
	{
		if (UWorld* World = GetWorld())
			CachedGameState = World->GetGameState<ALingoGameState>();
	}

	// GameState의 RemainMissionTime 업데이트
	if (CachedGameState && bIsActive && TimeLimit > 0.0f)
		CachedGameState->RemainMissionTime = TimeLimit;

	
	if (!QuestInfoWidget)
		return;

	QuestInfoWidget->SetVisibility(bIsActive ? ESlateVisibility::Visible : ESlateVisibility::Collapsed );

	if (bIsActive)
	{
		StartMissionTimer();
		QuestInfoWidget->InitQuestInfo(ULingoGameHelper::GetLingoPlayerState(GetWorld())->QuestRole);
	}
	else
	{
		StopMissionTimer();
	}
}

void UMainWidget::SetHookTargetVisible(bool bVisible)
{
	if (HookTargetIndicator)
	{
		HookTargetIndicator->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

void UMainWidget::UpdateHookIndicatorState(bool bIsAiming)
{
	if (!HookTargetIndicator)
		return;

	// 항상 표시
	HookTargetIndicator->SetVisibility(ESlateVisibility::Visible);

	// 에임 상태에 따라 이미지 변경
	if (bIsAiming && HookAimTexture)
	{
		// 타겟 감지됨 - 파란색 이미지
		HookTargetIndicator->SetBrushFromTexture(HookAimTexture);
	}
	else if (!bIsAiming && HookNoAimTexture)
	{
		// 타겟 미감지 - 회색 이미지
		HookTargetIndicator->SetBrushFromTexture(HookNoAimTexture);
	}
}