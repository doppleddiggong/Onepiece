// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UPopup_InterviewItem.generated.h"

/**
 * 
 */
UCLASS()
class ONEPIECE_API UPopup_InterviewItem : public UUserWidget
{
	GENERATED_BODY()

public:
	/** 질문 항목 초기화 */
	UFUNCTION(BlueprintCallable)
	void InitItem(const FInterviewQuestionData& Data);

	/** 플레이어가 입력한 답변 반환 */
	UFUNCTION(BlueprintCallable)
	FString GetAnswer() const;

protected:
	virtual void NativeConstruct() override;

protected:
	/* ----------------- Layout ----------------- */
	/** "Question.01" 같은 질문 인덱스 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_Index;

	/** "What is your current country..." 같은 질문 내용 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Text_Question;

	/** 답변 입력란 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UMultiLineEditableText> Edit_Answer;
};


