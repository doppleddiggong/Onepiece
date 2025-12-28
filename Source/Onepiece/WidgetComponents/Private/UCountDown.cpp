// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "UCountDown.h"

#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Animation/WidgetAnimation.h"
#include "GameLogging.h"

void UCountDown::NativeDestruct()
{
	Super::NativeDestruct();

	// 타이머 정리
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(CountDownTimerHandle);
	}
}

void UCountDown::StartCountDown(int32 InStartValue)
{
	if (InStartValue <= 0)
	{
		PRINTLOG(TEXT("[CountDown] Invalid start value: %d"), InStartValue);
		return;
	}

	CountDownValue = InStartValue;

	// 위젯 표시
	if (Border_CountDown)
	{
		Border_CountDown->SetVisibility(ESlateVisibility::Visible);
	}

	// 첫 번째 숫자 표시
	DisplayCurrentNumber();

	// 1초마다 업데이트
	GetWorld()->GetTimerManager().SetTimer(
		CountDownTimerHandle,
		this,
		&UCountDown::UpdateCountDown,
		1.0f,
		true
	);
}

void UCountDown::UpdateCountDown()
{
	CountDownValue--;

	if (CountDownValue > 0)
	{
		// 다음 숫자 표시
		DisplayCurrentNumber();
	}
	else
	{
		// 카운트다운 종료
		GetWorld()->GetTimerManager().ClearTimer(CountDownTimerHandle);

		// 위젯 숨기기
		if (Border_CountDown)
		{
			Border_CountDown->SetVisibility(ESlateVisibility::Hidden);
		}

		// Delegate 호출
		OnCountDownFinished.Broadcast();
	}
}

void UCountDown::DisplayCurrentNumber()
{
	// 텍스트 업데이트
	if (Txt_CountDown)
	{
		Txt_CountDown->SetText(FText::AsNumber(CountDownValue));
	}

	// 애니메이션 재생 (블루프린트에서 설정된 경우)
	if (Anim_CountDown)
	{
		PlayAnimation(Anim_CountDown);
	}
}
