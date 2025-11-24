// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "Onepiece/MessageBox/Public/MessageBox.h"

#include "GameDelegates.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"

void UMessageBox::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_X)
	{
		Btn_X->OnPressed.AddDynamic(this, &UMessageBox::OnCancelPressed);
	}

	if (Btn_Ok)
	{
		Btn_Ok->OnPressed.AddDynamic(this, &UMessageBox::OnOkPressed);
	}

	if (Btn_Cancel)
	{
		Btn_Cancel->OnPressed.AddDynamic(this, &UMessageBox::OnCancelPressed);
	}
}

void UMessageBox::OnOkPressed()
{
	FString UserName = Edt_Name->GetText().ToString();
	OnUserNameRegister.Broadcast(UserName);
	
	RemoveFromParent();
}

void UMessageBox::OnCancelPressed()
{
	RemoveFromParent();
}

void UMessageBox::SetTitle(FString InTitle)
{
	Txt_Title->SetText(FText::FromString(InTitle));
}

void UMessageBox::SetDescription(FString InDescription)
{
	Txt_Description->SetText(FText::FromString(InDescription));
}

void UMessageBox::SetNameFieldVisibility(bool InVisibility)
{
	ESlateVisibility NewVisibility = InVisibility ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;
	
	Txt_Name->SetVisibility(NewVisibility);
	Edt_Name->SetVisibility(NewVisibility);
}
