// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "ULevelSelectItem.h"
#include "ULevelWordGroup.h"
#include "UGameDataManager.h"
#include "FReadData.h"
#include "FListenData.h"
#include "FColorData.h"
#include "EWordType.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/Button.h"

void ULevelSelectItem::InitLevelItem(int32 InLevel, const FString& InLevelName)
{
	Level = InLevel;
	LevelName = InLevelName;

	UpdateLevelTitle();

	// 기존 WordGroup들 제거
	WordGroups.Empty();
	if (VerticalBox)
	{
		VerticalBox->ClearChildren();
	}

	// 새 WordGroup들 생성
	CreateAnimalGroup();
	CreateColorGroup();
	CreateRegionGroup();
	CreateFoodGroup();

	// VerticalBox에 추가
	PopulateWordGroups();
}

void ULevelSelectItem::UpdateLevelTitle()
{
	if (!Txt_Title)
	{
		return;
	}

	Txt_Title->SetText(FText::FromString(LevelName));
}

void ULevelSelectItem::CreateAnimalGroup()
{
	if (!WordGroupClass)
	{
		return;
	}

	UGameDataManager* GameDataManager = UGameDataManager::Get(GetWorld());
	if (!GameDataManager)
	{
		return;
	}

	// Level에 맞는 FReadData 가져오기
	TArray<FReadData> ReadDataArray = GameDataManager->GetReadDataByLevel(Level);

	TArray<int32> AnimalCodes;
	for (const FReadData& ReadData : ReadDataArray)
	{
		AnimalCodes.Add(ReadData.Index);
	}

	if (AnimalCodes.Num() > 0)
	{
		ULevelWordGroup* WordGroup = CreateWidget<ULevelWordGroup>(this, WordGroupClass);
		if (WordGroup)
		{
			WordGroup->InitGroup(EWordType::Animal, AnimalCodes);
			WordGroups.Add(WordGroup);
		}
	}
}

void ULevelSelectItem::CreateColorGroup()
{
	if (!WordGroupClass)
	{
		return;
	}

	UGameDataManager* GameDataManager = UGameDataManager::Get(GetWorld());
	if (!GameDataManager)
	{
		return;
	}

	// Level에 맞는 FColorData 가져오기
	TArray<FColorData> ColorDataArray = GameDataManager->GetColorDataByLevel(Level);

	TArray<int32> ColorCodes;
	for (const FColorData& ColorData : ColorDataArray)
	{
		ColorCodes.Add(ColorData.Index);
	}

	if (ColorCodes.Num() > 0)
	{
		ULevelWordGroup* WordGroup = CreateWidget<ULevelWordGroup>(this, WordGroupClass);
		if (WordGroup)
		{
			WordGroup->InitGroup(EWordType::Color, ColorCodes);
			WordGroups.Add(WordGroup);
		}
	}
}

void ULevelSelectItem::CreateRegionGroup()
{
	if (!WordGroupClass)
	{
		return;
	}

	UGameDataManager* GameDataManager = UGameDataManager::Get(GetWorld());
	if (!GameDataManager)
	{
		return;
	}

	// Category="Region"이고 Level에 맞는 FListenData 가져오기
	TArray<FListenData> ListenDataArray = GameDataManager->GetListenDataByCategory(TEXT("Region"));

	TArray<int32> RegionCodes;
	for (const FListenData& ListenData : ListenDataArray)
	{
		if (ListenData.Level == Level)
		{
			RegionCodes.Add(ListenData.Index);
		}
	}

	if (RegionCodes.Num() > 0)
	{
		ULevelWordGroup* WordGroup = CreateWidget<ULevelWordGroup>(this, WordGroupClass);
		if (WordGroup)
		{
			WordGroup->InitGroup(EWordType::Region, RegionCodes);
			WordGroups.Add(WordGroup);
		}
	}
}

void ULevelSelectItem::CreateFoodGroup()
{
	if (!WordGroupClass)
	{
		return;
	}

	UGameDataManager* GameDataManager = UGameDataManager::Get(GetWorld());
	if (!GameDataManager)
	{
		return;
	}

	// Category="Food"이고 Level에 맞는 FListenData 가져오기
	TArray<FListenData> ListenDataArray = GameDataManager->GetListenDataByCategory(TEXT("Food"));

	TArray<int32> FoodCodes;
	for (const FListenData& ListenData : ListenDataArray)
	{
		if (ListenData.Level == Level)
		{
			FoodCodes.Add(ListenData.Index);
		}
	}

	if (FoodCodes.Num() > 0)
	{
		ULevelWordGroup* WordGroup = CreateWidget<ULevelWordGroup>(this, WordGroupClass);
		if (WordGroup)
		{
			WordGroup->InitGroup(EWordType::Food, FoodCodes);
			WordGroups.Add(WordGroup);
		}
	}
}

void ULevelSelectItem::PopulateWordGroups()
{
	if (!VerticalBox)
	{
		return;
	}

	for (ULevelWordGroup* WordGroup : WordGroups)
	{
		if (WordGroup)
		{
			VerticalBox->AddChildToVerticalBox(WordGroup);
		}
	}
}
