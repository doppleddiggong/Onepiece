// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "UWordButton.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UWordButton::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Word)
	{
		Btn_Word->OnClicked.RemoveDynamic(this, &UWordButton::OnButtonClicked);
		Btn_Word->OnClicked.AddDynamic(this, &UWordButton::OnButtonClicked);
	}
}

void UWordButton::InitializeWordButton(const FString& InText, int32 InIndex)
{
	Index = InIndex;

	if (Txt_Word)
	{
		Txt_Word->SetText(FText::FromString(InText));
	}
}

void UWordButton::OnButtonClicked()
{
	OnWordButtonClicked.Broadcast(Index);
}
