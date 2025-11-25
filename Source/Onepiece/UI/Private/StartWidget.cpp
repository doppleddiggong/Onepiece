// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "StartWidget.h"

#include "GameLogging.h"
#include "UHoverButton.h"
#include "UKLingoNetworkSystem.h"
#include "Components/Button.h"
#include "Onepiece/MessageBox/Public/MessageBox.h"
#include "Onepiece/MessageBox/Public/MessageBoxManager.h"

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
	// 로그인
	if (auto MB = UMessageBoxManager::Get(GetWorld()))
	{
		MB->ShowMessageBox(EMessageBoxType::LogIn,"Login", "Please enter your registered name below.");
	}
}

void UStartWidget::OnRegistPressed()
{
	// 가입
	if (auto MB = UMessageBoxManager::Get(GetWorld()))
	{
		MB->ShowMessageBox(EMessageBoxType::Register,"Registeration", "Please enter your name below.");
	}
}

