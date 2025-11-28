// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "ScoreManager.h"

UScoreManager::UScoreManager()
{
	
}

void UScoreManager::GetTimeRank(float InTimeTaken, FString& Result)
{
	Result = "D";
	// 일단 하드코딩 (A~D)
	if (InTimeTaken < 300) Result = "C";
}
