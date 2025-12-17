#pragma once

#include "CoreMinimal.h"
#include "ETutorialStep.generated.h"

/**
 * 튜토리얼 진행 단계
 */
UENUM(BlueprintType)
enum class ETutorialStep : uint8
{
	Waiting     UMETA(DisplayName = "None"),
	
	MouseLook   UMETA(DisplayName = "Mouse Look"),
	Movement    UMETA(DisplayName = "Movement"),
	Sprint   UMETA(DisplayName = "Sprint"),
	Jump        UMETA(DisplayName = "Jump"),
	PickUp      UMETA(DisplayName = "Pickup"), // 왼쪽 마우스 : 집어올리기
	GrabGun		UMETA(DisplayName = "GrabGun"), // 오른쪽 마우스 : 멀리 있는거 가져오기
	Interaction UMETA(DisplayName = "Interaction"), // E 버튼 상호작용

	Completed   UMETA(DisplayName = "Completed")
};
