// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file FListenData.h
 * @brief FListenData 구조체를 선언합니다.
 */
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "FListenData.generated.h"

/**
 * @brief 듣기 학습 데이터를 정의하는 구조체
 * @details 단계별, 레벨별, 카테고리별 단어 정보를 저장합니다.
 */
USTRUCT(BlueprintType)
struct LATTELIBRARY_API FListenData : public FTableRowBase
{
	GENERATED_BODY()

	/** @brief 듣기 데이터 인덱스 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ListenData")
	int32 Index = 0;

	/** @brief 게임 단계 (Step) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ListenData")
	int32 Step = 1;

	/** @brief 난이도 레벨 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ListenData")
	int32 Level = 1;

	/** @brief 카테고리 (예: Region, Food 등) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ListenData")
	FString Category;

	/** @brief 학습 단어 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ListenData")
	FString Word;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ListenData")
	FString Eng;
};
