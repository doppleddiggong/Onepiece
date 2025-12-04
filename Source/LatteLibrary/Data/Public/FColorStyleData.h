// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "FColorStyleData.generated.h"


UENUM(BlueprintType)
enum class EColorStyleType : uint8
{
	Green,
	LightGreen,
	Yellow,
	Orange,
	Blue,
	Red,
	Purple,
	Gray
};

USTRUCT(BlueprintType)
struct LATTELIBRARY_API FColorStyleData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor BGColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor BorderColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor TitleColor;
};
