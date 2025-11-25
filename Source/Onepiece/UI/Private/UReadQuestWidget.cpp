// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "UReadQuestWidget.h"
#include "UReadQuestEntryWidget.h"
#include "ALingoGameState.h"
#include "ALingoPlayerState.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerController.h"
#include "GameLogging.h"
#include "Kismet/GameplayStatics.h"

UReadQuestWidget::UReadQuestWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UReadQuestWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 참조 캐싱
	CacheReferences();

	PRINTLOG(TEXT("[ReadQuestWidget] Widget constructed"));
}

void UReadQuestWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// 타이머 업데이트
	UpdateTimer();
}

void UReadQuestWidget::CacheReferences()
{
	// GameState 가져오기
	if (!CachedGameState)
	{
		CachedGameState = Cast<ALingoGameState>(UGameplayStatics::GetGameState(GetWorld()));
		if (!CachedGameState)
		{
			PRINTLOG(TEXT("[ReadQuestWidget] Failed to get GameState"));
		}
	}

	// PlayerState 가져오기
	if (!CachedPlayerState)
	{
		APlayerController* PC = GetOwningPlayer();
		if (PC)
		{
			CachedPlayerState = PC->GetPlayerState<ALingoPlayerState>();
			if (!CachedPlayerState)
			{
				PRINTLOG(TEXT("[ReadQuestWidget] Failed to get PlayerState"));
			}
		}
	}
}

void UReadQuestWidget::InitializeQuest()
{
	PRINTLOG(TEXT("[ReadQuestWidget] Initializing quest"));

	// 참조 확인
	CacheReferences();

	if (!CachedGameState || !CachedPlayerState)
	{
		PRINTLOG(TEXT("[ReadQuestWidget] Cannot initialize - missing references"));
		return;
	}

	// 미션 설명 텍스트 설정
	if (MissionDescriptionText && CachedGameState->CurScenarioData.word_data1.Eng.Len() > 0)
	{
		FString Description = FString::Printf(TEXT("Find: %s %s"),
			*CachedGameState->CurScenarioData.word_data1.Eng,
			*CachedGameState->CurScenarioData.word_data2.Eng);
		MissionDescriptionText->SetText(FText::FromString(Description));
	}

	// 선택지 리스트 빌드
	BuildSymbolList();
	BuildColorList();

	PRINTLOG(TEXT("[ReadQuestWidget] Quest initialized successfully"));
}

TArray<FString> UReadQuestWidget::GetUniqueSymbols() const
{
	TSet<FString> UniqueSet;
	TArray<FString> Result;

	if (!CachedGameState)
		return Result;

	// target_data에서 symbol 필드만 추출하여 중복 제거
	for (const FScenarioTargetData& TargetData : CachedGameState->CurScenarioData.target_data)
	{
		if (!TargetData.symbol.IsEmpty())
		{
			UniqueSet.Add(TargetData.symbol);
		}
	}

	Result = UniqueSet.Array();
	return Result;
}

TArray<FString> UReadQuestWidget::GetUniqueColors() const
{
	TSet<FString> UniqueSet;
	TArray<FString> Result;

	if (!CachedGameState)
		return Result;

	// target_data에서 color 필드만 추출하여 중복 제거
	for (const FScenarioTargetData& TargetData : CachedGameState->CurScenarioData.target_data)
	{
		if (!TargetData.color.IsEmpty())
		{
			UniqueSet.Add(TargetData.color);
		}
	}

	Result = UniqueSet.Array();
	return Result;
}

void UReadQuestWidget::BuildSymbolList()
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
		UReadQuestEntryWidget* EntryWidget = CreateWidget<UReadQuestEntryWidget>(GetWorld(), EntryWidgetClass);
		if (EntryWidget)
		{
			EntryWidget->InitializeEntry(Symbol, bCanSelectSymbol);
			EntryWidget->OnEntrySelected.AddUObject(this, &UReadQuestWidget::OnSymbolSelected);

			SymbolScrollBox->AddChild(EntryWidget);
			SymbolEntries.Add(EntryWidget);
		}
	}

	PRINTLOG(TEXT("[ReadQuestWidget] Symbol list built successfully"));
}

void UReadQuestWidget::BuildColorList()
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
		UReadQuestEntryWidget* EntryWidget = CreateWidget<UReadQuestEntryWidget>(GetWorld(), EntryWidgetClass);
		if (EntryWidget)
		{
			EntryWidget->InitializeEntry(Color, bCanSelectColor);
			EntryWidget->OnEntrySelected.AddUObject(this, &UReadQuestWidget::OnColorSelected);

			ColorScrollBox->AddChild(EntryWidget);
			ColorEntries.Add(EntryWidget);
		}
	}

	PRINTLOG(TEXT("[ReadQuestWidget] Color list built successfully"));
}

void UReadQuestWidget::OnSymbolSelected(const FString& Symbol, UReadQuestEntryWidget* EntryWidget)
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

void UReadQuestWidget::OnColorSelected(const FString& Color, UReadQuestEntryWidget* EntryWidget)
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

void UReadQuestWidget::UpdateTimer()
{
	if (!TimerText || !CachedGameState)
		return;

	// 남은 시간 표시
	float RemainTime = CachedGameState->GetRemainMissionTime();
	int32 Minutes = FMath::FloorToInt(RemainTime / 60.f);
	int32 Seconds = FMath::FloorToInt(RemainTime) % 60;

	FString TimeString = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
	TimerText->SetText(FText::FromString(TimeString));

	// 시간이 30초 이하면 빨간색으로 표시
	if (RemainTime <= 30.f)
	{
		TimerText->SetColorAndOpacity(FLinearColor::Red);
	}
	else
	{
		TimerText->SetColorAndOpacity(FLinearColor::White);
	}
}

void UReadQuestWidget::OnPlayerStateUpdated()
{
	if (!CachedPlayerState)
		return;

	PRINTLOG(TEXT("[ReadQuestWidget] PlayerState updated - checking for wrong answers"));

	// 심볼이 틀렸다면 엔트리 업데이트
	if (CachedPlayerState->bSymbolWrong)
	{
		for (UReadQuestEntryWidget* Entry : SymbolEntries)
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
		for (UReadQuestEntryWidget* Entry : ColorEntries)
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
