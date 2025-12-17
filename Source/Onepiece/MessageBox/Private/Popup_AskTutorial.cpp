// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "Popup_AskTutorial.h"

#include "UImageButton.h"

void UPopup_AskTutorial::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Yes)
	{
		Btn_Yes->OnButtonClickedEvent.RemoveDynamic(this, &UPopup_AskTutorial::OnClickYes);
		Btn_Yes->OnButtonClickedEvent.AddDynamic(this, &UPopup_AskTutorial::OnClickYes);
	}

	if (Btn_Skip)
	{
		Btn_Skip->OnButtonClickedEvent.RemoveDynamic(this, &UPopup_AskTutorial::OnClickSkip);
		Btn_Skip->OnButtonClickedEvent.AddDynamic(this, &UPopup_AskTutorial::OnClickSkip);
	}
}

void UPopup_AskTutorial::OnClickYes()
{
	
}

void UPopup_AskTutorial::OnClickSkip()
{
	
}
