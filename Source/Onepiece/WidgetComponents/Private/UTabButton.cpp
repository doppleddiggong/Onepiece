// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "UTabButton.h"
#include "UTabButtonGroup.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UTabButton::NativeConstruct()
{
	Super::NativeConstruct();

	if (Button_Tab)
	{
		Button_Tab->OnClicked.RemoveDynamic(this, &UTabButton::OnClicked);
		Button_Tab->OnClicked.AddDynamic(this, &UTabButton::OnClicked);
	}
}

void UTabButton::InitData(const int32 InTabIndex, UTabButtonGroup* InOwnerGroup)
{
	TabIndex = InTabIndex;
	OwnerTabGroup = InOwnerGroup;
}

void UTabButton::SetSelected(const bool bIsSelected) const
{
	if (!OwnerTabGroup.IsValid())
		return;

	// Image_ActivateState의 Visibility 및 색상 설정
	if (Image_ActivateState)
	{
		Image_ActivateState->SetVisibility(bIsSelected ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
		Image_ActivateState->SetColorAndOpacity(OwnerTabGroup->GetActivateColor(bIsSelected));
	}

	// Text 색상 변경
	if (Txt_ButtonLabel)
		Txt_ButtonLabel->SetColorAndOpacity(OwnerTabGroup->GetTextColor(bIsSelected));
}

void UTabButton::SetLabel(const FText& InText) const
{
	Txt_ButtonLabel->SetText(InText);
}

void UTabButton::OnClicked()
{
	OnTabButtonClicked.Broadcast(TabIndex);
}
