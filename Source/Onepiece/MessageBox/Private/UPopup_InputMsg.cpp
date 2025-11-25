// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "UPopup_InputMsg.h"

#include "GameLogging.h"
#include "UHoverButton.h"
#include "UImageButton.h"
#include "UKLingoNetworkSystem.h"
#include "UTextureButton.h"

#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UPopup_InputMsg::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Close)	Btn_Close->OnButtonClickedEvent.AddDynamic(this, &UPopup_InputMsg::OnClickCancel);
	if (Btn_Ok)		Btn_Ok->OnButtonClickedEvent.AddDynamic(this, &UPopup_InputMsg::OnClickOk);
	if (Btn_Cancel) Btn_Cancel->OnButtonClickedEvent.AddDynamic(this, &UPopup_InputMsg::OnClickCancel);
}

void UPopup_InputMsg::SetTitle(const FString& InTitle) { Txt_Title->SetText(FText::FromString(InTitle)); }
void UPopup_InputMsg::SetDesc(const FString& InDescription) { Txt_Desc->SetText(FText::FromString(InDescription)); }

void UPopup_InputMsg::OnClickOk()
{
	const FString InputString = Edit_Name->GetText().ToString();

	if ( PopupType == EPopupType::Login )
		RequestUserRegister(InputString);
	else
		RequestUserToken(InputString);
	
	RemoveFromParent();
}

void UPopup_InputMsg::OnClickCancel()
{
	RemoveFromParent();
}

void UPopup_InputMsg::InitPopup(const EPopupType InPopupType )
{
	this->PopupType = InPopupType;

	if ( PopupType == EPopupType::Login )
	{
		this->SetTitle("Login");
		this->SetDesc("Please enter your registered name below.");	
	}
	else
	{
		this->SetTitle("Registeration");
		this->SetDesc("Please enter your name below.");	
	}
}

void UPopup_InputMsg::RequestUserRegister(const FString& UserInput)
{
	if (UserInput.IsEmpty())
		return;

	if (auto KLingoNetwork = UKLingoNetworkSystem::Get(GetWorld()))
	{
		PRINTLOG(TEXT("[TEST] RequestUserRegister - UserName: %s"), *UserInput);
		KLingoNetwork->RequestUserRegister(
			UserInput,
			FResponseUserRegisterDelegate::CreateUObject(this,
				&UPopup_InputMsg::OnResponseUserRegister)
		);
	}
	else
	{
		PRINTLOG(TEXT("UKLingoNetworkSystem not found!"));
	}
}

void UPopup_InputMsg::RequestUserToken(const FString& UserInput)
{
	if (UserInput.IsEmpty())
		return;
	
	if (auto KLingoNetwork = UKLingoNetworkSystem::Get(GetWorld()))
	{
		PRINTLOG(TEXT("[TEST] RequestUserToken - UserName: %s"), *UserInput);
		KLingoNetwork->RequestUserToken(
			UserInput,
			FResponseUserTokenDelegate::CreateUObject(this, &UPopup_InputMsg::OnResponseUserToken)
		);
	}
	else
	{
		PRINTLOG(TEXT("UKLingoNetworkSystem not found!"));
	}
	
}

void UPopup_InputMsg::OnResponseUserRegister(FResponseUserRegister& ResponseData, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		PRINTLOG(TEXT("--- User Register SUCCESS ---"));
		ResponseData.PrintData();
		
		// 가입 성공 시 토큰 발급
		RequestUserToken(ResponseData.username);
	}
	else
	{
		PRINTLOG(TEXT("--- User Register FAILED ---"));
	}
}

void UPopup_InputMsg::OnResponseUserToken(FResponseUserToken& ResponseData, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		PRINTLOG(TEXT("--- User Token SUCCESS ---"));
		ResponseData.PrintData();
		PRINTLOG(TEXT("Token: %s"), *ResponseData.access_token);
		
		// 토큰 발급 성공 시 로비맵으로
		UGameplayStatics::OpenLevel(GetWorld(), TEXT("LobbyMap"));
	}
	else
	{
		PRINTLOG(TEXT("--- User Token FAILED ---"));
	}
}