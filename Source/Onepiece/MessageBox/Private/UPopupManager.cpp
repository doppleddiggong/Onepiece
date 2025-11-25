// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "UPopupManager.h"

#include "UPopup_InputMsg.h"

#include "FComponentHelper.h"
#include "GameLogging.h"
#include "UKLingoNetworkSystem.h"
#include "Onepiece/Onepiece.h"

#define MESSAGEBOX_PATH TEXT("/Game/CustomContents/UI/Widgets/WBP_MessageBox.WBP_MessageBox_C")

UPopupManager::UPopupManager()
{
	PopupClass = FComponentHelper::LoadClass<UPopup_InputMsg>(MESSAGEBOX_PATH);
}

void UPopupManager::ShowMessageBox(
	const FString& InTitle,
	const FString& InDescription,
	EMsgBoxType InType,
	const FOnMsgBoxOkDelegate& InOkDelegate)
{
	Popup_MsgBox->InitPopup(InTitle, InDescription, InType, InOkDelegate);
}

void UPopupManager::ShowMessageBox(
	const FString& InTitle,
	const FString& InDescription,
	EMsgBoxType InType,
	const FOnMsgBoxOkDelegate& InOkDelegate,
	const FOnMsgBoxCancelDelegate& InCancelDelegate )
{
	Popup_MsgBox->InitPopup(InTitle, InDescription, InType, InOkDelegate, InCancelDelegate);
}


void UPopupManager::OpenPopup(const EPopupType InType)
{
	UWorld* World = GetWorld();
	if (!World)
		return;

	EnsureWidgetForWorld(World);
	// 텍스트 설정
	if (Popup_InputMsg)
	{
		// 타입 별로 다르게 처리
		switch (InType)
		{
		case EPopupType::Register:
		case EPopupType::Login:
			{
				Popup_InputMsg->InitPopup(InType);
			}
			break;

		default:
			{
				PRINTLOG( TEXT("undefined Popup Type : %s"), *ENUM_TO_NAME(EPopupType, InType));
			}
		}
	}
}

void UPopupManager::HideMessageBox()
{
	UWorld* World = GetWorld();
	
	if (!World)
		return;

	if (!Popup_InputMsg)
		return;

	if (Popup_InputMsg->GetWorld() != World)
		return;

	Popup_InputMsg->RemoveFromParent();
}

void UPopupManager::EnsureWidgetForWorld(UWorld* World)
{
	if (World == nullptr || !World->IsGameWorld())
		return;
	
	// 위젯이 유효하고 같은 월드이며, 뷰포트에 추가되어 있는지 확인
	if (IsValid(Popup_InputMsg) &&
		Popup_InputMsg->GetWorld() == World &&
		Popup_InputMsg->IsInViewport())
	{
		return;
	}

	// 기존 위젯이 있으면 정리
	if (Popup_InputMsg)
	{
		if (Popup_InputMsg->IsInViewport())
			Popup_InputMsg->RemoveFromParent();
		
		Popup_InputMsg = nullptr;
	}

	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (LocalPlayer == nullptr)
		return;
	
	APlayerController* PC = LocalPlayer->GetPlayerController(World);
	if (PC == nullptr)
		return;
	
	if (!PopupClass)
		return;
	
	UPopup_InputMsg* NewWidget = CreateWidget<UPopup_InputMsg>(PC, PopupClass);
	if (!NewWidget)
		return;
	
	NewWidget->AddToViewport(GameLayer::Popup);
	
	Popup_InputMsg = NewWidget;
}