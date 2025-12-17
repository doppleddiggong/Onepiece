// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "ChatWidget.h"

#include "ChatBoxWidget.h"
#include "ChatInputBox.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"

UChatWidget::UChatWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UChatBoxWidget> ChatBoxWidgetClassRef(TEXT("/Game/CustomContents/UI/Widgets/Chat/WBP_ChatMessage.WBP_ChatMessage_C"));
	if (ChatBoxWidgetClassRef.Succeeded())
	{
		ChatBoxWidgetClass = ChatBoxWidgetClassRef.Class;
	}
}

void UChatWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	ChatInputBox->OnSendClicked.AddUObject(this, &UChatWidget::OnSendMessage);
}

void UChatWidget::OnSendMessage(FText inMessage)
{
	UChatBoxWidget* newChat = CreateWidget<UChatBoxWidget>(this, ChatBoxWidgetClass);
	newChat->TextBlock_MessageContent->SetText(inMessage);
	
	// TODO: 플레이어 이름 넣기
	// TODO: 플레이어 색상 넣기
	
	
	// ChatBox에 추가
	ScrollBox_ChatBox->AddChild(newChat);
}
