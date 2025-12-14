// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "ChatInputBox.h"

#include "Components/Button.h"
#include "Components/MultiLineEditableTextBox.h"
#include "DataWrappers/ChaosVDQueryDataWrappers.h"

void UChatInputBox::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	Button_Send->OnClicked.AddDynamic(this, &UChatInputBox::HandleSendClicked);
}

FText UChatInputBox::GetMessage()
{
	return MultiLineEditableTextBox_Input->GetText();
}

void UChatInputBox::HandleSendClicked()
{
	// 텍스트가 비어있지 않다면 Broadcast
	if (!MultiLineEditableTextBox_Input->GetText().IsEmpty())
	{
		OnSendClicked.Broadcast();
	}
}


