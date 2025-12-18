// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "ULevelWordGroup.h"
#include "ULevelWordItem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Blueprint/WidgetTree.h"

void ULevelWordGroup::InitGroup(EWordType InWordType, const TArray<int32>& InWordCodes)
{
	WordType = InWordType;
	WordCodes = InWordCodes;

	UpdateGroupTitle();
	PopulateItems();
}

void ULevelWordGroup::SetColumnCount(int32 InColumnCount)
{
	if (InColumnCount > 0)
	{
		ColumnCount = InColumnCount;

		// 열 개수가 변경되면 아이템을 다시 배치
		if (WordCodes.Num() > 0)
		{
			PopulateItems();
		}
	}
}

void ULevelWordGroup::UpdateGroupTitle()
{
	if (!Text_GroupName)
	{
		return;
	}

	FString GroupName;
	switch (WordType)
	{
	case EWordType::Animal:
		GroupName = TEXT("Animal");
		break;
	case EWordType::Color:
		GroupName = TEXT("Color");
		break;
	case EWordType::Region:
		GroupName = TEXT("Region");
		break;
	case EWordType::Food:
		GroupName = TEXT("Food");
		break;
	default:
		GroupName = TEXT("Unknown");
		break;
	}

	Text_GroupName->SetText(FText::FromString(GroupName));
}

void ULevelWordGroup::PopulateItems()
{
	if (!UniformGridPanel || !LevelWordItemClass)
	{
		return;
	}

	// 기존 아이템들 제거
	UniformGridPanel->ClearChildren();

	// 새 아이템들 생성 및 배치
	for (int32 i = 0; i < WordCodes.Num(); ++i)
	{
		// ULevelWordItem 생성
		ULevelWordItem* WordItem = CreateWidget<ULevelWordItem>(this, LevelWordItemClass);
		if (!WordItem)
		{
			continue;
		}

		// 아이템 초기화
		WordItem->InitInfo(WordType, WordCodes[i]);

		// 그리드 위치 계산
		int32 Row = i / ColumnCount;
		int32 Column = i % ColumnCount;

		// UniformGridPanel에 추가
		UUniformGridSlot* GridSlot = UniformGridPanel->AddChildToUniformGrid(WordItem, Row, Column);
		if (GridSlot)
		{
			// 슬롯 설정 (필요시)
			GridSlot->SetHorizontalAlignment(HAlign_Fill);
			GridSlot->SetVerticalAlignment(VAlign_Fill);
		}
	}
}
