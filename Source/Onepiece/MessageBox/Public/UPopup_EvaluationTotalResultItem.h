// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NetworkData.h"
#include "UPopup_EvaluationTotalResultItem.generated.h"

/**
 * @brief Evaluation 전체 결과를 표시하는 패널 위젯
 */
UCLASS()
class ONEPIECE_API UPopup_EvaluationTotalResultItem : public UUserWidget
{
	GENERATED_BODY()

public:
	/// @brief 전체 결과 데이터로 패널 초기화
	/// @param [in] InTotalResult 전체 결과 데이터
	UFUNCTION(BlueprintCallable, Category = "Evaluation")
	void InitPanel(const FTotalResult& InTotalResult);

protected:
	/// @brief 최종 점수 텍스트
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UTextBlock> Txt_FinalScore;

	/// @brief 등급 텍스트
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UTextBlock> Txt_Grade;

	/// @brief 피드백 요약 텍스트
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UTextBlock> Txt_FeedbackSummary;
};
