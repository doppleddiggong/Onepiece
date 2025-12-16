// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "UPopup_SpeakResult.h"

#include "ALingoPlayerState.h"
#include "FResultStatData.h"
#include "UAnswerItem.h"
#include "UImageButton.h"
#include "ULingoGameHelper.h"
#include "UPopupManager.h"
#include "UPopup_SpeakResultItem.h"
#include "UResultStatWidget.h"
#include "Components/Spacer.h"
#include "Components/VerticalBox.h"

void UPopup_SpeakResult::InitPopup(const FResponseSpeakResult& ResponseData)
{
	if (Btn_Confirm)
	{
		Btn_Confirm->OnButtonClickedEvent.RemoveDynamic(this, &UPopup_SpeakResult::OnClickClose);
		Btn_Confirm->OnButtonClickedEvent.AddDynamic(this, &UPopup_SpeakResult::OnClickClose);
	}

	this->SpeakResult = ResponseData;
	
	this->InitScore();
	this->InitQuestionList();
}

void UPopup_SpeakResult::OnClickClose()
{
	if (UPopupManager* PopupMgr = UPopupManager::Get(GetWorld()))
	{
		PopupMgr->HideCurrentPopup();
	}
}

void UPopup_SpeakResult::InitScore() const
{
	auto ResultList = SpeakResult.GetResultStatData();

	Result_Grade->InitData(ResultList[0]);
	Result_TopRate->InitData(ResultList[1]);
	Result_AverageScore->InitData(ResultList[2]);
}

void UPopup_SpeakResult::InitQuestionList()
{
	auto PS= ULingoGameHelper::GetLingoPlayerState(GetWorld());

	VerticalBox->ClearChildren();
	
	for (int32 i = 0; i < SpeakResult.scores.Num(); ++i)
	{
		FSpeakResultItem InItemData;
		InItemData.Index = i+1;
		InItemData.Question = PS->SpeakScenarioData.speak_quest_data[i].eng;
		InItemData.Feedback = SpeakResult.scores[i].desc;
		InItemData.Score = SpeakResult.scores[i].score;

		if( auto ItemWidget = CreateWidget<UPopup_SpeakResultItem>(GetWorld(), AnswerItemClass) )
		{
			ItemWidget->InitData(InItemData);
			VerticalBox->AddChildToVerticalBox(ItemWidget);

			if (i < SpeakResult.scores.Num() - 1)
			{
				USpacer* Spacer = NewObject<USpacer>(this);
				if (Spacer)
				{
					Spacer->SetSize(FVector2D(1.0f, 15.0f));
					VerticalBox->AddChildToVerticalBox(Spacer);
				}
			}
		}
	}
}