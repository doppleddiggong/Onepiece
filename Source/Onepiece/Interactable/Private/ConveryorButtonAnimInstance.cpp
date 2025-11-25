// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "ConveryorButtonAnimInstance.h"

#include "ConveyorButton.h"
#include "GameLogging.h"

void UConveryorButtonAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	ButtonActor = Cast<AConveyorButton>(GetOwningActor());
	if (ButtonActor)
	{
		bIsButtonOn = ButtonActor->GetIsButtonOn();
	}
}
