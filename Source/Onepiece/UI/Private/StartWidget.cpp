// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "StartWidget.h"

#include "Components/Button.h"
#include "Onepiece/MessageBox/Public/MessageBox.h"

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
}

void UStartWidget::OnRegistPressed()
{
	if (MessageBoxClass)
	{
		UMessageBox* MessageBoxWidget = CreateWidget<UMessageBox>(GetWorld(), MessageBoxClass);
		if (MessageBoxWidget)
		{
			MessageBoxWidget->AddToViewport();
		}
	}
}
