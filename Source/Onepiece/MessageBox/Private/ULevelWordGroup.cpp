// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "ULevelWordGroup.h"
#include "ULevelWordItem.h"
#include "UGameDataManager.h"
#include "FReadData.h"
#include "FListenData.h"
#include "FColorData.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Blueprint/WidgetTree.h"

void ULevelWordGroup::InitGroup(EWordType InWordType, int32 InCurrentLevel)
{
	WordType = InWordType;
	CurrentLevel = InCurrentLevel;

	UpdateGroupTitle();
	PopulateItems();
}

void ULevelWordGroup::SetColumnCount(int32 InColumnCount)
{
	if (InColumnCount > 0)
	{
		ColumnCount = InColumnCount;

		// 열 개수가 변경되면 아이템을 다시 배치
		PopulateItems();
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
	case EWordType::Animal:		GroupName = TEXT("Animal");		break;
	case EWordType::Color:		GroupName = TEXT("Color");		break;
	case EWordType::Region:		GroupName = TEXT("Region");		break;
	case EWordType::Food:		GroupName = TEXT("Food");		break;
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

	UGameDataManager* GameDataManager = UGameDataManager::Get(GetWorld());
	if (!GameDataManager)
	{
		return;
	}

	// 기존 아이템들 제거
	UniformGridPanel->ClearChildren();

	// 레벨별 단어 데이터를 수집하는 구조체
	struct FWordItemData
	{
		int32 WordCode;
		int32 Level;
		bool bIsCurrentLevel;
	};

	TArray<FWordItemData> AllWordData;

	// WordType에 따라 현재 레벨 이하의 모든 데이터 수집
	if (WordType == EWordType::Animal)
	{
		// FReadData에서 CurrentLevel 이하의 모든 데이터 가져오기
		for (int32 Level = 1; Level <= CurrentLevel; ++Level)
		{
			TArray<FReadData> ReadDataArray = GameDataManager->GetReadDataByLevel(Level);
			for (const FReadData& ReadData : ReadDataArray)
			{
				FWordItemData ItemData;
				ItemData.WordCode = ReadData.Index;
				ItemData.Level = Level;
				ItemData.bIsCurrentLevel = (Level == CurrentLevel);
				AllWordData.Add(ItemData);
			}
		}
	}
	else if (WordType == EWordType::Color)
	{
		// FColorData에서 CurrentLevel 이하의 모든 데이터 가져오기
		for (int32 Level = 1; Level <= CurrentLevel; ++Level)
		{
			TArray<FColorData> ColorDataArray = GameDataManager->GetColorDataByLevel(Level);
			for (const FColorData& ColorData : ColorDataArray)
			{
				FWordItemData ItemData;
				ItemData.WordCode = ColorData.Index;
				ItemData.Level = Level;
				ItemData.bIsCurrentLevel = (Level == CurrentLevel);
				AllWordData.Add(ItemData);
			}
		}
	}
	else if (WordType == EWordType::Region)
	{
		// FListenData (Category="Region")에서 CurrentLevel 이하의 모든 데이터 가져오기
		TArray<FListenData> ListenDataArray = GameDataManager->GetListenDataByCategory(TEXT("Region"));
		for (const FListenData& ListenData : ListenDataArray)
		{
			if (ListenData.Level >= 1 && ListenData.Level <= CurrentLevel)
			{
				FWordItemData ItemData;
				ItemData.WordCode = ListenData.Index;
				ItemData.Level = ListenData.Level;
				ItemData.bIsCurrentLevel = (ListenData.Level == CurrentLevel);
				AllWordData.Add(ItemData);
			}
		}
	}
	else if (WordType == EWordType::Food)
	{
		// FListenData (Category="Food")에서 CurrentLevel 이하의 모든 데이터 가져오기
		TArray<FListenData> ListenDataArray = GameDataManager->GetListenDataByCategory(TEXT("Food"));
		for (const FListenData& ListenData : ListenDataArray)
		{
			if (ListenData.Level >= 1 && ListenData.Level <= CurrentLevel)
			{
				FWordItemData ItemData;
				ItemData.WordCode = ListenData.Index;
				ItemData.Level = ListenData.Level;
				ItemData.bIsCurrentLevel = (ListenData.Level == CurrentLevel);
				AllWordData.Add(ItemData);
			}
		}
	}

	// Level 내림차순으로 정렬 (3 -> 2 -> 1)
	AllWordData.Sort([](const FWordItemData& A, const FWordItemData& B)
	{
		return A.Level > B.Level;
	});

	// 아이템 생성 및 배치
	for (int32 i = 0; i < AllWordData.Num(); ++i)
	{
		const FWordItemData& ItemData = AllWordData[i];

		// ULevelWordItem 생성
		ULevelWordItem* WordItem = CreateWidget<ULevelWordItem>(this, LevelWordItemClass);
		if (!WordItem)
		{
			continue;
		}

		// 아이템 초기화
		WordItem->InitInfo(WordType, ItemData.WordCode);

		// 색상 설정 (현재 레벨: 녹색, 하위 레벨: 회색)
		WordItem->SetItemColor(ItemData.bIsCurrentLevel);

		// 그리드 위치 계산
		int32 Row = i / ColumnCount;
		int32 Column = i % ColumnCount;

		// UniformGridPanel에 추가
		UUniformGridSlot* GridSlot = UniformGridPanel->AddChildToUniformGrid(WordItem, Row, Column);
		if (GridSlot)
		{
			GridSlot->SetHorizontalAlignment(HAlign_Fill);
			GridSlot->SetVerticalAlignment(VAlign_Fill);
		}
	}
}
