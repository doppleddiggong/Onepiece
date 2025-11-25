// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "StartWidget.h"

#include "UHoverButton.h"
#include "UPopupManager.h"

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
	if (auto MB = UPopupManager::Get(GetWorld()))
	{
		MB->OpenPopup(EPopupType::Login);
	}
}

void UStartWidget::OnRegistPressed()
{
	if (auto MB = UPopupManager::Get(GetWorld()))
	{
		MB->OpenPopup(EPopupType::Register);
	}
}

