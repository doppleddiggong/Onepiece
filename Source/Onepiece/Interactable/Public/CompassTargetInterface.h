// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ECompassMarkerType.h"
#include "CompassTargetInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UCompassTargetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ONEPIECE_API ICompassTargetInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	ECompassMarkerType MarkerType;
	
	ECompassMarkerType GetCompassMarkerInfo()
	{
		return MarkerType;
	};
	virtual void SetCompassMarkerInto(ECompassMarkerType InMarkerType)=0;
	
};
