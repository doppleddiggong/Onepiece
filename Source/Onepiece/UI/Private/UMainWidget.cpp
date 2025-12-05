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
		BM->OnTutorMessage.AddDynamic(this, &UMainWidget::OnTutorMessage);
	}

	StateWidget->InitWidget();
	QuestInfoWidget->SetVisibility( ESlateVisibility::Collapsed );

	// 훅 타겟 인디케이터 초기 숨김
	HookTargetIndicator->SetVisibility(ESlateVisibility::Hidden);

	// SpeakWidget 초기 숨김 (BindWidgetOptional이므로 null 체크 필요)
	SpeakWidget->SetWidgetVisibility(false);

	// TutorMessage 초기화 및 애니메이션 콜백 바인딩
	InitTutorMessage();

	SetMissionTimerState(false);
}

void UMainWidget::InitTutorMessage()
{
	if (TutorMessage)
	{
		TutorMessage->SetVisibility(ESlateVisibility::HitTestInvisible);
		
		FWidgetAnimationDynamicEvent HideDelegate;
		HideDelegate.BindDynamic(this, &UMainWidget::OnTutorHideComplete);
		BindToAnimationFinished(TutorHideAnim, HideDelegate);

		PlayAnimation(TutorHideAnim);
		
		bIsTutorVisible = false;
	}
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
		QuestInfoWidget->InitQuestInfo(ULingoGameHelper::GetLingoPlayerState(GetWorld())->QuestRole);
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

void UMainWidget::OnTutorMessage(const FText& NewMessage)
{
	if (!TutorMessage)
		return;

	// 기존 타이머가 있으면 클리어
	if (TutorHideTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(TutorHideTimerHandle);
	}

	if (bIsTutorVisible)
	{
		// 표시 중이면 Hide → SetText → Show
		PendingMessage = NewMessage;
		bHasPendingMessage = true;
		
		PlayAnimation(TutorHideAnim);
	}
	else
	{
		// 숨김 상태면 바로 SetText → Show
		TutorMessage->SetMessageText(NewMessage);
		
		if (TutorShowAnim)
		{
			PlayAnimation(TutorShowAnim);
			bIsTutorVisible = true;

			// 자동 Hide 타이머 시작
			StartTutorHideTimer();
		}
	}
}

void UMainWidget::OnTutorHideComplete()
{
	bIsTutorVisible = false;
	
	if (bHasPendingMessage && TutorMessage)
	{
		bHasPendingMessage = false;
		TutorMessage->SetMessageText(PendingMessage);
		
		if (TutorShowAnim)
		{
			PlayAnimation(TutorShowAnim);
			bIsTutorVisible = true;

			// 자동 Hide 타이머 시작
			StartTutorHideTimer();
		}
	}
}

void UMainWidget::StartTutorHideTimer()
{
	// 기존 타이머 클리어 (안전성을 위해)
	if (TutorHideTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(TutorHideTimerHandle);
	}

	// 지정된 시간 후 자동으로 Hide 애니메이션 재생
	GetWorld()->GetTimerManager().SetTimer(
		TutorHideTimerHandle,
		[this]()
		{
			if (bIsTutorVisible)
			{
				PlayAnimation(TutorHideAnim);
			}
		},
		TutorMessageDisplayDuration,
		false
	);
}

void UMainWidget::AddItemToBoxList(TArray<FResultStatData> InDataList)
{
	if (InDataList.Num() == 0)
		return;

	// 기존 타이머가 있으면 클리어
	if (ItemAddTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(ItemAddTimerHandle);
	}

	// 대기 리스트에 데이터 복사 및 인덱스 초기화
	PendingItemDataList = InDataList;
	CurItemIndex = 0;

	// 첫 번째 아이템은 즉시 추가
	ProcessNextItem();
}


void UMainWidget::AddItemToBoxItem(const FResultStatData& InData)
{
	if (!ItemHorizontalBox || !ItemWidgetClass )
		return;

	// 새 아이템 생성
	if (auto SpawnItem = CreateWidget<UAutoDespawnItem>(GetWorld(), ItemWidgetClass))
	{
		ItemHorizontalBox->AddChild(SpawnItem);

		SpawnItem->InitData(InData);
	}
}

void UMainWidget::ProcessNextItem()
{
	if (CurItemIndex >= PendingItemDataList.Num())
	{
		// 모든 아이템 추가 완료
		PendingItemDataList.Empty();
		CurItemIndex = 0;
		return;
	}

	// 현재 인덱스의 아이템 추가
	AddItemToBoxItem(PendingItemDataList[CurItemIndex]);
	CurItemIndex++;

	// 다음 아이템이 있으면 타이머 설정
	if (CurItemIndex < PendingItemDataList.Num())
	{
		GetWorld()->GetTimerManager().SetTimer(
			ItemAddTimerHandle,
			this,
			&UMainWidget::ProcessNextItem,
			ItemAddInterval,
			false
		);
	}
	else
	{
		// 모든 아이템 추가 완료
		PendingItemDataList.Empty();
		CurItemIndex = 0;
	}
}