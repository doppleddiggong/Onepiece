// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "ChatBoxWidget.h"

#include "FResourceTextureData.h"
#include "UGameDataManager.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UChatBoxWidget::SetPlayerName(FText InPlayerName) const
{
	Txt_Name->SetText(InPlayerName);
}

void UChatBoxWidget::SetMessage(FText InMessage) const
{
	Txt_Message->SetText(InMessage);
}

void UChatBoxWidget::SetPlayerProfile(
	FLinearColor InBgColor,
	EResourceTextureType InProfileType)
{
	Border_ProfileBG->SetBrushColor(InBgColor);

	UTexture2D* Texture = UGameDataManager::Get(this)->GetTexture(InProfileType);
	if (!Texture)
		return;

	FSlateBrush Brush = Image_Profile->GetBrush();
	Brush.SetResourceObject(Texture);
	Image_Profile->SetBrush(Brush);
}

void UChatBoxWidget::SetChatBubbleColor(bool IsMine) const
{
	auto BubbleColor = IsMine ? FLinearColor::Green : FLinearColor::White;

	Txt_Name->SetColorAndOpacity(BubbleColor);
	Image_ChatTail->SetColorAndOpacity(BubbleColor);
	Border_ChatBubble->SetContentColorAndOpacity(BubbleColor);
}