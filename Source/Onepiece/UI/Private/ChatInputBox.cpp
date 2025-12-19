// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "ChatInputBox.h"

#include "APlayerControl.h"
#include "GameLogging.h"
#include "Components/Button.h"
#include "Components/MultiLineEditableTextBox.h"
#include "Onepiece/Onepiece.h"

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

bool UChatInputBox::IsAIAsk(const FString& InMessage, FString& OutQuestion) const
{
	FString LeftPart, RightPart;

	// 콜론(:)을 기준으로 분리
	if (InMessage.Split(TEXT(":"), &LeftPart, &RightPart))
	{
		// 왼쪽 파트의 공백을 제거하고 "AI"와 일치하는지 확인 (대소문자 무시)
		if (LeftPart.TrimStartAndEnd().Equals(GameName::AI, ESearchCase::IgnoreCase))
		{
			OutQuestion = RightPart.TrimStart();
			return !OutQuestion.IsEmpty(); // 내용이 비어있지 않아야 true
		}
	}
	return false;
}

void UChatInputBox::HandleSendClicked()
{
	// 텍스트가 비어있지 않다면 처리
	FText Message = FlushMessage();
	if (Message.IsEmpty())
		return;

	auto* PC = Cast<APlayerControl>(GetWorld()->GetFirstPlayerController());
	if (!PC)
		return;

	const FString MessageStr = Message.ToString();
	FString CleanQuestion;

	if (IsAIAsk(MessageStr, CleanQuestion))
	{
		// 사용자가 입력한 전체 메시지 표시 (로그성)
		PC->ServerRPC_SendChat(Message);

		// AI에게 정제된 질문 전송
		PC->ServerRPC_SendAIQuestion(CleanQuestion);

		PRINTLOG(TEXT("[AI Chat] User question: %s"), *CleanQuestion);
	}
	else
	{
		// 2. 일반 채팅 메시지 전송
		PC->ServerRPC_SendChat(Message);
	}
}