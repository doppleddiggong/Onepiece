// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "UBasePopup.h"
#include "Popup_AskTutorial.generated.h"

/**
 * 
 */
UCLASS()
class ONEPIECE_API UPopup_AskTutorial : public UBasePopup
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	
public:
	// 튜토리얼 수락
	UPROPERTY(meta = (BindWidget))
	class UImageButton* Btn_Yes;

	// 튜토리얼 스킵
	UPROPERTY(meta = (BindWidget))
	class UImageButton* Btn_Skip;

	UFUNCTION()
	void OnClickYes();

	UFUNCTION()
	void OnClickSkip();
	
};
