// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "TutorSpeechWidget.h"

#include "Components/TextBlock.h"

void UTutorSpeechWidget::SetInputText(FString InText)
{
	FString inputText;
	if (InText.Len() > maxTextLength)
	{
		inputText = InText.Left(maxTextLength) + "...";
	}
	else
	{
		inputText = InText;
	}
	Text_Speech->SetText(FText::FromString(inputText));
}
