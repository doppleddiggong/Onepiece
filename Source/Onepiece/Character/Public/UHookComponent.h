// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file UHookComponent.h
 * @brief Hook 대상 표시 컴포넌트
 * @details Actor에 부착하면 그래플링 훅의 대상이 될 수 있음을 표시
 */

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UHookComponent.generated.h"

/**
 * @brief Hook 대상 표시 컴포넌트
 * @details Actor에 부착하면 그래플링 훅의 대상이 될 수 있음
 * @ingroup Character
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ONEPIECE_API UHookComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHookComponent();

public:
	/** Hook 가능 여부 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hook")
	bool bIsHookable = true;

	/** Hook 부착 오프셋 (선택사항) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hook")
	FVector HookAttachOffset = FVector::ZeroVector;
};
