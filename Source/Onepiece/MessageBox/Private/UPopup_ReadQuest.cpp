// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "UPopup_ReadQuest.h"
#include "UPopup_ReadQuestItem.h"
#include "ALingoGameState.h"
#include "ALingoPlayerState.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerController.h"
#include "GameLogging.h"
#include "ULingoGameHelper.h"
#include "UWordWidget.h"
#include "Components/Button.h"


UPopup_ReadQuest::UPopup_ReadQuest(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UPopup_ReadQuest::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Close)
	{
		Btn_Close->OnClicked.RemoveDynamic(this, &UPopup_ReadQuest::OnClickClose);
		Btn_Close->OnClicked.AddDynamic(this, &UPopup_ReadQuest::OnClickClose);
	}
}


void UPopup_ReadQuest::InitPopup(const FResponseScenario& InScenarioData)
{
	PRINTLOG(TEXT("[ReadQuestWidget] InitPopup quest"));

	this->ScenarioData = InScenarioData;

	// WordWidget 초기화
	WordWidget->InitWordData(InScenarioData.full_data);
	Txt_Desc->SetText( FText::FromString(InScenarioData.GetDescription() ));
	
	// 선택지 리스트 빌드
	InitWord1List();
	InitWord2List();
}


void UPopup_ReadQuest::InitWord1List()
{
	if (!Scroll_Word1 || !EntryWidgetClass)
	{
		PRINTLOG(TEXT("[ReadQuestWidget] Cannot build Word1 list - missing components"));
		return;
	}

	// 기존 엔트리 제거
	Scroll_Word1->ClearChildren();
	Word1EntryList.Empty();

	// 중복 제거된 심볼 리스트 가져오기
	TArray<FString> List = ScenarioData.GetWord1List();

	auto PS = ULingoGameHelper::GetLingoPlayerState(GetWorld());
	
	// 플레이어 역할 확인 (OnlyQuestion2는 심볼 선택 불가)
	bool bCanSelectSymbol = (PS && PS->QuestRole != EReadQuestRole::OnlyQuestion2);

	// 엔트리 위젯 생성
	for (const FString& Symbol : List)
	{
		UPopup_ReadQuestItem* EntryWidget = CreateWidget<UPopup_ReadQuestItem>(GetWorld(), EntryWidgetClass);
		if (EntryWidget)
		{
			EntryWidget->InitializeEntry(Symbol, bCanSelectSymbol);
			EntryWidget->OnEntrySelected.AddUObject(this, &UPopup_ReadQuest::OnSelectWord1);

			Scroll_Word1->AddChild(EntryWidget);
			Word1EntryList.Add(EntryWidget);
		}
	}
}

void UPopup_ReadQuest::InitWord2List()
{
	if (!Scroll_Word2 || !EntryWidgetClass)
	{
		PRINTLOG(TEXT("[ReadQuestWidget] Cannot build Word2 list - missing components"));
		return;
	}

	// 기존 엔트리 제거
	Scroll_Word2->ClearChildren();
	Word2EntryList.Empty();

	// 중복 제거된 색상 리스트 가져오기
	TArray<FString> List = ScenarioData.GetWord2List();

	auto PS = ULingoGameHelper::GetLingoPlayerState(GetWorld());
	// 플레이어 역할 확인 (OnlyQuestion1은 색상 선택 불가)
	bool bCanSelectColor = (PS && PS->QuestRole != EReadQuestRole::OnlyQuestion1);

	// 엔트리 위젯 생성
	for (const FString& Color : List)
	{
		UPopup_ReadQuestItem* EntryWidget = CreateWidget<UPopup_ReadQuestItem>(GetWorld(), EntryWidgetClass);
		if (EntryWidget)
		{
			EntryWidget->InitializeEntry(Color, bCanSelectColor);
			EntryWidget->OnEntrySelected.AddUObject(this, &UPopup_ReadQuest::OnSelectWord2);

			Scroll_Word2->AddChild(EntryWidget);
			Word2EntryList.Add(EntryWidget);
		}
	}
}

void UPopup_ReadQuest::OnSelectWord1(const FString& Symbol, UPopup_ReadQuestItem* EntryWidget)
{
	if (!EntryWidget)
		return;

	// 이전 선택 해제
	if (SelectedSymbolEntry && SelectedSymbolEntry != EntryWidget)
		SelectedSymbolEntry->SetSelected(false);

	// 새로운 선택
	SelectedSymbolEntry = EntryWidget;
	SelectedSymbolEntry->SetSelected(true);

	// 서버에 전송
	auto PS = ULingoGameHelper::GetLingoPlayerState(GetWorld());
	PS->Server_SetSelectedWord1(Symbol);
}

void UPopup_ReadQuest::OnSelectWord2(const FString& Color, UPopup_ReadQuestItem* EntryWidget)
{
	if (!EntryWidget)
		return;

	// 이전 선택 해제
	if (SelectedColorEntry && SelectedColorEntry != EntryWidget)
	{
		SelectedColorEntry->SetSelected(false);
	}

	// 새로운 선택
	SelectedColorEntry = EntryWidget;
	SelectedColorEntry->SetSelected(true);

	// 서버에 전송
	auto PS = ULingoGameHelper::GetLingoPlayerState(GetWorld());
	PS->Server_SetSelectedWord2(Color);
}

void UPopup_ReadQuest::OnPlayerStateUpdated()
{
	auto PS = ULingoGameHelper::GetLingoPlayerState(GetWorld());
	// 심볼이 틀렸다면 엔트리 업데이트
	if (PS->bWrongWord1)
	{
		for (UPopup_ReadQuestItem* Entry : Word1EntryList)
		{
			if (Entry && Entry->GetChoiceValue() == PS->SelectedWord1)
			{
				Entry->SetWrong(true);
				Entry->SetSelected(false);
				SelectedSymbolEntry = nullptr;
				break;
			}
		}
	}

	// 색상이 틀렸다면 엔트리 업데이트
	if (PS->bWrongWord2)
	{
		for (UPopup_ReadQuestItem* Entry : Word2EntryList)
		{
			if (Entry && Entry->GetChoiceValue() == PS->SelectedWord2)
			{
				Entry->SetWrong(true);
				Entry->SetSelected(false);
				SelectedColorEntry = nullptr;
				break;
			}
		}
	}
}

void UPopup_ReadQuest::OnClickClose()
{
	RemoveFromParent();
}