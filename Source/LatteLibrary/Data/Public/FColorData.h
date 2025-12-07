// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file FColorData.h
 * @brief FColorData 구조체를 선언합니다.
 */
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "FColorData.generated.h"

/**
 * @brief 색상 데이터를 정의하는 구조체
 * @details 레벨별 색상 정보와 16진수 색상 값을 저장합니다.
 */
USTRUCT(BlueprintType)
struct LATTELIBRARY_API FColorData : public FTableRowBase
{
	GENERATED_BODY()

	/** @brief 색상 인덱스 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ColorData")
	int32 Index = 0;

	/** @brief 난이도 레벨 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ColorData")
	int32 Level = 1;

	/** @brief 색상 설명 (한글명) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ColorData")
	FString Desc;

	/** @brief 16진수 색상 코드 (예: #FF0000) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ColorData")
	FString HexColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ColorData")
	FString Eng;


	/** @brief HexColor 문자열을 FLinearColor로 변환하여 반환 */
	FLinearColor GetLinearColor() const
	{
		if (HexColor.IsEmpty())
		{
			return FLinearColor::White;
		}

		// '#' 있어도 없어도 처리 가능
		FString Parsed = HexColor;
		Parsed.RemoveFromStart(TEXT("#"));

		// FColor 파싱 (6자리/8자리 자동 처리)
		FColor SRGBColor = FColor::FromHex(Parsed);

		// FLinearColor로 변환 (sRGB → Linear)
		return FLinearColor::FromSRGBColor(SRGBColor);
	}
};
