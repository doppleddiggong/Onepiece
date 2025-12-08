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
#include "UTutorMessage.h"
#include "UAutoDespawnItem.h"
#include "UFadeWidget.h"
#include "Engine/World.h"
#include "Components/Image.h"
#include "Components/HorizontalBox.h"
#include "Engine/Texture2D.h"
#include "Animation/WidgetAnimation.h"
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
	{
		BM->OnUpdateMissionTimerState.AddDynamic(this, &UMainWidget::OnUpdateMissionTimerState);
	}

	StateWidget->InitWidget();
	QuestInfoWidget->SetVisibility( ESlateVisibility::Collapsed );

	// 훅 타겟 인디케이터 초기 숨김
	HookTargetIndicator->SetVisibility(ESlateVisibility::Hidden);

	// SpeakWidget 초기 숨김 (BindWidgetOptional이므로 null 체크 필요)
	SpeakWidget->SetWidgetVisibility(false);

	SetMissionTimerState(false);
}

void UMainWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateTimerDisplay();
	UpdateSpeakWidget();
}

void UMainWidget::SetMissionTimerState(bool bIsActive) const
{
	PlayTimer->SetVisibility(bIsActive ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void UMainWidget::UpdateTimerDisplay() const
{
	if (!CachedGameState || !PlayTimer)
		return;
	
	PlayTimer->UpdateTimerText(ULingoGameHelper::GetFormatTimer(CachedGameState->GetRemainMissionTime()));
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
		CachedGameState->UpdateRemainMissionTime( TimeLimit );

	if (!QuestInfoWidget)
		return;

	QuestInfoWidget->SetVisibility(bIsActive ? ESlateVisibility::Visible : ESlateVisibility::Collapsed );
	HookTargetIndicator->SetVisibility(ESlateVisibility::Visible);
	
	if (bIsActive)
	{
		QuestInfoWidget->InitQuestInfo();
	}

	SetMissionTimerState(bIsActive);
}

void UMainWidget::UpdateHookState(bool bIsAiming)
{
	if (!HookTargetIndicator || !HookTargetIndicator->IsVisible() )
		return;

	// 에임 상태에 따라 이미지 변경
	HookTargetIndicator->SetBrushFromTexture(bIsAiming ? HookAimTexture : HookNoAimTexture );
}

void UMainWidget::UpdateSpeakWidget()
{
	USpeakStageSubsystem* Subsystem = GetWorld()->GetSubsystem<USpeakStageSubsystem>();
	ASpeakStageActor* SpeakStage = Subsystem ? Subsystem->GetSpeakStage() : nullptr;
	APlayerState* CurrentSpeaker = SpeakStage ? SpeakStage->GetCurrentSpeaker() : nullptr;

	const bool bShouldShow = Subsystem && Subsystem->IsInitialized() && SpeakStage && CurrentSpeaker;

	SpeakWidget->SetWidgetVisibility(bShouldShow);

	if (bShouldShow)
	{
		APlayerController* LocalPC = GetWorld()->GetFirstPlayerController();
		if (!LocalPC)
			return;

		APlayerState* LocalPlayerState = LocalPC->GetPlayerState<APlayerState>();
		SpeakWidget->UpdateSpeakStageUI(SpeakStage, LocalPlayerState);
	}
}

void UMainWidget::FadeOut(float Duration)
{
	if (!FadeWidget)
	{
		PRINTLOG(TEXT("UMainWidget::FadeOut - FadeWidget is null"));
		return;
	}

	FadeWidget->FadeOut(Duration);
}

void UMainWidget::FadeIn(float Duration)
{
	if (!FadeWidget)
	{
		PRINTLOG(TEXT("UMainWidget::FadeIn - FadeWidget is null"));
		return;
	}

	FadeWidget->FadeIn(Duration);
}