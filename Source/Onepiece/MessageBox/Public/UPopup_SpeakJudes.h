// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "UBasePopup.h"
#include "Blueprint/UserWidget.h"
#include "UPopup_SpeakJudes.generated.h"

/// @brief SpeakJudes 팝업 확인 버튼 클릭 시 호출되는 델리게이트
DECLARE_DELEGATE(FOnSpeakJudesConfirmDelegate);

/**
 * @brief Speaking Judges 결과를 표시하는 팝업
 *
 * FResponseSpeakingJudes 데이터를 받아 점수와 피드백을 표시합니다.
 */
UCLASS()
class ONEPIECE_API UPopup_SpeakJudes : public UBasePopup
{
	GENERATED_BODY()

public:
	/**
	 * @brief 팝업을 초기화하고 데이터를 설정합니다
	 * @param Response SpeakingJudes 응답 데이터
	 * @param InOnConfirm 확인 버튼 클릭 시 호출될 델리게이트
	 */
	void InitPopup(const struct FResponseSpeakingJudes& Response, const FOnSpeakJudesConfirmDelegate& InOnConfirm = FOnSpeakJudesConfirmDelegate());

	/// @brief 확인 버튼 클릭 시 호출
	UFUNCTION()
	void OnClickClose();

protected:
	/// @brief Grammar Score 텍스트
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UResultStatWidget> Result_Grammer;

	/// @brief Context Score 텍스트
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UResultStatWidget> Result_Context;

	/// @brief Overall Score 텍스트
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UResultStatWidget> Result_Overall;

	/// @brief Feedback 텍스트
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_Feedback;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextureButton> Btn_Close;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImageButton> Btn_Confirm;

private:
	/// @brief 확인 버튼 클릭 시 호출될 델리게이트
	FOnSpeakJudesConfirmDelegate OnConfirmDelegate;
};