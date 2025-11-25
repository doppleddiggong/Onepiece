// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "UPopup_MsgBox.h"

#include "UImageButton.h"
#include "UTextureButton.h"
#include "Components/Spacer.h"
#include "Components/TextBlock.h"

void UPopup_MsgBox::NativeConstruct()
{
	Super::NativeConstruct();

	// 중복 바인딩 방지: 기존 바인딩 제거 후 재바인딩
	if (Btn_Close)
	{
		Btn_Close->OnButtonClickedEvent.RemoveDynamic(this, &UPopup_MsgBox::OnClickCancel);
		Btn_Close->OnButtonClickedEvent.AddDynamic(this, &UPopup_MsgBox::OnClickCancel);
	}
	if (Btn_Ok)
	{
		Btn_Ok->OnButtonClickedEvent.RemoveDynamic(this, &UPopup_MsgBox::OnClickOk);
		Btn_Ok->OnButtonClickedEvent.AddDynamic(this, &UPopup_MsgBox::OnClickOk);
	}
	if (Btn_Cancel)
	{
		Btn_Cancel->OnButtonClickedEvent.RemoveDynamic(this, &UPopup_MsgBox::OnClickCancel);
		Btn_Cancel->OnButtonClickedEvent.AddDynamic(this, &UPopup_MsgBox::OnClickCancel);
	}
}

void UPopup_MsgBox::SetTitle(const FString& InTitle) { Txt_Title->SetText(FText::FromString(InTitle)); }
void UPopup_MsgBox::SetDesc(const FString& InDescription) { Txt_Desc->SetText(FText::FromString(InDescription)); }

void UPopup_MsgBox::InitButton(EMsgBoxType InType)
{
	switch (InType)
	{
	case EMsgBoxType::OK:
		Btn_Ok->SetVisibility(ESlateVisibility::Visible);
		Btn_Cancel->SetVisibility(ESlateVisibility::Collapsed);
		Spacing_OkCancel->SetVisibility(ESlateVisibility::Collapsed);
		break;

	case EMsgBoxType::OK_CANCEL:
		Btn_Ok->SetVisibility(ESlateVisibility::Visible);
		Btn_Cancel->SetVisibility(ESlateVisibility::Visible);
		Spacing_OkCancel->SetVisibility(ESlateVisibility::Visible);
		break;
	}
}

void UPopup_MsgBox::OnClickOk()
{
	RemoveFromParent();

	if (OnOkDelegate.IsBound())
		OnOkDelegate.Execute();
}

void UPopup_MsgBox::OnClickCancel()
{
	RemoveFromParent();
	
	if (OnCancelDelegate.IsBound())
		OnCancelDelegate.Execute();
}


void UPopup_MsgBox::InitPopup(
	const FString& InTitle,
	const FString& InDescription,
	EMsgBoxType InType,
	const FOnMsgBoxOkDelegate& InOkDelegate)
{
	this->SetTitle(InTitle);
	this->SetDesc(InDescription);
	this->InitButton(InType);

	OnOkDelegate = InOkDelegate;
	OnCancelDelegate.Unbind();
}

void UPopup_MsgBox::InitPopup(
	const FString& InTitle,
	const FString& InDescription,
	EMsgBoxType InType,
	const FOnMsgBoxOkDelegate& InOkDelegate,
	const FOnMsgBoxCancelDelegate& InCancelDelegate )
{
	this->SetTitle(InTitle);
	this->SetDesc(InDescription);
	this->InitButton(InType);

	OnOkDelegate = InOkDelegate;
	OnCancelDelegate = InCancelDelegate;
}