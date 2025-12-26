// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "EPageScrollDirection.generated.h"

/// @brief 페이지 스크롤 방향을 정의하는 열거형
UENUM(BlueprintType)
enum class EPageScrollDirection : uint8
{
	/// @brief 수평 스크롤 (좌우)
	Horizontal UMETA(DisplayName = "Horizontal"),

	/// @brief 수직 스크롤 (상하)
	Vertical UMETA(DisplayName = "Vertical")
};
