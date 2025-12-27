// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "NetworkData.h"
#include "EWordType.h"
#include "FDailyStudy.generated.h"

/**
 * @brief Daily Study 단어 아이템 데이터
 *
 * 로컬에서 사용하는 단어 데이터 구조체입니다.
 * UGameDataManager에서 로드한 데이터를 저장합니다.
 */
USTRUCT(BlueprintType)
struct FDailyStudyWordItem
{
	GENERATED_BODY()

	/** 단어 타입 (Animal, Color, Region, Food) */
	UPROPERTY(BlueprintReadWrite, Category = "DailyStudy")
	EWordType WordType;

	/** DataTable 인덱스 */
	UPROPERTY(BlueprintReadWrite, Category = "DailyStudy")
	int32 WordCode = 0;

	/** 영어 단어 (예: "Lion") */
	UPROPERTY(BlueprintReadWrite, Category = "DailyStudy")
	FString Eng;

	/** 한국어 번역 (예: "사자") */
	UPROPERTY(BlueprintReadWrite, Category = "DailyStudy")
	FString Kor;

	/** 단어 이미지 */
	UPROPERTY(BlueprintReadWrite, Category = "DailyStudy")
	TSoftObjectPtr<UTexture2D> Texture;
};

/**
 * @brief Daily Study 답변 데이터
 *
 * 각 문제에 대한 답변 정보를 저장합니다.
 */
USTRUCT(BlueprintType)
struct FDailyStudyAnswer
{
	GENERATED_BODY()

	/** 질문 인덱스 (0-based, 0~9) */
	UPROPERTY(BlueprintReadWrite, Category = "DailyStudy")
	int32 QuestionIndex = 0;

	/** 정답 단어 (영어) */
	UPROPERTY(BlueprintReadWrite, Category = "DailyStudy")
	FString ExpectedAnswer;

	/** 녹음 파일 경로 */
	UPROPERTY(BlueprintReadWrite, Category = "DailyStudy")
	FString UserAudioPath;

	/** API 응답 (점수) */
	UPROPERTY(BlueprintReadWrite, Category = "DailyStudy")
	FResponseSpeakingJudes JudgeResult;

	/** 완료 여부 */
	UPROPERTY(BlueprintReadWrite, Category = "DailyStudy")
	bool bCompleted = false;

	/** 건너뛰기 여부 */
	UPROPERTY(BlueprintReadWrite, Category = "DailyStudy")
	bool bSkipped = false;
};


USTRUCT(BlueprintType)
struct FDailyStudyResult
{
	GENERATED_BODY()

	/** 평균 문법 점수 */
	UPROPERTY(BlueprintReadWrite, Category = "DailyStudy")
	int32 AvgGrammarScore = 0;

	/** 평균 맥락 점수 */
	UPROPERTY(BlueprintReadWrite, Category = "DailyStudy")
	int32 AvgContextScore = 0;

	/** 평균 최종 점수 */
	UPROPERTY(BlueprintReadWrite, Category = "DailyStudy")
	int32 AvgFinalScore = 0;

	/** 완료한 문제 개수 */
	UPROPERTY(BlueprintReadWrite, Category = "DailyStudy")
	int32 CompletedCount = 0;

	/** 건너뛴 문제 개수 */
	UPROPERTY(BlueprintReadWrite, Category = "DailyStudy")
	int32 SkippedCount = 0;

	/** 총 문제 개수 */
	UPROPERTY(BlueprintReadWrite, Category = "DailyStudy")
	int32 TotalCount = 0;
};