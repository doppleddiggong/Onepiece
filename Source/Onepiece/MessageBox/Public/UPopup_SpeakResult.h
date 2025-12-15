// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "UBasePopup.h"
#include "UPopup_SpeakResult.generated.h"

/**
 * 
 */
UCLASS()
class ONEPIECE_API UPopup_SpeakResult : public UBasePopup
{
	GENERATED_BODY()

public:
	void InitPopup(const struct FResponseSpeakResult& ResponseData);

private:
	UFUNCTION(BlueprintCallable, Category = "Close")
	void OnClickClose();

	UFUNCTION()
	void InitSpeakResult(const struct FResponseSpeakResult& ResponseData);
	
protected:
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UImageButton> Btn_Confirm;
	
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UVerticalBox> VerticalBox;
	
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UResultStatWidget> Result_Grade;
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UResultStatWidget> Result_TopRate;
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UResultStatWidget> Result_AverageScore;

private:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UAnswerItem> AnswerItemClass;
};
