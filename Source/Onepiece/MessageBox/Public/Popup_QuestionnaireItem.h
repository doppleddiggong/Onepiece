// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "NetworkData.h"
#include "Blueprint/UserWidget.h"
#include "Popup_QuestionnaireItem.generated.h"

/** 답변 상태 변경 델리게이트 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAnswerStateChanged, bool, bIsAnswered);

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

	/** 답변 상태 설정 */
	UFUNCTION(BlueprintCallable)
	void SetAnswerState(bool bAnswered);

	/** 답변 입력 여부 반환 */
	UFUNCTION(BlueprintCallable)
	bool IsAnswered() const { return bIsAnswered; }

	/** 답변 상태 변경 시 호출되는 델리게이트 */
	UPROPERTY(BlueprintAssignable)
	FOnAnswerStateChanged OnAnswerStateChanged;

protected:
	/* ----------------- Layout ----------------- */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UWidgetSwitcher> WidgetSwitcher;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Text_Index;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Image_Checker;

	/** "What is your current country..." 같은 질문 내용 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Text_Question;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImageButton> Button_Answer;

private:
	FWriteQuestionData QuestionData;

	UPROPERTY()
	TObjectPtr<class UTextureRenderTarget2D> RT_Canvas;

	/** 답변 입력 여부 */
	bool bIsAnswered = false;

	UFUNCTION()
	void OnClickButton();

	/** WriteBoard 저장 완료 콜백 */
	UFUNCTION()
	void OnWriteBoardSaved();

	/** WidgetSwitcher 상태 업데이트 */
	void UpdateWidgetSwitcher();
};
