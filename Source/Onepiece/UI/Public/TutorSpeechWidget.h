// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TutorSpeechWidget.generated.h"

/**
 * 
 */
UCLASS()
class ONEPIECE_API UTutorSpeechWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetInputText(FString InText);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<class UTextBlock> Text_Speech;
	
private:
	const int32 maxTextLength = 35;
};
