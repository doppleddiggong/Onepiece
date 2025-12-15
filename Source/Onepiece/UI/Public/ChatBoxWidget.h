// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatBoxWidget.generated.h"

/**
 * 
 */
UCLASS()
class ONEPIECE_API UChatBoxWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> TextBlock_PlayerName;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> TextBlock_MessageContent;
};
