// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file UPlayTimer.cpp
 * @brief UPlayTimer의 동작을 구현합니다.
 */
#include "UPlayTimer.h"
#include "Components/TextBlock.h"

void UPlayTimer::UpdateTimerText(const float InRemainTime)
{
	const int32 TotalMilliseconds = FMath::FloorToInt(InRemainTime * 1000.f);

	const int32 Minutes       = TotalMilliseconds / 60000;
	const int32 Seconds       = (TotalMilliseconds / 1000) % 60;
	const int32 Milliseconds  = (TotalMilliseconds % 1000) / 10;

	Txt_MinSec->SetText(FText::FromString(FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds )));
	Txt_MilliSec->SetText(FText::FromString( FString::Printf(TEXT(".%02d"), Milliseconds)));
}
