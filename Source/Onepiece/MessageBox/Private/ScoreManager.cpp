// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "ScoreManager.h"

#include "ALingoGameState.h"

UScoreManager::UScoreManager()
{
	
}

void UScoreManager::GetTimeRank(float InTimeTaken, FString& Result)
{
	Result = "D";
	// 일단 하드코딩
	if (InTimeTaken <= 300) Result = "C";
	if (InTimeTaken <= 240) Result = "B";
	if (InTimeTaken <= 180) Result = "A";
}

void UScoreManager::GetAccuracyPercentage(FString& OutPercentage)
{
	ALingoGameState* GS = Cast<ALingoGameState>(GetWorld()->GetGameState());
	if (GS)
	{
		// 틀린 개수
		int32 WrongCnt = GS->WrongLuggageList.Num();
		
		// 정답률 계산
		float Percentage = ((10.f - WrongCnt) / 10.f) * 100.f;
		int32 RoundedPercentage = FMath::RoundToInt(Percentage);
		
		// FString으로 변환
		OutPercentage = FString::Printf(TEXT("%d%%"), RoundedPercentage);
	}
}
