// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "NetworkData.h"
#include "Blueprint/UserWidget.h"
#include "Popup_QuestionnaireItem.generated.h"

/**
 * 
 */
UCLASS()
class ONEPIECE_API UPopup_QuestionnaireItem : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	
public:
	/** 질문 항목 초기화 */ 
	UFUNCTION(BlueprintCallable)
	void InitItem(const FWriteQuestionData& Data);

protected:
	/* ----------------- Layout ----------------- */
	/** "Question.01" 같은 질문 인덱스 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Text_Index;

	/** "What is your current country..." 같은 질문 내용 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Text_Question;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImageButton> Button_Answer;

private:
	FWriteQuestionData QuestionData;
	
	UPROPERTY()
	TObjectPtr<class UTextureRenderTarget2D> RT_Canvas;
	
	UFUNCTION()
	void OnClickButton();
};
