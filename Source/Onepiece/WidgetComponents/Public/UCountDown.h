// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UCountDown.generated.h"

/**
 * @brief 카운트다운 종료 시 호출되는 Delegate
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCountDownFinished);

/**
 * @brief 카운트다운 위젯
 *
 * 지정된 숫자부터 0까지 카운트다운하며 애니메이션을 표시합니다.
 * 카운트다운이 끝나면 EndCallback을 통해 알립니다.
 *
 * @details
 * - StartValue부터 1까지 1초 간격으로 표시
 * - 각 숫자 표시 시 애니메이션 재생 (블루프린트에서 설정)
 * - 0에 도달하면 위젯을 숨기고 EndCallback 호출
 */
UCLASS()
class ONEPIECE_API UCountDown : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * @brief 카운트다운을 시작합니다.
	 * @param [in] InStartValue 시작 숫자 (예: 3)
	 */
	UFUNCTION(BlueprintCallable, Category = "CountDown")
	void StartCountDown(int32 InStartValue);

	/**
	 * @brief 카운트다운 종료 시 호출되는 Delegate
	 */
	UPROPERTY(BlueprintAssignable, Category = "CountDown")
	FOnCountDownFinished OnCountDownFinished;

protected:
	virtual void NativeDestruct() override;

private:
	/**
	 * @brief 카운트다운 업데이트 (타이머 콜백)
	 */
	void UpdateCountDown();

	/**
	 * @brief 현재 숫자를 표시하고 애니메이션을 재생합니다.
	 */
	void DisplayCurrentNumber();

protected:
	// ===================================================================
	// UI Widgets (BindWidget)
	// ===================================================================

	/// @brief 카운트다운 컨테이너
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<class UBorder> Border_CountDown;

	/// @brief 카운트다운 텍스트
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_CountDown;

	// ===================================================================
	// Animations (블루프린트에서 설정)
	// ===================================================================

	/// @brief 숫자 표시 애니메이션 (블루프린트에서 설정)
	UPROPERTY(BlueprintReadWrite, Transient, meta = (BindWidgetAnimOptional))
	TObjectPtr<class UWidgetAnimation> Anim_CountDown;

private:
	// ===================================================================
	// Member Variables
	// ===================================================================

	/// @brief 현재 카운트다운 값
	int32 CountDownValue = 0;

	/// @brief 카운트다운 타이머
	FTimerHandle CountDownTimerHandle;
};
