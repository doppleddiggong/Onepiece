// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ConveryorButtonAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ONEPIECE_API UConveryorButtonAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class AConveyorButton> ButtonActor;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsButtonOn = false;
};
