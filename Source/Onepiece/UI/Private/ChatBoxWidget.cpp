// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "ChatBoxWidget.h"

#include "Components/TextBlock.h"

void UChatBoxWidget::SetContent(FText inMessage)
{
	TextBlock_MessageContent->SetText(inMessage);
}
