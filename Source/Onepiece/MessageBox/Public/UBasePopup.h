// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "EPopupType.h"
#include "Blueprint/UserWidget.h"
#include "UBasePopup.generated.h"

/**
 * 
 */
UCLASS()
class ONEPIECE_API UBasePopup : public UUserWidget
{
	GENERATED_BODY()

protected:
	EPopupType PopupType;
};
