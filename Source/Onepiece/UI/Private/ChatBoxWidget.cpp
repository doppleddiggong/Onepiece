// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "ChatBoxWidget.h"

#include "Components/Border.h"
#include "Components/TextBlock.h"

void UChatBoxWidget::SetContent(FText inMessage)
{
	TextBlock_MessageContent->SetText(inMessage);
}

void UChatBoxWidget::SetPlayerName(FText inPlayerName)
{
	TextBlock_PlayerName->SetText(inPlayerName);
}

void UChatBoxWidget::SetPlayerBGColor(FLinearColor inColor)
{
	Border_PlayerBG->SetBrushColor(inColor);
}
