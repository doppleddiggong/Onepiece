// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "NetworkData.h"
#include "FDailyStudyResult.generated.h"

USTRUCT(BlueprintType)
struct FDailyStudyResult
{
	GENERATED_BODY()

	/** 현재 점수 */
	UPROPERTY(BlueprintReadWrite, Category = "DailyStudy")
	int32 CurrentScore = 0;

	/** 질문 단어 데이터 리스트 (한글/영어/발음) */
	UPROPERTY(BlueprintReadWrite, Category = "DailyStudy")
	TArray<FWordData> QuestionList;

	/** 답변 리스트 */
	UPROPERTY(BlueprintReadWrite, Category = "DailyStudy")
	TArray<FResponseSpeakingJudes> AnswerList;
};