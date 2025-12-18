// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "UBasePopup.h"
#include "NetworkData.h"
#include "ALingoGameState.h"
#include "Popup_Result.generated.h"

/**
 * 
 */
UCLASS()
class ONEPIECE_API UPopup_Result : public UBasePopup
{
	GENERATED_BODY()

public:
	virtual void NativeDestruct() override;
	
	void InitPopup(EQuestType InQuestType);

private:
	UFUNCTION(BlueprintCallable, Category = "Close")
	void OnClickClose();

	void InitWordWidget();
	void InitWrongList();

	UFUNCTION()
	void InitReadResult(const FResponseReadResult& ResponseData);
	UFUNCTION()
	void InitListenResult(const FResponseListenResult& ResponseData);

	void RequestResult();
	
	UFUNCTION()
	void OnResponseReadResult(FResponseReadResult& ResponseData, bool bWasSuccessful);
	UFUNCTION()
	void OnResponseListenResult(FResponseListenResult& ResponseData, bool bWasSuccessful);
	
protected:
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UTextBlock> Txt_Title;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UImage> Image_Symbol;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UImageButton> Btn_Confirm;
	
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UTextBlock> Txt_Kor;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UTextBlock> Txt_Eng;
	
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UVerticalBox> VerticalBox;
	
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UResultStatWidget> Result_Time;
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UResultStatWidget> Result_Grade;
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UResultStatWidget> Result_TopRate;
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UResultStatWidget> Result_AverageScore;

private:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UAnswerItem> AnswerItemClass;

	EQuestType QuestType;
};