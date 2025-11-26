// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "EPopupType.h"
#include "Blueprint/UserWidget.h"
#include "UBasePopup.generated.h"

/**
 * @brief 모든 팝업의 기본 클래스
 *
 * 팝업 오픈 애니메이션 기능을 제공합니다.
 */
UCLASS()
class ONEPIECE_API UBasePopup : public UUserWidget
{
	GENERATED_BODY()

public:
	// ~ Begin UUserWidget Interface
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	// ~ End UUserWidget Interface

	/**
	 * @brief 팝업 오픈 애니메이션을 재생합니다
	 */
	UFUNCTION(BlueprintCallable, Category = "Popup|Animation")
	void OpenAnimation();

protected:
	EPopupType PopupType;

	// ========================================
	// Script Open Animation
	// ========================================

	/**
	 * @brief 스크립트 오픈 애니메이션 업데이트
	 */
	void UpdateAnimation(float InDeltaTime);

	/** 애니메이션 재생 중 여부 */
	bool bIsOpenAnim = false;

	/** 경과 시간 */
	float OpenElapsedTime = 0.0f;

	/** 애니메이션 지속 시간 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Popup|Animation")
	float OpenDuration = 0.1f;

	/** 시작 스케일 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Popup|Animation")
	float OpenStartScale = 0.8f;

	/** 목표 스케일 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Popup|Animation")
	float OpenTargetScale = 1.0f;

	/** Ease 지수 (값이 클수록 더 급격하게 가속) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Popup|Animation")
	float OpenEaseExponent = 2.0f;

	/** 애니메이션 중심점 (0~1 범위) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Popup|Animation")
	FVector2D OpenPivot = FVector2D(0.5f, 0.5f);
};
