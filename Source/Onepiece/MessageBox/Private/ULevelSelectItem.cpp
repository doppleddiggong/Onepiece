// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "ULevelSelectItem.h"
#include "ULevelWordGroup.h"
#include "EWordType.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "UImageButton.h"

void ULevelSelectItem::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Select)
	{
		Btn_Select->OnButtonClickedEvent.RemoveDynamic(this, &ULevelSelectItem::OnSelectButtonClicked);
		Btn_Select->OnButtonClickedEvent.AddDynamic(this, &ULevelSelectItem::OnSelectButtonClicked);
	}
}

void ULevelSelectItem::InitLevelItem(int32 InLevel, const FString& InLevelName)
{
	Level = InLevel;
	LevelName = InLevelName;

	Txt_Title->SetText(FText::FromString(LevelName));

	if (VerticalBox)
		VerticalBox->ClearChildren();

	// 새 WordGroup들 생성
	if ( WordGroupClass )
	{
		CreateAnimalGroup();
		CreateColorGroup();
		CreateRegionGroup();
		CreateFoodGroup();
	}
}

void ULevelSelectItem::CreateAnimalGroup()
{
	ULevelWordGroup* WordGroup = CreateWidget<ULevelWordGroup>(this, WordGroupClass);
	if (WordGroup)
	{
		WordGroup->InitGroup(EWordType::Animal, Level);
		VerticalBox->AddChildToVerticalBox(WordGroup);
	}
}

void ULevelSelectItem::CreateColorGroup()
{
	ULevelWordGroup* WordGroup = CreateWidget<ULevelWordGroup>(this, WordGroupClass);
	if (WordGroup)
	{
		WordGroup->InitGroup(EWordType::Color, Level);
		VerticalBox->AddChildToVerticalBox(WordGroup);
	}
}

void ULevelSelectItem::CreateRegionGroup()
{
	ULevelWordGroup* WordGroup = CreateWidget<ULevelWordGroup>(this, WordGroupClass);
	if (WordGroup)
	{
		WordGroup->InitGroup(EWordType::Region, Level);
		VerticalBox->AddChildToVerticalBox(WordGroup);
	}
}

void ULevelSelectItem::CreateFoodGroup()
{
	ULevelWordGroup* WordGroup = CreateWidget<ULevelWordGroup>(this, WordGroupClass);
	if (WordGroup)
	{
		WordGroup->InitGroup(EWordType::Food, Level);
		VerticalBox->AddChildToVerticalBox(WordGroup);
	}
}


void ULevelSelectItem::OnSelectButtonClicked()
{
	// 델리게이트 실행: 선택된 레벨 정보를 상위 클래스에 전달
	if (OnLevelSelected.IsBound())
	{
		OnLevelSelected.Execute(Level, LevelName);
	}
}
