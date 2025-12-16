// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "USpeakWidget.h"

#include "ASpeakStageActor.h"
#include "ALingoPlayerState.h"
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

void USpeakWidget::UpdateSpeakStage(ASpeakStageActor* SpeakStage, APlayerState* LocalPlayerState, int32 StepIndex)
{
	if (!SpeakStage)
	{
		PRINTLOG(TEXT("[SpeakWidget] UpdateSpeakStageUI - SpeakStage is null"));
		return;
	}

	// 서버에서 전달받은 StepIndex를 사용하여 복제 타이밍 문제를 회피
	const int32 TotalSteps = SpeakStage->GetTotalQuestions();

	// PlayerState에서 직접 질문 가져오기
	FString Question;
	if (auto PS = Cast<ALingoPlayerState>(LocalPlayerState))
	{
		if (PS->SpeakScenarioData.speak_quest_data.IsValidIndex(StepIndex))
		{
			Question = PS->SpeakScenarioData.speak_quest_data[StepIndex].GetQuestionMessage();
		}
	}

	if ( Question.IsEmpty())
	{
		SetWidgetVisibility(false);
		PRINTLOG(TEXT("[SpeakWidget] Question is empty for StepIndex %d"), StepIndex);
		return;
	}

	CurrentProgress->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), StepIndex + 1, TotalSteps )));
	CurrentQuestionText->SetText( FText::FromString(*Question));

	PRINTLOG(TEXT("[SpeakWidget] Updated to step %d/%d: %s"), StepIndex + 1, TotalSteps, *Question);
}

void USpeakWidget::SetWidgetVisibility(bool bShow)
{
	SetVisibility(bShow ? ESlateVisibility::Visible :  ESlateVisibility::Collapsed );
}
