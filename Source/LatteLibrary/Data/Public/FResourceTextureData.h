// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Engine/Texture2D.h"
#include "FResourceTextureData.generated.h"


UENUM(BlueprintType)
enum class EResourceTextureType : uint8
{
	Rarity_D,
	Rarity_C,
	Rarity_B,
	Rarity_A,
	Rarity_S,

	KLingo,

	Time,
	Score,
	Color,

	Read,
	Listen,
	Speak,
	Write,

	Grammar, 
	Context,	
	Overall,

	GreenOwl,
	RedOwl,
	Bot,
};

USTRUCT(BlueprintType)
struct LATTELIBRARY_API FResourceTextureData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> Texture;
};
