// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "UTutorMessage.h"
#include "Components/TextBlock.h"

void UTutorMessage::SetMessageText(const FText& NewMessage)
{
	if (Txt_Message)
	{
		Txt_Message->SetText(NewMessage);
	}
}