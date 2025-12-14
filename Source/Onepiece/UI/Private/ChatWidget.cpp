// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "ChatWidget.h"

#include "ChatBoxWidget.h"
#include "ChatInputBox.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"

void UChatWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	ChatInputBox->OnSendClicked.AddUObject(this, &UChatWidget::OnSendMessage);
}

void UChatWidget::OnSendMessage()
{
	FText message = ChatInputBox->GetMessage();
	if (!message.IsEmpty())
	{
		UChatBoxWidget* newChat = NewObject<UChatBoxWidget>();
		newChat->TextBlock_MessageContent->SetText(message);
		
		// TODO: 플레이어 이름 넣기
		// TODO: 플레이어 색상 넣기
		
		// ChatBox에 추가
		ScrollBox_ChatBox->AddChild(newChat);
	}
}
