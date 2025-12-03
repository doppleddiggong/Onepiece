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
#include "ASpeakStageActor.h"
#include "UBroadcastManager.h"
#include "GameLogging.h"
#include "ULingoGameHelper.h"
#include "UQuestInfoWidget.h"
#include "USpeakWidget.h"
#include "USpeakStageSubsystem.h"
#include "Engine/World.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerController.h"

#define AIM_TEXTURE_PATH TEXT("/Game/CustomContents/UI/UITexture/HookAim.HookAim")
#define NO_AIM_TEXTURE_PATH TEXT("/Game/CustomContents/UI/UITexture/NoHookAim.NoHookAim")

UMainWidget::UMainWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UTexture2D> AimTextureFinder(AIM_TEXTURE_PATH);
	if (AimTextureFinder.Succeeded())
		HookAimTexture = AimTextureFinder.Object;

	static ConstructorHelpers::FObjectFinder<UTexture2D> NoAimTextureFinder(NO_AIM_TEXTURE_PATH);
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

	// SpeakWidget 초기 숨김 (BindWidgetOptional이므로 null 체크 필요)
	if (SpeakWidget)
		SpeakWidget->SetWidgetVisibility(false);

	StopMissionTimer();
}

void UMainWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateTimerDisplay();
	UpdateSpeakWidget();
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
	HookTargetIndicator->SetVisibility(ESlateVisibility::Visible);
	
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

void UMainWidget::UpdateHookIndicatorState(bool bIsAiming)
{
	if (!HookTargetIndicator)
		return;

	if ( !HookTargetIndicator->IsVisible() )
		return;

	// 에임 상태에 따라 이미지 변경
	if (bIsAiming)
	{
		// 타겟 감지됨
		HookTargetIndicator->SetBrushFromTexture(HookAimTexture);
	}
	else if (!bIsAiming)
	{
		// 타겟 미감지
		HookTargetIndicator->SetBrushFromTexture(HookNoAimTexture);
	}
}

void UMainWidget::UpdateSpeakWidget()
{
	// SpeakWidget이 없으면 리턴 (BindWidgetOptional)
	if (!SpeakWidget)
		return;

	// World 가져오기
	UWorld* World = GetWorld();
	if (!World)
		return;

	// SpeakStageSubsystem 가져오기
	USpeakStageSubsystem* Subsystem = World->GetSubsystem<USpeakStageSubsystem>();
	if (!Subsystem || !Subsystem->IsInitialized())
	{
		// Subsystem이 없으면 SpeakWidget 숨김
		SpeakWidget->SetWidgetVisibility(false);
		return;
	}

	// SpeakStage 가져오기
	ASpeakStageActor* SpeakStage = Subsystem->GetSpeakStage();
	if (!SpeakStage)
	{
		// SpeakStage가 없으면 SpeakWidget 숨김
		SpeakWidget->SetWidgetVisibility(false);
		return;
	}

	// 현재 발화자 확인
	APlayerState* CurrentSpeaker = SpeakStage->GetCurrentSpeaker();
	if (!CurrentSpeaker)
	{
		// 발화자가 없으면 (Stage 완료) SpeakWidget 숨김
		SpeakWidget->SetWidgetVisibility(false);
		return;
	}

	// SpeakStage가 활성화되어 있으면 SpeakWidget 표시
	SpeakWidget->SetWidgetVisibility(true);

	// 로컬 플레이어의 PlayerState 가져오기
	APlayerController* LocalPC = World->GetFirstPlayerController();
	if (!LocalPC)
		return;

	APlayerState* LocalPlayerState = LocalPC->GetPlayerState<APlayerState>();

	// SpeakWidget UI 업데이트
	SpeakWidget->UpdateSpeakStageUI(SpeakStage, LocalPlayerState);
}