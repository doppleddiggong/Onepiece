// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "ChatWidget.h"

#include "APlayerControl.h"
#include "ChatBoxWidget.h"
#include "GameLogging.h"
#include "Components/ScrollBox.h"
#include "Components/VerticalBox.h"

UChatWidget::UChatWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UChatBoxWidget> LeftChatBoxWidgetClassRef(TEXT("/Game/CustomContents/UI/Widgets/Chat/WBP_LeftChatMessage.WBP_LeftChatMessage_C"));
	if (LeftChatBoxWidgetClassRef.Succeeded())
	{
		LeftChatBoxWidgetClass = LeftChatBoxWidgetClassRef.Class;
	}
	ConstructorHelpers::FClassFinder<UChatBoxWidget> RightChatBoxWidgetClassRef(TEXT("/Game/CustomContents/UI/Widgets/Chat/WBP_RightChatMessage.WBP_RightChatMessage_C"));
	if (RightChatBoxWidgetClassRef.Succeeded())
	{
		RightChatBoxWidgetClass = RightChatBoxWidgetClassRef.Class;
	}
}

void UChatWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UChatWidget::SendMessage(FResponseUserMe sendUser, FText inMessage)
{
	auto* PC = Cast<APlayerControl>(GetWorld()->GetFirstPlayerController<APlayerController>());
	if (!PC)
	{
		PRINTLOG(TEXT("Cannot get playercontroller"));
	}
	
	// 플레이어 정보 가져오기
	FResponseUserMe info = PC->GetUserInfo();
	
	UChatBoxWidget* newChat = nullptr;
	
	// 플레이어에 따라 분류
	if (info.id == sendUser.id)
	{
		// 왼쪽 정렬
		PRINTLOG(TEXT("left - sendUserId : %d"), sendUser.id);
		
		newChat = CreateWidget<UChatBoxWidget>(GetWorld(), LeftChatBoxWidgetClass);
	}
	else
	{
		// 오른쪽 정렬
		PRINTLOG(TEXT("right - sendUserId : %d"), sendUser.id);
		
		newChat = CreateWidget<UChatBoxWidget>(GetWorld(), RightChatBoxWidgetClass);
	}
	
	newChat->SetContent(inMessage);
	
	// TODO: 플레이어 색상 넣기
	newChat->SetPlayerBGColor(FColor::FromHex(TEXT("E94C4CFF")));
	// 플레이어 이름 넣기
	newChat->SetPlayerName(FText::FromString(info.username));

	// 현재 스크롤 값 & 마지막 스크롤 값
	float scrollOffset = ScrollBox_ChatBox->GetScrollOffset();
	float scrollOffsetOfEnd = ScrollBox_ChatBox->GetScrollOffsetOfEnd();
	PRINTLOG(TEXT("before scrollend - ScrollOffset: %f, ScrollOffsetOfEnd: %f"), scrollOffset, scrollOffsetOfEnd);
	
	ScrollBox_ChatBox->SetScrollOffset(scrollOffsetOfEnd);
	
	// ChatBox에 추가
	ScrollBox_ChatBox->AddChild(newChat);
	
	//  스크롤이 맨 마지막일 때
	if (info.id == sendUser.id || scrollOffset == scrollOffsetOfEnd)
	{		
		FTimerHandle handle;
		GetWorld()->GetTimerManager().SetTimer(handle, [this, &scrollOffset, &scrollOffsetOfEnd]()
		{
			// 스크롤 위치를 맨 끝으로 해라!
			ScrollBox_ChatBox->ScrollToEnd();
		}, 0.1f, false);
	}
}