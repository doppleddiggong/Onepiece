// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "EPopupType.h"
#include "Blueprint/UserWidget.h"
#include "FEaseHelper.h"
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
	// Blueprint Open Animation
	// ========================================

	/**
	 * @brief 블루프린트에서 정의한 오픈 애니메이션
	 * @details 설정된 경우 스크립트 애니메이션 대신 이 애니메이션을 재생합니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Popup|Animation", meta = (BindWidgetOptional))
	UWidgetAnimation* BlueprintOpenAnimation;

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
	float OpenDuration = 0.2f;

	/** 시작 스케일 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Popup|Animation")
	float OpenStartScale = 0.8f;

	/** 목표 스케일 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Popup|Animation")
	float OpenTargetScale = 1.0f;

	/** 애니메이션 중심점 (0~1 범위) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Popup|Animation")
	FVector2D OpenPivot = FVector2D(0.5f, 0.5f);

	/**
	 * @brief Easing 타입
	 * @details 애니메이션의 보간 곡선을 선택합니다. 기본값은 EaseOutBack (탄성 효과)
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Popup|Animation")
	EEaseType OpenEaseType = EEaseType::EaseOutBack;

public:
	/**
	 * @brief 이 팝업이 활성화되어 있을 때 플레이어 조작을 허용할지 여부
	 * @details true면 이 팝업이 열려있어도 플레이어가 캐릭터를 조작할 수 있습니다.
	 *          기본값은 false (대부분의 팝업은 플레이어 조작을 차단)
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Popup|Control")
	bool bAllowPlayerControl = false;
};
