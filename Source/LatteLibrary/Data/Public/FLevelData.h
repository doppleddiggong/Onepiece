// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file FLevelData.h
 * @brief FLevelData 구조체를 선언합니다.
 */
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "FLevelData.generated.h"

/**
 * @brief 레벨 데이터를 정의하는 구조체
 * @details 단계별, 레벨별 제한 시간 정보를 저장합니다.
 */
USTRUCT(BlueprintType)
struct LATTELIBRARY_API FLevelData : public FTableRowBase
{
	GENERATED_BODY()

	/** @brief 레벨 데이터 인덱스 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelData")
	int32 Index = 0;

	/** @brief 게임 단계 (Step) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelData")
	int32 Step = 1;

	/** @brief 난이도 레벨 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelData")
	int32 Level = 1;

	/** @brief 제한 시간 (초) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelData", meta = (ClampMin = "0"))
	float Time = 300.0f;
};
