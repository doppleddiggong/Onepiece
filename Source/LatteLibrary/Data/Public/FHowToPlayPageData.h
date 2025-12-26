// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "FHowToPlayPageData.generated.h"

/// @brief HowToPlay 페이지 타입
UENUM(BlueprintType)
enum class EHowToPlayPageType : uint8
{
	Control		UMETA(DisplayName = "Control"),
	Read		UMETA(DisplayName = "Read"),
	Listen		UMETA(DisplayName = "Listen"),
	Speak		UMETA(DisplayName = "Speak"),
	Write		UMETA(DisplayName = "Write")
};

/// @brief HowToPlay 페이지 데이터
USTRUCT(BlueprintType)
struct FHowToPlayPageData : public FTableRowBase
{
	GENERATED_BODY()

	/// @brief 페이지 타입
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HowToPlay")
	EHowToPlayPageType PageType = EHowToPlayPageType::Control;

	/// @brief 페이지 제목
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HowToPlay")
	FText Title = FText::FromString(TEXT("Title"));

	/// @brief 페이지 설명
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HowToPlay")
	FText Description = FText::FromString(TEXT("Description"));

	/// @brief 페이지 아이콘 (선택적)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HowToPlay")
	TObjectPtr<UTexture2D> Texture = nullptr;
};