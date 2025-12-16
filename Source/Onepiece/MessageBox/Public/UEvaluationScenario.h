// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NetworkData.h"
#include "UEvaluationScenario.generated.h"

/**
 * @brief Evaluation 시나리오별 결과를 표시하는 패널 위젯
 */
UCLASS()
class ONEPIECE_API UEvaluationScenario : public UUserWidget
{
	GENERATED_BODY()

public:
	/// @brief 시나리오 결과 데이터로 패널 초기화
	/// @param [in] InScenarioResult 시나리오 결과 데이터
	UFUNCTION(BlueprintCallable, Category = "Evaluation")
	void InitPanel(const FScenarioResult& InScenarioResult);

protected:
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UImage> Image_Symbol;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UTextBlock> Txt_Title;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UTextBlock> Txt_Desc;
	
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UImage> Image_Grade;
};
