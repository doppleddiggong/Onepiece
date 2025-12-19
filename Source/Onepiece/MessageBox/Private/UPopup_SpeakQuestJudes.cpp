// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "UPopup_SpeakQuestJudes.h"

#include "UImageButton.h"
#include "NetworkData.h"
#include "Animation/WidgetAnimation.h"
#include "UDespawnItem.h"
#include "UGameDataManager.h"
#include "ULingoGameHelper.h"
#include "UPopupManager.h"
#include "EPopupType.h"
#include "Components/Image.h"


void UPopup_SpeakQuestJudes::NativeConstruct()
{
	Super::NativeConstruct();

	// 이 팝업은 플레이어 조작을 허용
	bAllowPlayerControl = true;

	if (HideAnim)
	{
		// 이 객체에 바인딩된 모든 델리게이트 제거 (중복 방지)
		UnbindAllFromAnimationFinished(HideAnim);

		// 새로 바인딩
		FWidgetAnimationDynamicEvent HideDelegate;
		HideDelegate.BindDynamic(this, &UPopup_SpeakQuestJudes::OnHideAnimComplete);
		BindToAnimationFinished(HideAnim, HideDelegate);
	}
}

void UPopup_SpeakQuestJudes::InitPopup(const FResponseSpeakingJudes& Response)
{
	auto ResultList = Response.GetResultStatData();
	GrammerItem->InitData( ResultList[0]);
	ContextItem->InitData( ResultList[1]);

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
		// HideCurrentPopup() 대신 자기 자신을 명시적으로 닫기
		// (위에 다른 팝업이 있어도 자신만 스택에서 제거됨)
		PopupMgr->HidePopup(EPopupType::SpeakQuestJudes);
	}
}