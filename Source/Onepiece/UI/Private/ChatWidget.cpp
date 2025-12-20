// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "ChatWidget.h"

#include "APlayerControl.h"
#include "ChatBoxWidget.h"
#include "ChatInputBox.h"
#include "GameLogging.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/ScrollBox.h"
#include "Components/ScrollBoxSlot.h"
#include "Components/VerticalBox.h"
#include "GameFramework/GameStateBase.h"

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

	SetIsFocusable(true);
}

void UChatWidget::SendMessage(FResponseUserMe sendUser, FText inMessage, int32 PlayerIndex)
{
	auto* PC = Cast<APlayerControl>(GetWorld()->GetFirstPlayerController<APlayerController>());
	if (!PC)
	{
		PRINTLOG(TEXT("Cannot get playercontroller"));
	}

	// 메시지 받은 플레이어 정보 가져오기
	FResponseUserMe info = PC->GetUserInfo();
	bool bIsSender = (info.id == sendUser.id);

	// ChatBox 생성
	UChatBoxWidget* newChat = CreateChatBox(bIsSender);
	newChat->SetMessage(inMessage);

	newChat->SetPlayerProfile(
		sendUser.GetChatProfileBg(PlayerIndex),
		sendUser.GetChatProfileTextureType(PlayerIndex));

	newChat->SetPlayerName(FText::FromString(sendUser.username));
	newChat->SetMessage(inMessage);
	newChat->SetChatBubbleColor(bIsSender);

	// ChatBox에 추가 & 정렬
	VerticalBox_Content->AddChild(newChat);

	UScrollBoxSlot* parentSlot = Cast<UScrollBoxSlot>(newChat->Slot);
	if (parentSlot)
	{
		parentSlot->SetHorizontalAlignment((bIsSender ? HAlign_Left : HAlign_Right));
	}

	// 위젯의 레이아웃을 강제로 갱신하여 정확한 크기 계산
	newChat->ForceLayoutPrepass();

	// 부모 위젯들의 레이아웃을 무효화하여 다음 틱에 재계산
	VerticalBox_Content->InvalidateLayoutAndVolatility();
	ScrollBox_ChatBox->InvalidateLayoutAndVolatility();

	// 현재 스크롤 값 & 마지막 스크롤 값
	float scrollOffset = ScrollBox_ChatBox->GetScrollOffset();
	float scrollOffsetOfEnd = ScrollBox_ChatBox->GetScrollOffsetOfEnd();
	PRINTLOG(TEXT("before scrollend - ScrollOffset: %f, ScrollOffsetOfEnd: %f"), scrollOffset, scrollOffsetOfEnd);
	
	if (bIsSender || scrollOffset == scrollOffsetOfEnd)
	{
		// 레이아웃 갱신 후 스크롤 (타이머 사용)
		FTimerHandle ScrollTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(ScrollTimerHandle, [this, newChat]()
		{
			// 마지막 위젯을 뷰에 표시
			ScrollBox_ChatBox->ScrollWidgetIntoView(newChat, true, EDescendantScrollDestination::BottomOrRight);

			// 추가로 ScrollToEnd도 실행
			ScrollBox_ChatBox->ScrollToEnd();
		}, 0.1f, false);
	}

	UWidgetBlueprintLibrary::SetFocusToGameViewport();
	PC->SetInputMode(FInputModeGameOnly());
	PC->SetShowMouseCursor(false);
}

void UChatWidget::FocusInput()
{
	ChatInputBox->SetInputFocus(true);
}

UChatBoxWidget* UChatWidget::CreateChatBox(bool bIsSender)
{
	return CreateWidget<UChatBoxWidget>(GetWorld(), bIsSender? LeftChatBoxWidgetClass : RightChatBoxWidgetClass );
}

