// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "UPopup_ReadQuestItem.h"
#include "Components/CheckBox.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "GameLogging.h"

UPopup_ReadQuestItem::UPopup_ReadQuestItem(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UPopup_ReadQuestItem::NativeConstruct()
{
	Super::NativeConstruct();

	// 중복 바인딩 방지
	if (SelectCheckBox)
	{
		SelectCheckBox->OnCheckStateChanged.RemoveDynamic(this, &UPopup_ReadQuestItem::OnCheckStateChanged);
		SelectCheckBox->OnCheckStateChanged.AddDynamic(this, &UPopup_ReadQuestItem::OnCheckStateChanged);
	}
}

void UPopup_ReadQuestItem::InitializeEntry(const FString& Value, bool bEnabled)
{
	ChoiceValue = Value;

	// 텍스트 설정
	if (ChoiceText)
	{
		ChoiceText->SetText(FText::FromString(Value));
	}

	// 체크박스 활성화 상태 설정
	if (SelectCheckBox)
	{
		SelectCheckBox->SetIsEnabled(bEnabled);
		// 초기에는 체크 해제 상태로 설정
		SelectCheckBox->SetCheckedState(ECheckBoxState::Unchecked);
	}

	// 초기 상태로 설정
	bIsSelected = false;
	bIsWrong = false;

	UpdateVisualState();

	PRINTLOG(TEXT("[ReadQuestEntry] Initialized with value: %s, enabled: %s"), *Value, bEnabled ? TEXT("true") : TEXT("false"));
}

void UPopup_ReadQuestItem::SetSelected(bool bSelected)
{
	if (bIsSelected == bSelected)
		return;

	bIsSelected = bSelected;

	// 체크박스 상태 동기화
	if (SelectCheckBox)
		SelectCheckBox->SetCheckedState(bSelected ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);

	UpdateVisualState();

	PRINTLOG(TEXT("[ReadQuestEntry] Selection changed: %s, value: %s"), bSelected ? TEXT("Selected") : TEXT("Deselected"), *ChoiceValue);
}

void UPopup_ReadQuestItem::SetWrong(bool bWrong)
{
	if (bIsWrong == bWrong)
		return;

	bIsWrong = bWrong;
	UpdateVisualState();

	PRINTLOG(TEXT("[ReadQuestEntry] Wrong state changed: %s, value: %s"), bWrong ? TEXT("Wrong") : TEXT("Correct"), *ChoiceValue);
}

void UPopup_ReadQuestItem::OnCheckStateChanged(bool bIsChecked)
{
	// 오답 상태면 체크 해제
	if (bIsWrong)
	{
		if (SelectCheckBox)
		{
			SelectCheckBox->SetCheckedState(ECheckBoxState::Unchecked);
		}
		PRINTLOG(TEXT("[ReadQuestEntry] CheckBox state changed but entry is marked as wrong: %s"), *ChoiceValue);
		return;
	}

	// 체크된 경우에만 델리게이트 브로드캐스트
	if (bIsChecked)
	{
		OnEntrySelected.Broadcast(ChoiceValue, this);
		PRINTLOG(TEXT("[ReadQuestEntry] CheckBox checked: %s"), *ChoiceValue);
	}
	else
	{
		PRINTLOG(TEXT("[ReadQuestEntry] CheckBox unchecked: %s"), *ChoiceValue);
	}
}

void UPopup_ReadQuestItem::UpdateVisualState()
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

	// 체크박스 스타일 변경 (선택적)
	if (SelectCheckBox)
	{
		// 오답이면 체크박스 비활성화
		if (bIsWrong)
		{
			SelectCheckBox->SetIsEnabled(false);
			SelectCheckBox->SetCheckedState(ECheckBoxState::Unchecked);
		}
	}
}
