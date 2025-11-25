// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "LobbyWidget.h"

#include "SessionInfoWidget.h"
#include "UDialogManager.h"
#include "UHoverButton.h"
#include "UImageButton.h"
#include "ULingoGameInstance.h"
#include "UTextureButton.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"

void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	GI = Cast<ULingoGameInstance>(GetGameInstance());
	// 중복 바인딩 방지
	GI->onFindComplete.Unbind();
	GI->onFindComplete.BindUObject(this, &ULobbyWidget::OnFindComplete);

	// Lobby Canvas - 중복 바인딩 방지
	if (Btn_Single)
	{
		Btn_Single->OnButtonClickedEvent.RemoveDynamic(this, &ULobbyWidget::OnSingleClicked);
		Btn_Single->OnButtonClickedEvent.AddDynamic(this, &ULobbyWidget::OnSingleClicked);
	}
	if (Btn_Host)
	{
		Btn_Host->OnButtonClickedEvent.RemoveDynamic(this, &ULobbyWidget::OnHostClicked);
		Btn_Host->OnButtonClickedEvent.AddDynamic(this, &ULobbyWidget::OnHostClicked);
	}
	if (Btn_Join)
	{
		Btn_Join->OnButtonClickedEvent.RemoveDynamic(this, &ULobbyWidget::OnJoinClicked);
		Btn_Join->OnButtonClickedEvent.AddDynamic(this, &ULobbyWidget::OnJoinClicked);
	}
	if (Btn_LogOut)
	{
		Btn_LogOut->OnButtonClickedEvent.RemoveDynamic(this, &ULobbyWidget::OnLogOutClicked);
		Btn_LogOut->OnButtonClickedEvent.AddDynamic(this, &ULobbyWidget::OnLogOutClicked);
	}

	// Create Canvas - 중복 바인딩 방지
	if (Btn_Create)
	{
		Btn_Create->OnButtonClickedEvent.RemoveDynamic(this, &ULobbyWidget::OnCreateClicked);
		Btn_Create->OnButtonClickedEvent.AddDynamic(this, &ULobbyWidget::OnCreateClicked);
	}
	if (EdtTxt_SessionName)
	{
		EdtTxt_SessionName->OnTextChanged.RemoveDynamic(this, &ULobbyWidget::OnValueChangedSessionName);
		EdtTxt_SessionName->OnTextChanged.AddDynamic(this, &ULobbyWidget::OnValueChangedSessionName);
	}

	// Join Canvas - 중복 바인딩 방지
	if (Btn_Update)
	{
		Btn_Update->OnButtonClickedEvent.RemoveDynamic(this, &ULobbyWidget::OnClickFind);
		Btn_Update->OnButtonClickedEvent.AddDynamic(this, &ULobbyWidget::OnClickFind);
	}
}

void ULobbyWidget::OnSingleClicked()
{
	UDialogManager::Get(GetWorld())->ShowToast(TEXT("OnSingleClicked"));
}

void ULobbyWidget::OnHostClicked()
{
	// 세션 생성 화면으로 이동
	widgetSwitcher->SetActiveWidgetIndex(1);
}

void ULobbyWidget::OnJoinClicked()
{
	// 세션 조회 화면으로 이동
	widgetSwitcher->SetActiveWidgetIndex(2);
	// 최초 업데이트
	OnClickFind();
}

void ULobbyWidget::OnLogOutClicked()
{
	UDialogManager::Get(GetWorld())->ShowToast(TEXT("OnLogOutClicked"));
}

void ULobbyWidget::OnCreateClicked()
{
	// 세션 이름
	FString sessionName = EdtTxt_SessionName->GetText().ToString();
	// 세션 생성
	GI->CreateMySession(sessionName);
}

void ULobbyWidget::OnValueChangedSessionName(const FText& Text)
{
	// text 값의 길이에 따른 생성 버튼 활성 / 비활성
	// text.IsEmpty()
	Btn_Create->SetIsEnabled(Text.ToString().Len() > 0);
}

void ULobbyWidget::OnClickFind()
{
	Scrl_SessionList->ClearChildren();
	// 버튼 비활성화
	Txt_Update->SetText(FText::FromString(TEXT("Updating ...")));
	Btn_Update->SetIsEnabled(false);

	GI->FindOtherSession();
}

void ULobbyWidget::OnFindComplete(int32 idx, FString sessionName)
{
	if (idx == -1)
	{
		// 조회 버튼 문구 변경
		Txt_Update->SetText(FText::FromString(TEXT("Update Session")));
		// 조회 버튼 활성화
		Btn_Update->SetIsEnabled(true);	
	}
	else
	{
		// sessionInfoWidget 만들자.
		USessionInfoWidget* item = CreateWidget<USessionInfoWidget>(GetWorld(), sessionInfoWidget);
		// 만들어진 item 을 scrollSessionList 에 추가
		Scrl_SessionList->AddChild(item);
		// 만들어지 item 정보 설정
		item->SetSessionInfo(idx, sessionName);
	}
}