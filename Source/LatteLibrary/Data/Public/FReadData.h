// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file FReadData.h
 * @brief FReadData 구조체를 선언합니다.
 */
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "FReadData.generated.h"

/**
 * @brief 읽기 학습 데이터를 정의하는 구조체
 * @details 레벨별 단어 학습 정보를 저장합니다.
 */
USTRUCT(BlueprintType)
struct LATTELIBRARY_API FReadData : public FTableRowBase
{
	GENERATED_BODY()

	/** @brief 읽기 데이터 인덱스 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReadData")
	int32 Index = 0;

	/** @brief 난이도 레벨 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReadData")
	int32 Level = 1;

	/** @brief 학습 단어 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReadData")
	FString Word;
};
