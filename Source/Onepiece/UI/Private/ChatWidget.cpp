// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "ChatWidget.h"

#include "APlayerControl.h"
#include "ChatBoxWidget.h"
#include "ChatInputBox.h"
#include "GameLogging.h"
#include "Components/Border.h"
#include "Components/ScrollBox.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"

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
}

void UChatWidget::SendMessage(FText inMessage)
{
	// 현재 스크롤 값 & 마지막 스크롤 값
	float scrollOffset = ScrollBox_ChatBox->GetScrollOffset();
	float scrollOffsetOfEnd = ScrollBox_ChatBox->GetScrollOffsetOfEnd();
	
	UChatBoxWidget* newChat = CreateWidget<UChatBoxWidget>(GetWorld(), ChatBoxWidgetClass);
	newChat->SetContent(inMessage);

	auto* PC = Cast<APlayerControl>(GetWorld()->GetFirstPlayerController<APlayerController>());
	if (!PC)
	{
		PRINTLOG(TEXT("Cannot get playercontroller"));
	}
	
	// 플레이어 정보 가져오기
	FResponseUserMe info = PC->GetUserInfo();
	// TODO: 플레이어 색상 넣기
	newChat->SetPlayerBGColor(FColor::FromHex(TEXT("E94C4CFF")));
	// 플레이어 이름 넣기
	newChat->SetPlayerName(FText::FromString(info.username));
	
	scrollOffset = ScrollBox_ChatBox->GetScrollOffset();
	scrollOffsetOfEnd = ScrollBox_ChatBox->GetScrollOffsetOfEnd();
	PRINTLOG(TEXT("before scrollend - ScrollOffset: %f, ScrollOffsetOfEnd: %f"), scrollOffset, scrollOffsetOfEnd);
	ScrollBox_ChatBox->SetScrollOffset(scrollOffsetOfEnd);
	
	// ChatBox에 추가
	ScrollBox_ChatBox->AddChild(newChat);
	
	// 스크롤이 맨 마지막일 때
	if (scrollOffset == scrollOffsetOfEnd)
	{		
		FTimerHandle handle;
		GetWorld()->GetTimerManager().SetTimer(handle, [this, &scrollOffset, &scrollOffsetOfEnd]()
		{
			// 스크롤 위치를 맨 끝으로 해라!
			ScrollBox_ChatBox->ScrollToEnd();
		}, 0.1f, false);
	}
}
