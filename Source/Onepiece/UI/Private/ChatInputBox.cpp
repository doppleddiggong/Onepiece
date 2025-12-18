// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "ChatInputBox.h"

#include "APlayerControl.h"
#include "GameLogging.h"
#include "Components/Button.h"
#include "Components/MultiLineEditableTextBox.h"

void UChatInputBox::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	Button_Send->OnClicked.AddDynamic(this, &UChatInputBox::HandleSendClicked);
}

void UChatInputBox::NativeOnFocusLost(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnFocusLost(InFocusEvent);
	
	if (APlayerControl* PC = Cast<APlayerControl>(GetOwningPlayer()))
	{
		FInputModeGameOnly inputMode;
		PC->SetInputMode(inputMode);
	}
}

FReply UChatInputBox::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	// Enter 키 감지 & Shift+Enter는 줄바꿈 허용
	if (InKeyEvent.GetKey() == EKeys::Enter && !InKeyEvent.IsShiftDown())
	{
		HandleSendClicked();
		return FReply::Handled();
	}
	
	return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
}

FText UChatInputBox::FlushMessage()
{
	// message에 저장 & 텍스트 칸 비우기
	FText message = MultiLineEditableTextBox_Input->GetText();
	
	MultiLineEditableTextBox_Input->SetText(FText::GetEmpty());
	
	return message;
}

void UChatInputBox::FocusInput()
{
	MultiLineEditableTextBox_Input->SetKeyboardFocus();
}

void UChatInputBox::HandleSendClicked()
{
	// 텍스트가 비어있지 않다면 Broadcast
	FText message = FlushMessage();
	if (!message.IsEmpty())
	{		
		if (auto* PC = Cast<APlayerControl>(GetWorld()->GetFirstPlayerController<APlayerController>()))
		{
			PC->ServerRPC_SendChat(message);
		}
	}
}


