// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file UFadeWidget.h
 * @brief 화면 페이드 인/아웃 효과를 제공하는 위젯
 */

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UFadeWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFadeComplete);

/**
 * @brief 화면 페이드 인/아웃 효과를 제공하는 위젯
 * @details 텔레포트 등의 화면 전환 연출에 사용됩니다.
 */
UCLASS()
class ONEPIECE_API UFadeWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	/**
	 * @brief 페이드 아웃 효과 시작 (화면이 어두워짐)
	 * @param Duration 페이드 아웃 지속 시간 (초)
	 */
	UFUNCTION(BlueprintCallable, Category = "Fade")
	void FadeOut(float Duration = 0.5f);

	/**
	 * @brief 페이드 인 효과 시작 (화면이 밝아짐)
	 * @param Duration 페이드 인 지속 시간 (초)
	 */
	UFUNCTION(BlueprintCallable, Category = "Fade")
	void FadeIn(float Duration = 0.5f);

	/**
	 * @brief 페이드 아웃 완료 시 호출되는 델리게이트
	 */
	UPROPERTY(BlueprintAssignable, Category = "Fade")
	FOnFadeComplete OnFadeOutComplete;

	/**
	 * @brief 페이드 인 완료 시 호출되는 델리게이트
	 */
	UPROPERTY(BlueprintAssignable, Category = "Fade")
	FOnFadeComplete OnFadeInComplete;

private:
	/**
	 * @brief 페이드 아웃 애니메이션 완료 콜백
	 */
	UFUNCTION()
	void OnFadeOutAnimationFinished();

	/**
	 * @brief 페이드 인 애니메이션 완료 콜백
	 */
	UFUNCTION()
	void OnFadeInAnimationFinished();

protected:
	/**
	 * @brief 페이드 아웃 애니메이션 (블루프린트에서 정의)
	 */
	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	TObjectPtr<class UWidgetAnimation> FadeOutAnim;

	/**
	 * @brief 페이드 인 애니메이션 (블루프린트에서 정의)
	 */
	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	TObjectPtr<class UWidgetAnimation> FadeInAnim;

	/**
	 * @brief 페이드 이미지 (검은색 전체 화면)
	 */
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
	TObjectPtr<class UImage> FadeImage;
};
