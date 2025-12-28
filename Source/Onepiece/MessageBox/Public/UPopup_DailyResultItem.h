// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UPopup_DailyResultItem.generated.h"

/**
 * @brief Daily Result 아이템 위젯
 * 
 * 각 단어별 학습 결과를 표시하는 아이템 위젯입니다.
 * 간단한 3개 파라미터(Question, FeedBack, Score)로 초기화됩니다.
 */
UCLASS()
class ONEPIECE_API UPopup_DailyResultItem : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * @brief 아이템 데이터로 위젯 초기화
	 * @param Question 질문 (한국어 단어)
	 * @param FeedBack 피드백 메시지
	 * @param Score 점수
	 */
	void InitData(const FString& Question, const FString& FeedBack, const int32 Score);

protected:
	/** 문제 번호 텍스트 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_Index;

	/** 질문 텍스트 (한국어 단어) */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_Question;

	/** 피드백 텍스트 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_Feedback;
	
	/** 점수 텍스트 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_Score;
};
