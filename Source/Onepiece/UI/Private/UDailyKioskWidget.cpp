// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "UDailyKioskWidget.h"
#include "UConfigLibrary.h"
#include "ULingoGameHelper.h"
#include "GameLogging.h"
#include "Components/TextBlock.h"

void UDailyKioskWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Best Score 로드 및 표시
	UpdateBestScore();
}

void UDailyKioskWidget::UpdateBestScore()
{
	// 플레이어 UserID 가져오기
	const int32 UserId = ULingoGameHelper::GetUserId(this);

	// ConfigLibrary에서 Daily Best Score 읽기
	const int32 BestScore = UConfigLibrary::GetUserInt(UserId, TEXT("DailyBestScore"), 0);

	// TextBlock에 표시
	if (Txt_BestScore)
	{
		FString ScoreText = FString::Printf(TEXT("Daily Best: %d"), BestScore);
		Txt_BestScore->SetText(FText::FromString(ScoreText));

		PRINTLOG(TEXT("[DailyKioskWidget] User %d - Best Score: %d"), UserId, BestScore);
	}
	else
	{
		PRINTLOG(TEXT("[DailyKioskWidget] ERROR: Txt_BestScore is null!"));
	}
}
