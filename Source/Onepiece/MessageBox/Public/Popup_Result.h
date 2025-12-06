// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "UBasePopup.h"
#include "NetworkData.h"
#include "Popup_Result.generated.h"

/**
 * 
 */
UCLASS()
class ONEPIECE_API UPopup_Result : public UBasePopup
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	void InitPopup();

private:
	UFUNCTION(BlueprintCallable, Category = "Close")
	void OnClickClose();

	void SetWordWidget();
	void SetWrongList();
	void SetTimeTaken();

	void RequestReadResult();

	UFUNCTION()
	void OnResponseReadResult(FResponseReadResult& ResponseData, bool bWasSuccessful);
	
protected:
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UImageButton> Btn_OK;
	
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UTextureButton> Btn_Exit;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UTextBlock> Txt_Kor;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UTextBlock> Txt_Eng;
	
	// 오답 표시
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UScrollBox> Scrl_WrongList;


	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UTextBlock> Txt_TimeRank;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UTextBlock> Txt_TimeTaken;

	// 정답 정확도
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UTextBlock> Txt_Accuracy;

	// 랭킹 백분율
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UCircularProgressBar> CircleBar_Ranking;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UTextBlock> Txt_Rank;
};
