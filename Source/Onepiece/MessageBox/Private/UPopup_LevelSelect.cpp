// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "UPopup_LevelSelect.h"
#include "ULevelSelectItem.h"
#include "Components/HorizontalBox.h"
#include "Components/Button.h"

void UPopup_LevelSelect::NativeConstruct()
{
	Super::NativeConstruct();
}

void UPopup_LevelSelect::InitPopup()
{
	// 기존 아이템들 제거
	LevelSelectItems.Empty();
	if (HorizontalBox)
	{
		HorizontalBox->ClearChildren();
	}

	// 레벨 아이템들 생성
	ShowLevels();
}

void UPopup_LevelSelect::ShowLevels()
{
	if (!HorizontalBox || !LevelSelectItemClass)
	{
		return;
	}

	// 기존 아이템들 제거
	HorizontalBox->ClearChildren();
	LevelSelectItems.Empty();

	// 레벨 아이템들 생성 및 추가
	for (int32 i = 0; i < 3; ++i)
	{
		ULevelSelectItem* LevelItem = CreateWidget<ULevelSelectItem>(this, LevelSelectItemClass);
		if (LevelItem)
		{
			// 레벨은 1부터 시작 (i+1)
			int32 Level = i + 1;
			FString LevelName = (i < LevelNames.Num()) ? LevelNames[i] : FString::Printf(TEXT("Level %d"), Level);

			LevelItem->InitLevelItem(Level, LevelName);

			HorizontalBox->AddChildToHorizontalBox(LevelItem);
			LevelSelectItems.Add(LevelItem);
		}
	}
}
