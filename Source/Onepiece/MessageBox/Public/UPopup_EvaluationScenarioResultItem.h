// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NetworkData.h"
#include "UPopup_EvaluationScenarioResultItem.generated.h"

/**
 * @brief Evaluation 시나리오별 결과를 표시하는 패널 위젯
 */
UCLASS()
class ONEPIECE_API UPopup_EvaluationScenarioResultItem : public UUserWidget
{
	GENERATED_BODY()

public:
	/// @brief 시나리오 결과 데이터로 패널 초기화
	/// @param [in] InScenarioResult 시나리오 결과 데이터
	UFUNCTION(BlueprintCallable, Category = "Evaluation")
	void InitPanel(const FScenarioResult& InScenarioResult);

protected:
	/// @brief 영역 이름 텍스트 (예: "읽기 영역")
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UTextBlock> Txt_DisplayName;

	/// @brief 최종 점수 텍스트
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UTextBlock> Txt_FinalScore;

	/// @brief 등급 텍스트
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UTextBlock> Txt_Grade;

	/// @brief 피드백 제목 텍스트
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UTextBlock> Txt_FeedbackTitle;

	/// @brief 피드백 메시지 텍스트
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UTextBlock> Txt_FeedbackMessage;

	/// @brief 액션 아이템 텍스트
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UTextBlock> Txt_ActionItem;
};
