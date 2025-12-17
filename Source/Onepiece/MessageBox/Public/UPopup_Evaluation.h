// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "UBasePopup.h"
#include "NetworkData.h"
#include "UPopup_Evaluation.generated.h"

/**
 * @brief Evaluation 결과를 표시하는 팝업
 *
 * TabButtonGroup과 WidgetSwitcher를 사용하여 5개의 탭을 제공합니다:
 * - 0: Total Result (전체 결과)
 * - 1: Reading (읽기 영역)
 * - 2: Listening (듣기 영역)
 * - 3: Writing (쓰기 영역)
 * - 4: Speaking (말하기 영역)
 */
UCLASS()
class ONEPIECE_API UPopup_Evaluation : public UBasePopup
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	/// @brief 팝업 초기화 (FResponseEvaluationResultDelegate에서 받은 데이터 기반)
	/// @param [in] InEvaluationResult Evaluation 결과 데이터
	UFUNCTION(BlueprintCallable, Category = "Evaluation")
	void InitPopup(const FResponseEvaluationResult& InEvaluationResult);

private:
	/// @brief 닫기 버튼 클릭 핸들러
	UFUNCTION(BlueprintCallable, Category = "Evaluation")
	void OnClickClose();

protected:
	/// @brief 닫기 버튼
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UImageButton> Btn_Confirm;

	/// @brief 전체 결과 패널 (탭 0)
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UEvaluationTotal> Panel_TotalResult;

	/// @brief Reading 결과 패널 (탭 1)
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UEvaluationScenario> Panel_Reading;

	/// @brief Listening 결과 패널 (탭 2)
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UEvaluationScenario> Panel_Listening;

	/// @brief Writing 결과 패널 (탭 3)
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UEvaluationScenario> Panel_Writing;

	/// @brief Speaking 결과 패널 (탭 4)
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UEvaluationScenario> Panel_Speaking;
};