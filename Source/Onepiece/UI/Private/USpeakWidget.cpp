// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "USpeakWidget.h"
#include "ASpeakStageActor.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"
#include "GameLogging.h"

USpeakWidget::USpeakWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// 기본 설정
	CachedSpeakStage = nullptr;
	CachedLocalPlayerState = nullptr;
}

void USpeakWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 초기 상태: 모든 텍스트 초기화
	if (CurrentSpeakerText)
	{
		CurrentSpeakerText->SetText(FText::FromString(TEXT("Waiting...")));
	}

	if (CurrentQuestionText)
	{
		CurrentQuestionText->SetText(FText::FromString(TEXT("Please wait for your turn")));
	}

	if (ProgressText)
	{
		ProgressText->SetText(FText::FromString(TEXT("0/0")));
	}

	if (QueuePositionText)
	{
		QueuePositionText->SetText(FText::FromString(TEXT("")));
	}
}

void USpeakWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// 캐시된 SpeakStage가 있으면 자동 업데이트
	if (CachedSpeakStage && CachedLocalPlayerState)
	{
		UpdateSpeakStageUI(CachedSpeakStage, CachedLocalPlayerState);
	}
}

//----------------------------------------------------------
// Public Interface
//----------------------------------------------------------

void USpeakWidget::UpdateSpeakStageUI(ASpeakStageActor* SpeakStage, APlayerState* LocalPlayerState)
{
	if (!SpeakStage)
	{
		PRINTLOG(TEXT("[SpeakWidget] UpdateSpeakStageUI - SpeakStage is null"));
		return;
	}

	// 캐싱
	CachedSpeakStage = SpeakStage;
	CachedLocalPlayerState = LocalPlayerState;

	// 1. 현재 발화자 표시
	if (CurrentSpeakerText)
	{
		APlayerState* CurrentSpeaker = SpeakStage->GetCurrentSpeaker();
		if (CurrentSpeaker)
		{
			FString SpeakerName = CurrentSpeaker->GetPlayerName();

			// 내 턴인지 확인
			if (LocalPlayerState && CurrentSpeaker == LocalPlayerState)
			{
				CurrentSpeakerText->SetText(FText::FromString(
					FString::Printf(TEXT("Your Turn: %s"), *SpeakerName)
				));
			}
			else
			{
				CurrentSpeakerText->SetText(FText::FromString(
					FString::Printf(TEXT("Current Speaker: %s"), *SpeakerName)
				));
			}
		}
		else
		{
			CurrentSpeakerText->SetText(FText::FromString(TEXT("Waiting for players...")));
		}
	}

	// 2. 현재 질문 표시
	if (CurrentQuestionText)
	{
		FString Question = SpeakStage->GetCurrentQuestion();
		if (!Question.IsEmpty())
		{
			CurrentQuestionText->SetText(FText::FromString(Question));
		}
		else
		{
			CurrentQuestionText->SetText(FText::FromString(TEXT("No question available")));
		}
	}

	// 3. 진행률 표시 (예: 3/5)
	if (ProgressText)
	{
		int32 CurrentStep = SpeakStage->GetCurrentStepIndex();
		int32 TotalSteps = SpeakStage->GetTotalQuestions();

		// 질문이 있으면 1부터 시작 (사용자 친화적)
		if (TotalSteps > 0)
		{
			ProgressText->SetText(FText::FromString(
				FString::Printf(TEXT("Progress: %d/%d"), CurrentStep + 1, TotalSteps)
			));
		}
		else
		{
			ProgressText->SetText(FText::FromString(TEXT("Progress: 0/0")));
		}
	}

	// 4. 대기 순서 표시 (선택사항)
	if (QueuePositionText && LocalPlayerState)
	{
		APlayerState* CurrentSpeaker = SpeakStage->GetCurrentSpeaker();

		// 내 턴이 아니면 대기 메시지 표시
		if (CurrentSpeaker && CurrentSpeaker != LocalPlayerState)
		{
			QueuePositionText->SetText(FText::FromString(TEXT("Waiting for your turn...")));
		}
		else if (CurrentSpeaker && CurrentSpeaker == LocalPlayerState)
		{
			QueuePositionText->SetText(FText::FromString(TEXT("It's your turn! Speak now.")));
		}
		else
		{
			QueuePositionText->SetText(FText::FromString(TEXT("")));
		}
	}
}

void USpeakWidget::SetWidgetVisibility(bool bShow)
{
	if (bShow)
	{
		SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		SetVisibility(ESlateVisibility::Collapsed);
	}
}
