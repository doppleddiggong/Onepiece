// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StartWidget.generated.h"

/**
 * 
 */
UCLASS()
class ONEPIECE_API UStartWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Start;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Regist;

	UFUNCTION()
	void OnStartPressed();
	
	UFUNCTION()
	void OnRegistPressed();
};
