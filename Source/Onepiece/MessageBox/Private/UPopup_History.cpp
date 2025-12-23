// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "UPopup_History.h"
#include "UHistoryItem.h"
#include "UPopupManager.h"
#include "UChatHistorySystem.h"
#include "ULingoGameHelper.h"
#include "GameLogging.h"
#include "Components/TextBlock.h"
#include "UImageButton.h"
#include "UTextureButton.h"
#include "Components/ScrollBox.h"
#include "Components/VerticalBox.h"
#include "Components/Spacer.h"
#include "APlayerControl.h"

void UPopup_History::NativeConstruct()
{
	Super::NativeConstruct();

	// 버튼 바인딩
	if (Btn_Close)
	{
		Btn_Close->OnButtonClickedEvent.RemoveDynamic(this, &UPopup_History::OnClickClose);
		Btn_Close->OnButtonClickedEvent.AddDynamic(this, &UPopup_History::OnClickClose);
	}

	if (Btn_Clear)
	{
		Btn_Clear->OnButtonClickedEvent.RemoveDynamic(this, &UPopup_History::OnClickClear);
		Btn_Clear->OnButtonClickedEvent.AddDynamic(this, &UPopup_History::OnClickClear);
	}
}

void UPopup_History::InitPopup()
{
	RefreshHistoryList();
}

void UPopup_History::RefreshHistoryList()
{
	if (!VerticalBox || !Txt_NoData)
	{
		return;
	}

	// 기존 항목 제거
	VerticalBox->ClearChildren();

	// ChatHistorySystem에서 히스토리 로드
	APlayerControl* PC = ULingoGameHelper::GetPlayerControl(GetWorld());
	if (!PC || !PC->GetChatHistorySystem())
	{
		Txt_NoData->SetVisibility(ESlateVisibility::Visible);
		if (ScrollBox)
		{
			ScrollBox->SetVisibility(ESlateVisibility::Hidden);
		}
		return;
	}

	TArray<FChatHistoryItem> HistoryList;
	const int32 Count = PC->GetChatHistorySystem()->LoadAllChatHistory(HistoryList);

	if (Count == 0)
	{
		Txt_NoData->SetVisibility(ESlateVisibility::Visible);
		if (ScrollBox)
		{
			ScrollBox->SetVisibility(ESlateVisibility::Hidden);
		}
		return;
	}

	// 히스토리 아이템 생성 (Index 순서대로 = 상단이 구 데이터, 하단이 신 데이터)
	Txt_NoData->SetVisibility(ESlateVisibility::Hidden);
	if (ScrollBox)
	{
		ScrollBox->SetVisibility(ESlateVisibility::Visible);
	}

	for (int32 i = 0; i < HistoryList.Num(); ++i)
	{
		if (!HistoryItemClass)
		{
			continue;
		}

		UHistoryItem* ItemWidget = CreateWidget<UHistoryItem>(this, HistoryItemClass);
		if (!ItemWidget)
		{
			continue;
		}

		ItemWidget->InitItem(HistoryList[i]);
		VerticalBox->AddChildToVerticalBox(ItemWidget);

		// Spacer 추가 (마지막 항목 제외)
		if (i < HistoryList.Num() - 1)
		{
			USpacer* Spacer = NewObject<USpacer>(VerticalBox);
			Spacer->SetSize(FVector2D(1.0f, ItemSpacing));
			VerticalBox->AddChildToVerticalBox(Spacer);
		}
	}

	PRINTLOG(TEXT("[Popup_History] Refreshed %d items"), Count);
}

void UPopup_History::OnClickClose()
{
	if (UPopupManager* PopupManager = UPopupManager::Get(GetWorld()))
	{
		PopupManager->HideCurrentPopup();
	}
}

void UPopup_History::OnClickClear()
{
	APlayerControl* PC = ULingoGameHelper::GetPlayerControl(GetWorld());
	if (!PC || !PC->GetChatHistorySystem())
	{
		return;
	}

	PC->GetChatHistorySystem()->ClearChatHistory();
	RefreshHistoryList();

	PRINTLOG(TEXT("[Popup_History] Cleared all history"));
}
