// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UTabIndicator.generated.h"

/**
 * @brief 탭 선택 인디케이터 위젯. 선택된 탭으로 부드럽게 이동하는 애니메이션을 제공합니다.
 */
UCLASS()
class ONEPIECE_API UTabIndicator : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeDestruct() override;
	
public:
	/// @brief 목표 위치로 이동합니다.
	/// @param [in] InTargetPosition 이동할 목표 위치 (부모 기준 상대 좌표)
	/// @param [in] InAnimate 애니메이션 여부 (false면 즉시 이동)
	UFUNCTION(BlueprintCallable, Category = "TabIndicator")
	void MoveTo(FVector2D InTargetPosition, bool InAnimate = true);

	/// @brief 애니메이션 속도를 설정합니다.
	/// @param [in] Speed 애니메이션 속도 (초 단위)
	UFUNCTION(BlueprintCallable, Category = "TabIndicator")
	void SetAnimationSpeed(float Speed);

	/// @brief 이동 완료 이벤트 델리게이트
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMoveCompleted);

	/// @brief 이동 애니메이션이 완료되었을 때 발생하는 이벤트
	UPROPERTY(BlueprintAssignable, Category = "TabIndicator|Events")
	FOnMoveCompleted OnMoveCompleted;

private:
	/// @brief 애니메이션을 위한 Tick 함수
	void TickAnimation();

	/// @brief 현재 위젯의 CanvasPanelSlot을 가져옵니다.
	class UCanvasPanelSlot* GetCanvasSlot() const;

private:
	/// @brief 애니메이션 속도 (초 단위)
	UPROPERTY(EditAnywhere, Category = "TabIndicator|Animation", meta = (ClampMin = "0.01", ClampMax = "2.0"))
	float AnimationSpeed = 0.15f;

	/// @brief 애니메이션을 위한 타이머 핸들
	FTimerHandle AnimTimerHandle;

	/// @brief 애니메이션 시작 위치
	FVector2D AnimStartPosition = FVector2D::ZeroVector;

	/// @brief 애니메이션 목표 위치
	FVector2D AnimTargetPosition = FVector2D::ZeroVector;

	/// @brief 애니메이션 진행 시간
	float AnimElapsedTime = 0.0f;

	/// @brief 애니메이션 진행 중 여부
	bool bIsAnimating = false;
};
