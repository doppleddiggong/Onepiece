// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "StartWidget.h"

#include "GameLogging.h"
#include "UKLingoNetworkSystem.h"
#include "Components/Button.h"
#include "Onepiece/MessageBox/Public/MessageBox.h"
#include "Onepiece/MessageBox/Public/MessageBoxManager.h"

void UStartWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Start)
	{
		Btn_Start->OnPressed.AddDynamic(this, &UStartWidget::OnStartPressed);
	}

	if (Btn_Regist)
	{
		Btn_Regist->OnPressed.AddDynamic(this, &UStartWidget::OnRegistPressed);
	}
}

void UStartWidget::OnStartPressed()
{
	// 로그인
	UMessageBoxManager* MB = UMessageBoxManager::Get(GetWorld());
	if (MB)
	{
		MB->ShowMessageBox(EMessageBoxType::LogIn,"LogIn", "Please enter your registered name below.");
	}
}

void UStartWidget::OnRegistPressed()
{
	// 가입
	UMessageBoxManager* MB = UMessageBoxManager::Get(GetWorld());
	if (MB)
	{
		MB->ShowMessageBox(EMessageBoxType::Register,"Registeration", "Please enter your name below.");
	}
}

