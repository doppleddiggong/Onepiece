// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "UReadQuestEntryWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "GameLogging.h"

UReadQuestEntryWidget::UReadQuestEntryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UReadQuestEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 버튼 클릭 이벤트 바인딩
	if (SelectButton)
	{
		SelectButton->OnClicked.AddDynamic(this, &UReadQuestEntryWidget::OnButtonClicked);
	}
}

void UReadQuestEntryWidget::InitializeEntry(const FString& Value, bool bEnabled)
{
	ChoiceValue = Value;

	// 텍스트 설정
	if (ChoiceText)
	{
		ChoiceText->SetText(FText::FromString(Value));
	}

	// 버튼 활성화 상태 설정
	if (SelectButton)
	{
		SelectButton->SetIsEnabled(bEnabled);
	}

	// 초기 상태로 설정
	bIsSelected = false;
	bIsWrong = false;

	UpdateVisualState();

	PRINTLOG(TEXT("[ReadQuestEntry] Initialized with value: %s, enabled: %s"), *Value, bEnabled ? TEXT("true") : TEXT("false"));
}

void UReadQuestEntryWidget::SetSelected(bool bSelected)
{
	if (bIsSelected == bSelected)
		return;

	bIsSelected = bSelected;
	UpdateVisualState();

	PRINTLOG(TEXT("[ReadQuestEntry] Selection changed: %s, value: %s"), bSelected ? TEXT("Selected") : TEXT("Deselected"), *ChoiceValue);
}

void UReadQuestEntryWidget::SetWrong(bool bWrong)
{
	if (bIsWrong == bWrong)
		return;

	bIsWrong = bWrong;
	UpdateVisualState();

	PRINTLOG(TEXT("[ReadQuestEntry] Wrong state changed: %s, value: %s"), bWrong ? TEXT("Wrong") : TEXT("Correct"), *ChoiceValue);
}

void UReadQuestEntryWidget::OnButtonClicked()
{
	// 오답 상태이거나 이미 선택된 상태면 무시
	if (bIsWrong)
	{
		PRINTLOG(TEXT("[ReadQuestEntry] Button clicked but entry is marked as wrong: %s"), *ChoiceValue);
		return;
	}

	// 델리게이트 브로드캐스트
	OnEntrySelected.Broadcast(ChoiceValue, this);

	PRINTLOG(TEXT("[ReadQuestEntry] Button clicked: %s"), *ChoiceValue);
}

void UReadQuestEntryWidget::UpdateVisualState()
{
	if (!StateImage)
		return;

	// 상태에 따라 이미지 표시
	if (bIsWrong)
	{
		// 오답: 빨간색 X 표시 등
		StateImage->SetVisibility(ESlateVisibility::Visible);
		StateImage->SetColorAndOpacity(FLinearColor::Red);
	}
	else if (bIsSelected)
	{
		// 선택됨: 초록색 체크 표시 등
		StateImage->SetVisibility(ESlateVisibility::Visible);
		StateImage->SetColorAndOpacity(FLinearColor::Green);
	}
	else
	{
		// 기본 상태: 이미지 숨김
		StateImage->SetVisibility(ESlateVisibility::Hidden);
	}

	// 버튼 스타일 변경 (선택적)
	if (SelectButton)
	{
		// 오답이면 버튼 비활성화
		if (bIsWrong)
		{
			SelectButton->SetIsEnabled(false);
		}
	}
}
