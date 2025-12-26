// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "FHowToPlayPageData.generated.h"

/// @brief HowToPlay 페이지 타입
UENUM(BlueprintType)
enum class EHowToPlayPageType : uint8
{
	Control_P1		UMETA(DisplayName = "Control_P1"),
	Control_P2		UMETA(DisplayName = "Control_P2"),
	Control_P3		UMETA(DisplayName = "Control_P3"),
	Control_P4		UMETA(DisplayName = "Control_P4"),
	Control_P5		UMETA(DisplayName = "Control_P5"),
	Control_P6		UMETA(DisplayName = "Control_P6"),

	Read_P1		UMETA(DisplayName = "Read_P1"),
	Read_P2		UMETA(DisplayName = "Read_P2"),
	Read_P3		UMETA(DisplayName = "Read_P3"),
	Read_P4		UMETA(DisplayName = "Read_P4"),


	Listen_P1		UMETA(DisplayName = "Listen_P1"),
	Listen_P2		UMETA(DisplayName = "Listen_P2"),
	Listen_P3		UMETA(DisplayName = "Listen_P3"),
	Listen_P4		UMETA(DisplayName = "Listen_P4"),


	Speak_P1		UMETA(DisplayName = "Speak_P1"),
	Speak_P2		UMETA(DisplayName = "Speak_P2"),
	Speak_P3		UMETA(DisplayName = "Speak_P3"),


	Write_P1		UMETA(DisplayName = "Write_P1"),
	Write_P2		UMETA(DisplayName = "Write_P2"),
	Write_P3		UMETA(DisplayName = "Write_P3")
};

/// @brief HowToPlay 페이지 데이터
USTRUCT(BlueprintType)
struct FHowToPlayPageData : public FTableRowBase
{
	GENERATED_BODY()

	/// @brief 페이지 타입
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HowToPlay")
	EHowToPlayPageType PageType = EHowToPlayPageType::Control_P1;

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
