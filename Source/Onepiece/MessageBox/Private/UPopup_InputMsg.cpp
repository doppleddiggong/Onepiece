// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "UPopup_InputMsg.h"

#include "GameLogging.h"
#include "UDialogManager.h"
#include "UHoverButton.h"
#include "UImageButton.h"
#include "UKLingoNetworkSystem.h"
#include "UPopupManager.h"
#include "UTextureButton.h"

#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UPopup_InputMsg::NativeConstruct()
{
	Super::NativeConstruct();
}

void UPopup_InputMsg::SetTitle(const FString& InTitle) { Txt_Title->SetText(FText::FromString(InTitle)); }
void UPopup_InputMsg::SetDesc(const FString& InDescription) { Txt_Desc->SetText(FText::FromString(InDescription)); }

void UPopup_InputMsg::OnClickOk()
{
	const FString InputString = Edit_Name->GetText().ToString();

	if ( PopupType == EPopupType::Login  )
		RequestUserToken(InputString);
	else
		RequestUserRegister(InputString);

	// PopupManager를 통해 팝업 닫기 (마우스 커서 처리 포함)
	if (UPopupManager* PopupMgr = UPopupManager::Get(GetWorld()))
	{
		PopupMgr->HideCurrentPopup();
	}
}

void UPopup_InputMsg::OnClickCancel()
{
	// PopupManager를 통해 팝업 닫기 (마우스 커서 처리 포함)
	if (UPopupManager* PopupMgr = UPopupManager::Get(GetWorld()))
	{
		PopupMgr->HideCurrentPopup();
	}
}

void UPopup_InputMsg::InitPopup(const EPopupType InPopupType )
{
	// 중복 바인딩 방지: 기존 바인딩 제거 후 재바인딩
	if (Btn_Close)
	{
		Btn_Close->OnButtonClickedEvent.RemoveDynamic(this, &UPopup_InputMsg::OnClickCancel);
		Btn_Close->OnButtonClickedEvent.AddDynamic(this, &UPopup_InputMsg::OnClickCancel);
	}
	if (Btn_Ok)
	{
		Btn_Ok->OnButtonClickedEvent.RemoveDynamic(this, &UPopup_InputMsg::OnClickOk);
		Btn_Ok->OnButtonClickedEvent.AddDynamic(this, &UPopup_InputMsg::OnClickOk);
	}
	if (Btn_Cancel)
	{
		Btn_Cancel->OnButtonClickedEvent.RemoveDynamic(this, &UPopup_InputMsg::OnClickCancel);
		Btn_Cancel->OnButtonClickedEvent.AddDynamic(this, &UPopup_InputMsg::OnClickCancel);
	}

	this->PopupType = InPopupType;

	if ( PopupType == EPopupType::Login  )
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
		RequestUserMe();
	}
	else
	{
		PRINTLOG(TEXT("--- User Token FAILED ---"));
	}
}

void UPopup_InputMsg::RequestUserMe()
{
	if (auto KLingoNetwork = UKLingoNetworkSystem::Get(GetWorld()))
	{
		KLingoNetwork->RequestUserMe( FResponseUserMeDelegate::CreateUObject(this, &UPopup_InputMsg::OnResponseUserMe) );
	}
	else
	{
		PRINTLOG(TEXT("UKLingoNetworkSystem not found!"));
	}
}

void UPopup_InputMsg::OnResponseUserMe(FResponseUserMe& ResponseData, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		PRINTLOG(TEXT("--- User Me SUCCESS ---"));

		// 로비맵으로 (Seamless Travel로 PlayerState 유지)
		if (UWorld* World = GetWorld())
		{
			UDialogManager::Get(GetWorld())->ShowToast(TEXT("Login Success"));
			
			World->ServerTravel(TEXT("/Game/CustomContents/Maps/LobbyMap?listen"), true, false);
		}
	}
	else
	{
		PRINTLOG(TEXT("--- User Token FAILED ---"));
	}
}
