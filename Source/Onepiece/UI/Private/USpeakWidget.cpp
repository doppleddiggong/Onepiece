// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "USpeakWidget.h"

#include "ASpeakStageActor.h"
#include "Components/TextBlock.h"
#include "GameLogging.h"

USpeakWidget::USpeakWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void USpeakWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 초기 상태: 모든 텍스트 초기화
	if (CurrentQuestionText)
	{
		CurrentQuestionText->SetText(FText::FromString(TEXT("[1/5] Please wait for your turn")));
	}
}

//----------------------------------------------------------
// Public Interface
//----------------------------------------------------------

void USpeakWidget::UpdateSpeakStage(ASpeakStageActor* SpeakStage, APlayerState* LocalPlayerState)
{
	if (!SpeakStage)
	{
		PRINTLOG(TEXT("[SpeakWidget] UpdateSpeakStageUI - SpeakStage is null"));
		return;
	}

	if (CurrentQuestionText)
	{
		const int32 CurrentStep = SpeakStage->GetCurrentStepIndex();
		const int32 TotalSteps = SpeakStage->GetTotalQuestions();
		const FString Question = SpeakStage->GetCurrentQuestion();

		if (!Question.IsEmpty())
		{
			CurrentQuestionText->SetText(FText::FromString(FString::Printf(TEXT("[%d/%d]\n%s"), CurrentStep + 1, TotalSteps, *Question )));
		}
		else
		{
			CurrentQuestionText->SetText(FText::FromString(TEXT("No question available")));
		}
	}
}

void USpeakWidget::SetWidgetVisibility(bool bShow)
{
	SetVisibility(bShow ? ESlateVisibility::Visible :  ESlateVisibility::Collapsed );
}
