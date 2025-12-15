// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "UPopup_SpeakQuestJudes.h"

#include "UImageButton.h"
#include "NetworkData.h"
#include "Components/TextBlock.h"
#include "Components/HorizontalBox.h"
#include "Animation/WidgetAnimation.h"
#include "UAutoDespawnItem.h"
#include "FResultStatData.h"
#include "UPopupManager.h"


void UPopup_SpeakQuestJudes::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Ok)
	{
		Btn_Ok->OnButtonClickedEvent.RemoveDynamic(this, &UPopup_SpeakQuestJudes::OnClickOk);
		Btn_Ok->OnButtonClickedEvent.AddDynamic(this, &UPopup_SpeakQuestJudes::OnClickOk);
	}

	if (HideAnim)
	{
		FWidgetAnimationDynamicEvent HideDelegate;
		HideDelegate.BindDynamic(this, &UPopup_SpeakQuestJudes::OnHideAnimComplete);

		BindToAnimationFinished(HideAnim, HideDelegate);
	}
}

void UPopup_SpeakQuestJudes::InitPopup(FResponseSpeakingJudes& Response)
{
	// 피드백 메시지 설정
	if (Txt_Feedback)
	{
		Txt_Feedback->SetText(FText::FromString(Response.final_feedback));
	}

	// 점수 데이터를 아이템으로 추가
	if (ItemHorizontalBox && ItemWidgetClass)
	{
		// 기존 아이템 제거
		ItemHorizontalBox->ClearChildren();

		// 각 점수를 아이템으로 추가
		for (const FResultStatData& StatData : Response.GetResultStatData())
		{
			if (auto ItemWidget = CreateWidget<UAutoDespawnItem>(GetWorld(), ItemWidgetClass))
			{
				ItemWidget->InitData(StatData);
				ItemHorizontalBox->AddChildToHorizontalBox(ItemWidget);
			}
		}
	}

	PlayAnimation(ShowAnim);
}

void UPopup_SpeakQuestJudes::OnClickOk()
{
	PlayAnimation(HideAnim);
}

void UPopup_SpeakQuestJudes::OnHideAnimComplete()
{
	if (UPopupManager* PopupMgr = UPopupManager::Get(GetWorld()))
	{
		PopupMgr->HideCurrentPopup();
	}
}