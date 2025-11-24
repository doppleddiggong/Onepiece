// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file UPlayTimer.cpp
 * @brief UPlayTimer의 동작을 구현합니다.
 */
#include "UPlayTimer.h"
#include "Components/TextBlock.h"

UPlayTimer::UPlayTimer(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UPlayTimer::UpdateTimerText(const FString& TimeText)
{
	if (RemainPlayTimeText)
	{
		RemainPlayTimeText->SetText(FText::FromString(TimeText));
	}
}
