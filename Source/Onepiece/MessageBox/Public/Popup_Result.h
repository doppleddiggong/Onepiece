// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "UBasePopup.h"
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

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	class UImageButton* Btn_OK;
	
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	class UTextureButton* Btn_Exit;

private:
	UFUNCTION(BlueprintCallable, Category = "Close")
	void OnClickClose();

public:
	// 정답 표시
	// UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	// class UWordWidget* WordWidget;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UTextBlock> Txt_Kor;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UTextBlock> Txt_Eng;
	
	// 오답 표시
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	class UScrollBox* Scrl_WrongList;

	void SetWordWidget();
	void SetWrongList();
	
public:
	// 걸린 시간
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	class UTextBlock* Txt_TimeRank;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	class UTextBlock* Txt_TimeTaken;

	float TimeTaken = -1;

	void SetTimeRank();
	void SetTimeTaken();

public:
	// 정답 정확도
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	class UTextBlock* Txt_Accuracy;

	void SetAccuracy();
};
