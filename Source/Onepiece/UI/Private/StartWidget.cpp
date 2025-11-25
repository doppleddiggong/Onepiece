// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "StartWidget.h"

#include "UHoverButton.h"
#include "UPopupManager.h"
#include "UPopup_InputMsg.h"

void UStartWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (HoverBtn_Start)
	{
		HoverBtn_Start->OnButtonClickedEvent.AddDynamic(this, &UStartWidget::OnStartPressed);
	}

	if (HoverBtn_Regist)
	{
		HoverBtn_Regist->OnButtonClickedEvent.AddDynamic(this, &UStartWidget::OnRegistPressed);
	}
}

void UStartWidget::OnStartPressed()
{
	if (const auto PopupMgr = UPopupManager::Get(GetWorld()))
	{
		if (const auto Popup = Cast<UPopup_InputMsg>(PopupMgr->ShowPopup(EPopupType::InputMsg_Login)))
		{
			Popup->InitPopup(EPopupType::InputMsg_Login);
		}
	}
}

void UStartWidget::OnRegistPressed()
{
	if (const auto PopupMgr = UPopupManager::Get(GetWorld()))
	{
		if (const auto Popup = Cast<UPopup_InputMsg>(PopupMgr->ShowPopup(EPopupType::InputMsg_Register)))
		{
			Popup->InitPopup(EPopupType::InputMsg_Register);
		}
	}
}