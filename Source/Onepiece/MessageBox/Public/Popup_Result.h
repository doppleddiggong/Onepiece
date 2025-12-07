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
	void InitPopup(EQuestType InQuestType);

private:
	UFUNCTION(BlueprintCallable, Category = "Close")
	void OnClickClose();

	void InitWordWidget();
	void InitWrongList();

	void InitReadResult(const FResponseReadResult& ResponseData);
	void InitListenResult(const FResponseListenResult& ResponseData);

	void RequestResult();
	
	UFUNCTION()
	void OnResponseReadResult(FResponseReadResult& ResponseData, bool bWasSuccessful);
	UFUNCTION()
	void OnResponseListenResult(FResponseListenResult& ResponseData, bool bWasSuccessful);
	
protected:
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UImageButton> Btn_OK;
	
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UTextureButton> Btn_Exit;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UTextBlock> Txt_Kor;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UTextBlock> Txt_Eng;
	
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UScrollBox> Scrl_WrongList;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UResultStatWidget> Result_Time;
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UResultStatWidget> Result_Grade;
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UResultStatWidget> Result_TopRate;
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UResultStatWidget> Result_AverageScore;

private:
	EQuestType QuestType;
};