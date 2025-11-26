// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "APopupTesterActor.h"

#include "EPopupType.h"
#include "UDialogManager.h"
#include "UPopupManager.h"
#include "UPopup_MsgBox.h"

#include "GameLogging.h"
#include "ULingoGameHelper.h"
#include "UPopup_InputMsg.h"
#include "UPopup_ReadQuest.h"


APopupTesterActor::APopupTesterActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void APopupTesterActor::ToastMsg()
{
	UDialogManager::Get(GetWorld())->ShowToast(TEXT("토스트 메세지 샘플"));
}

void APopupTesterActor::OKCancelMsgBox()
{
	UPopupManager::Get(GetWorld())->ShowMsgBox(TEXT("알림"), TEXT("OK_CANCEL 설명문"),
		EMsgBoxType::OK_CANCEL,
		FOnMsgBoxOkDelegate::CreateUObject(this, &APopupTesterActor::OnOK),
		FOnMsgBoxCancelDelegate::CreateUObject(this, &APopupTesterActor::OnCancel));
}

void APopupTesterActor::OKMsgBox()
{
	UPopupManager::Get(GetWorld())->ShowMsgBox(TEXT("알림"), TEXT("OK 설명문"),
		EMsgBoxType::OK,
		FOnMsgBoxOkDelegate::CreateUObject(this, &APopupTesterActor::OnOK));
}

void APopupTesterActor::PopupInputMsg()
{
	if (const auto PopupMgr = UPopupManager::Get(GetWorld()))
	{
		if (const auto Popup = Cast<UPopup_InputMsg>(PopupMgr->ShowPopup(EPopupType::Login)))
		{
			Popup->InitPopup(EPopupType::Login);
		}
	}
}

void APopupTesterActor::ReadQuest()
{
	if (const auto PopupMgr = UPopupManager::Get(GetWorld()))
	{
		if (const auto Popup = Cast<UPopup_ReadQuest>(PopupMgr->ShowPopup(EPopupType::ReadQuest)))
		{
			Popup->InitPopup(ULingoGameHelper::GetLingoGameState(GetWorld())->CurScenarioData);
		}
	}
}

void APopupTesterActor::OnOK()
{
	PRINT_STRING(TEXT("I'M OK"));
}

void APopupTesterActor::OnCancel()
{
	PRINT_STRING(TEXT("I'M Cancel"));
}


