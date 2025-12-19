// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "ULevelSelectItem.h"
#include "ULevelWordGroup.h"
#include "EWordType.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "UImageButton.h"
#include "Components/Spacer.h"

void ULevelSelectItem::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Select)
	{
		Btn_Select->OnButtonClickedEvent.RemoveDynamic(this, &ULevelSelectItem::OnSelectButtonClicked);
		Btn_Select->OnButtonClickedEvent.AddDynamic(this, &ULevelSelectItem::OnSelectButtonClicked);
	}
}

void ULevelSelectItem::InitLevelItem(int32 InLevel, const FString& InLevelName, int32 InPlayTime)
{
	Level = InLevel;
	LevelName = InLevelName;
	PlayTime = InPlayTime;

	Txt_Title->SetText(FText::FromString(LevelName));
	Txt_PlayTime->SetText(FText::FromString(FString::Printf(TEXT("%d"), PlayTime)));

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

		USpacer* Spacer = NewObject<USpacer>(this);
		if (Spacer)
		{
			Spacer->SetSize(FVector2D(0.0f, 15.0f));
			VerticalBox->AddChildToVerticalBox(Spacer);
		}
	}
}

void ULevelSelectItem::CreateColorGroup()
{
	ULevelWordGroup* WordGroup = CreateWidget<ULevelWordGroup>(this, WordGroupClass);
	if (WordGroup)
	{
		WordGroup->InitGroup(EWordType::Color, Level);
		VerticalBox->AddChildToVerticalBox(WordGroup);

		USpacer* Spacer = NewObject<USpacer>(this);
		if (Spacer)
		{
			Spacer->SetSize(FVector2D(0.0f, 15.0f));
			VerticalBox->AddChildToVerticalBox(Spacer);
		}
	}
}

void ULevelSelectItem::CreateRegionGroup()
{
	ULevelWordGroup* WordGroup = CreateWidget<ULevelWordGroup>(this, WordGroupClass);
	if (WordGroup)
	{
		WordGroup->InitGroup(EWordType::Region, Level);
		VerticalBox->AddChildToVerticalBox(WordGroup);

		USpacer* Spacer = NewObject<USpacer>(this);
		if (Spacer)
		{
			Spacer->SetSize(FVector2D(0.0f, 15.0f));
			VerticalBox->AddChildToVerticalBox(Spacer);
		}
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
	if (OnLevelSelected.IsBound())
		OnLevelSelected.Execute(Level);
}
