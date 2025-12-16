// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "UTabButton.h"
#include "UTabButtonGroup.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UTabButton::NativeConstruct()
{
	Super::NativeConstruct();

	if (Button_Tab)
	{
		Button_Tab->OnClicked.RemoveDynamic(this, &UTabButton::OnClicked);
		Button_Tab->OnClicked.AddDynamic(this, &UTabButton::OnClicked);
	}
}

void UTabButton::InitData(int32 InTabIndex, UTabButtonGroup* InOwnerGroup)
{
	tabIndex = InTabIndex;
	ownerButtonGroup = InOwnerGroup;
}

void UTabButton::SetSelected(bool bIsSelected)
{
	// Widget_ActivateState의 Visibility 설정
	if (Widget_ActivateState)
		Widget_ActivateState->SetVisibility( bIsSelected ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed );

	// Text 색상 변경
	if ( ownerButtonGroup.IsValid())
		Text_ButtonLabel->SetColorAndOpacity(bIsSelected ? ownerButtonGroup->GetSelectedColor() : ownerButtonGroup->GetUnselectedColor());
}

void UTabButton::SetLabel(const FText& InText)
{
	Text_ButtonLabel->SetText(InText);
}

void UTabButton::OnClicked()
{
	OnTabButtonClicked.Broadcast(tabIndex);
}
