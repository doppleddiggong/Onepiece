// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file UMainWidget.cpp
 * @brief UMainWidget의 동작을 구현합니다.
 */
#include "UMainWidget.h"
#include "Input/Reply.h"
#include "UPlayTimer.h"
#include "UStateWidget.h"
#include "UVoiceRecording.h"
#include "ALingoGameState.h"
#include "ALingoPlayerState.h"
#include "ASpeakStageActor.h"
#include "ChatWidget.h"
#include "CompassWidget.h"
#include "ULingoGameHelper.h"
#include "UBroadcastManager.h"
#include "GameLogging.h"
#include "QuestOrderWidget.h"
#include "UQuestInfoWidget.h"
#include "USpeakWidget.h"
#include "UFadeWidget.h"
#include "URoomWidget.h"
#include "Engine/World.h"
#include "Components/Image.h"
#include "Components/WidgetSwitcher.h"
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
	StateWidget->SetVisibility( ESlateVisibility::Collapsed);

	RoomWidget->InitWidget();
	RoomWidget->SetVisibility( ESlateVisibility::Collapsed);
	
	QuestInfoWidget->SetVisibility( ESlateVisibility::Collapsed );

	CompassWidget->SetVisibility( ESlateVisibility::Collapsed );

	VoiceRecording->InitWidget();
	VoiceRecording->SetVisibility( ESlateVisibility::Collapsed);
	
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
}

void UMainWidget::SetMissionTimerState(bool bIsActive) const
{
	PlayTimer->SetVisibility(bIsActive ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void UMainWidget::UpdateStateWidget(int32 UserId, const FString& UserName)
{
	if ( UserName == "" )
		return;

	StateWidget->SetVisibility(ESlateVisibility::Visible);
	StateWidget->UpdateUserName(UserId, UserName);
}

void UMainWidget::UpdateRoomWidget(int32 InRoomLevel, int64 InRoomId)
{
	if ( InRoomId <= 0 )
		return;

	RoomWidget->SetVisibility(ESlateVisibility::Visible);
	RoomWidget->UpdateRoomInfo(InRoomLevel, InRoomId);
}

void UMainWidget::SendChatMessage(FResponseUserMe sendUser, const FText& inMessage, int32 PlayerIndex)
{
	// PRINTLOG(TEXT("[SendChat] UMainWidget::SendChatMessage - %s"), *inMessage.ToString());
	ChatWidget->SendMessage(sendUser, inMessage, PlayerIndex);
}

void UMainWidget::SetFocusOnChat()
{
	ChatWidget->FocusInput();
}

void UMainWidget::UpdateTimerDisplay() const
{
	if (!CachedGameState || !PlayTimer)
		return;
	
	PlayTimer->UpdateTimerText( CachedGameState->GetRemainMissionTime());
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

	// 타이머를 멈춰야 하는 경우 (결과 팝업 등)
	if (CachedGameState && !bIsActive && TimeLimit == 0.0f)
	{
		CachedGameState->StopMissionTimer();
	}

	// if (!QuestInfoWidget)
	// 	return;

	// QuestInfoWidget->SetVisibility(bIsActive ? ESlateVisibility::Visible : ESlateVisibility::Collapsed );
	HookTargetIndicator->SetVisibility(ESlateVisibility::Visible);

	if (bIsActive)
	{
		WidgetSwitcher->SetActiveWidgetIndex(0);
		// QuestInfoWidget->InitQuestInfo();
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

void UMainWidget::UpdateSpeakWidget(int32 StepIndex)
{
	WidgetSwitcher->SetActiveWidgetIndex(1);

	ASpeakStageActor* SpeakStage = nullptr;
	ALingoPlayerState* LocalPlayerState = nullptr;

	const bool bCanShow =
		CanShowSpeakWidget(SpeakStage, LocalPlayerState);
	SpeakWidget->SetWidgetVisibility(bCanShow);

	if ( bCanShow )
		SpeakWidget->UpdateSpeakStage(SpeakStage, LocalPlayerState, StepIndex);
}

bool UMainWidget::CanShowSpeakWidget( ASpeakStageActor*& OutSpeakStage,	ALingoPlayerState*& OutLocalPlayerState) const
{
	OutSpeakStage = ULingoGameHelper::GetSpeakStageActor(this);
	if (!OutSpeakStage)
		return false;

	ALingoPlayerState* CurrentSpeaker = OutSpeakStage->GetCurrentSpeaker();
	if (!CurrentSpeaker)
		return false;

	APlayerController* LocalPC = GetWorld()->GetFirstPlayerController();
	if (!LocalPC)
		return false;

	OutLocalPlayerState = LocalPC->GetPlayerState<ALingoPlayerState>();
	if (!OutLocalPlayerState)
		return false;

	return (OutLocalPlayerState == CurrentSpeaker);
}

void UMainWidget::UpdateChatWidget()
{
	ChatWidget->SetVisibility(ESlateVisibility::Visible);
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