// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "Onepiece/MessageBox/Public/MessageBox.h"

#include "GameDelegates.h"
#include "UHoverButton.h"
#include "UImageButton.h"
#include "UTextureButton.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"

void UMessageBox::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Close)
	{
		Btn_Close->OnButtonClickedEvent.AddDynamic(this, &UMessageBox::OnClickCancel);
	}

	if (Btn_Ok)
	{
		Btn_Ok->OnButtonClickedEvent.AddDynamic(this, &UMessageBox::OnClickOk);
	}

	if (Btn_Cancel)
	{
		Btn_Cancel->OnButtonClickedEvent.AddDynamic(this, &UMessageBox::OnClickCancel);
	}
}


void UMessageBox::SetTitle(const FString& InTitle)
{
	Txt_Title->SetText(FText::FromString(InTitle));
}

void UMessageBox::SetDesc(const FString& InDescription)
{
	Txt_Desc->SetText(FText::FromString(InDescription));
}

void UMessageBox::OnClickOk()
{
	FString UserName = Edit_Name->GetText().ToString();
	OnUserNameRegister.Broadcast(UserName);
	
	RemoveFromParent();
}

void UMessageBox::OnClickCancel()
{
	RemoveFromParent();
}

void UMessageBox::SetNameFieldVisibility(bool InVisibility)
{
	Edit_Name->SetVisibility(InVisibility ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}