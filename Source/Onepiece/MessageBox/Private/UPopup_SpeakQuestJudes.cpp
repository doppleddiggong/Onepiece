// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "UPopup_SpeakQuestJudes.h"

#include "UImageButton.h"
#include "NetworkData.h"
#include "FResultStatData.h"
#include "Animation/WidgetAnimation.h"
#include "UDespawnItem.h"
#include "UGameDataManager.h"
#include "ULingoGameHelper.h"
#include "UPopupManager.h"
#include "Components/Image.h"


void UPopup_SpeakQuestJudes::NativeConstruct()
{
	Super::NativeConstruct();

	if (HideAnim)
	{
		FWidgetAnimationDynamicEvent HideDelegate;
		HideDelegate.BindDynamic(this, &UPopup_SpeakQuestJudes::OnHideAnimComplete);

		BindToAnimationFinished(HideAnim, HideDelegate);
	}
}

void UPopup_SpeakQuestJudes::InitPopup(const FResponseSpeakingJudes& Response)
{
	// Grammar Score
	// Context Score
	// Final Overall Score

	TArray<FResultStatData> List = Response.GetResultStatData();
	GrammerItem->InitData( List[0]);
	ContextItem->InitData( List[1]);

	EResourceTextureType TextureType = ULingoGameHelper::ConvertGradeScore(Response.final_overall_score);
	UGameDataManager* DataManager = UGameDataManager::Get(this);
	if (!DataManager)
		return;

	UTexture2D* Texture = DataManager->GetTexture(TextureType);
	if (!Texture)
		return;

	FSlateBrush Brush = Image_Grade->GetBrush();
	Brush.SetResourceObject(Texture);
	Image_Grade->SetBrush(Brush);

	if (GetWorld() && GetWorld()->GetTimerManager().IsTimerActive(LifetimeTimer))
		GetWorld()->GetTimerManager().ClearTimer(LifetimeTimer);
	
	PlayAnimation(ShowAnim);

	// Lifetime 후 FadeOut 시작
	GetWorld()->GetTimerManager().SetTimer(
		LifetimeTimer,
		this,
		&UPopup_SpeakQuestJudes::OnClickOk,
		Lifetime,
		false
	);
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