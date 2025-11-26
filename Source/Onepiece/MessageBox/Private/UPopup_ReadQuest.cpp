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
#include "Kismet/GameplayStatics.h"

UPopup_ReadQuest::UPopup_ReadQuest(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UPopup_ReadQuest::NativeConstruct()
{
	Super::NativeConstruct();

	PRINTLOG(TEXT("[ReadQuestWidget] Widget constructed"));
}

void UPopup_ReadQuest::InitializeQuest()
{
	PRINTLOG(TEXT("[ReadQuestWidget] Initializing quest"));

	// 미션 설명 텍스트 설정
	if ( CachedGameState->CurScenarioData.word_data1.Eng.Len() > 0)
	{
		FString Description = ULingoGameHelper::GetLingoGameState(GetWorld())->CurScenarioData.GetDescription();
		MissionDescriptionText->SetText(FText::FromString(Description));
	}

	// 선택지 리스트 빌드
	BuildSymbolList();
	BuildColorList();

	PRINTLOG(TEXT("[ReadQuestWidget] Quest initialized successfully"));
}

TArray<FString> UPopup_ReadQuest::GetUniqueSymbols() const
{
	TSet<FString> UniqueSet;
	TArray<FString> Result;

	if (!CachedGameState)
		return Result;

	// target_data에서 symbol 필드만 추출하여 중복 제거
	for (const FScenarioTargetData& TargetData : CachedGameState->CurScenarioData.target_data)
	{
		if (!TargetData.word1.code.IsEmpty())
		{
			UniqueSet.Add(TargetData.word1.code);
		}
	}

	Result = UniqueSet.Array();
	return Result;
}

TArray<FString> UPopup_ReadQuest::GetUniqueColors() const
{
	TSet<FString> UniqueSet;
	TArray<FString> Result;

	if (!CachedGameState)
		return Result;

	// target_data에서 color 필드만 추출하여 중복 제거
	for (const FScenarioTargetData& TargetData : CachedGameState->CurScenarioData.target_data)
	{
		if (!TargetData.word2.code.IsEmpty())
		{
			UniqueSet.Add(TargetData.word2.code);
		}
	}

	Result = UniqueSet.Array();
	return Result;
}

void UPopup_ReadQuest::BuildSymbolList()
{
	if (!SymbolScrollBox || !EntryWidgetClass)
	{
		PRINTLOG(TEXT("[ReadQuestWidget] Cannot build symbol list - missing components"));
		return;
	}

	// 기존 엔트리 제거
	SymbolScrollBox->ClearChildren();
	SymbolEntries.Empty();

	// 중복 제거된 심볼 리스트 가져오기
	TArray<FString> Symbols = GetUniqueSymbols();

	PRINTLOG(TEXT("[ReadQuestWidget] Building symbol list with %d items"), Symbols.Num());

	// 플레이어 역할 확인 (OnlyQuestion2는 심볼 선택 불가)
	bool bCanSelectSymbol = (CachedPlayerState && CachedPlayerState->QuestRole != EReadQuestRole::OnlyQuestion2);

	// 엔트리 위젯 생성
	for (const FString& Symbol : Symbols)
	{
		UPopup_ReadQuestItem* EntryWidget = CreateWidget<UPopup_ReadQuestItem>(GetWorld(), EntryWidgetClass);
		if (EntryWidget)
		{
			EntryWidget->InitializeEntry(Symbol, bCanSelectSymbol);
			EntryWidget->OnEntrySelected.AddUObject(this, &UPopup_ReadQuest::OnSymbolSelected);

			SymbolScrollBox->AddChild(EntryWidget);
			SymbolEntries.Add(EntryWidget);
		}
	}

	PRINTLOG(TEXT("[ReadQuestWidget] Symbol list built successfully"));
}

void UPopup_ReadQuest::BuildColorList()
{
	if (!ColorScrollBox || !EntryWidgetClass)
	{
		PRINTLOG(TEXT("[ReadQuestWidget] Cannot build color list - missing components"));
		return;
	}

	// 기존 엔트리 제거
	ColorScrollBox->ClearChildren();
	ColorEntries.Empty();

	// 중복 제거된 색상 리스트 가져오기
	TArray<FString> Colors = GetUniqueColors();

	PRINTLOG(TEXT("[ReadQuestWidget] Building color list with %d items"), Colors.Num());

	// 플레이어 역할 확인 (OnlyQuestion1은 색상 선택 불가)
	bool bCanSelectColor = (CachedPlayerState && CachedPlayerState->QuestRole != EReadQuestRole::OnlyQuestion1);

	// 엔트리 위젯 생성
	for (const FString& Color : Colors)
	{
		UPopup_ReadQuestItem* EntryWidget = CreateWidget<UPopup_ReadQuestItem>(GetWorld(), EntryWidgetClass);
		if (EntryWidget)
		{
			EntryWidget->InitializeEntry(Color, bCanSelectColor);
			EntryWidget->OnEntrySelected.AddUObject(this, &UPopup_ReadQuest::OnColorSelected);

			ColorScrollBox->AddChild(EntryWidget);
			ColorEntries.Add(EntryWidget);
		}
	}

	PRINTLOG(TEXT("[ReadQuestWidget] Color list built successfully"));
}

void UPopup_ReadQuest::OnSymbolSelected(const FString& Symbol, UPopup_ReadQuestItem* EntryWidget)
{
	if (!CachedPlayerState || !EntryWidget)
		return;

	PRINTLOG(TEXT("[ReadQuestWidget] Symbol selected: %s"), *Symbol);

	// 이전 선택 해제
	if (SelectedSymbolEntry && SelectedSymbolEntry != EntryWidget)
	{
		SelectedSymbolEntry->SetSelected(false);
	}

	// 새로운 선택
	SelectedSymbolEntry = EntryWidget;
	SelectedSymbolEntry->SetSelected(true);

	// 서버에 전송
	CachedPlayerState->ServerSetSelectedSymbol(Symbol);
}

void UPopup_ReadQuest::OnColorSelected(const FString& Color, UPopup_ReadQuestItem* EntryWidget)
{
	if (!CachedPlayerState || !EntryWidget)
		return;

	PRINTLOG(TEXT("[ReadQuestWidget] Color selected: %s"), *Color);

	// 이전 선택 해제
	if (SelectedColorEntry && SelectedColorEntry != EntryWidget)
	{
		SelectedColorEntry->SetSelected(false);
	}

	// 새로운 선택
	SelectedColorEntry = EntryWidget;
	SelectedColorEntry->SetSelected(true);

	// 서버에 전송
	CachedPlayerState->ServerSetSelectedColor(Color);
}

void UPopup_ReadQuest::OnPlayerStateUpdated()
{
	if (!CachedPlayerState)
		return;

	PRINTLOG(TEXT("[ReadQuestWidget] PlayerState updated - checking for wrong answers"));

	// 심볼이 틀렸다면 엔트리 업데이트
	if (CachedPlayerState->bSymbolWrong)
	{
		for (UPopup_ReadQuestItem* Entry : SymbolEntries)
		{
			if (Entry && Entry->GetChoiceValue() == CachedPlayerState->SelectedSymbol)
			{
				Entry->SetWrong(true);
				Entry->SetSelected(false);
				SelectedSymbolEntry = nullptr;
				break;
			}
		}
	}

	// 색상이 틀렸다면 엔트리 업데이트
	if (CachedPlayerState->bColorWrong)
	{
		for (UPopup_ReadQuestItem* Entry : ColorEntries)
		{
			if (Entry && Entry->GetChoiceValue() == CachedPlayerState->SelectedColor)
			{
				Entry->SetWrong(true);
				Entry->SetSelected(false);
				SelectedColorEntry = nullptr;
				break;
			}
		}
	}
}
