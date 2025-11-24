// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file FWordStudyData.h
 * @brief FWordStudyData 구조체를 선언합니다.
 */
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "FWordStudyData.generated.h"

/**
 * @brief 단어 학습 데이터를 정의하는 구조체
 * @details 한국어, 영어, 발음 정보를 저장합니다.
 */
USTRUCT(BlueprintType)
struct LATTELIBRARY_API FWordStudyData : public FTableRowBase
{
	GENERATED_BODY()

	/** @brief 단어 데이터 인덱스 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WordData")
	int32 Index = 0;

	/** @brief 한국어 문장 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WordData")
	FString Kor;

	/** @brief 영어 번역 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WordData")
	FString Eng;

	/** @brief 발음 (로마자 표기) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WordData")
	FString Pronunciation;
};
