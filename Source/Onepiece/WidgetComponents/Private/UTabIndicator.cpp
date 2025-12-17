// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "UTabIndicator.h"
#include "Components/CanvasPanelSlot.h"
#include "TimerManager.h"
#include "Components/PanelWidget.h"

void UTabIndicator::NativeDestruct()
{
	Super::NativeDestruct();

	// 타이머 정리
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(AnimTimerHandle);
	}
}

void UTabIndicator::MoveTo(FVector2D InTargetPosition, bool InAnimate)
{
	UCanvasPanelSlot* CanvasSlot = GetCanvasSlot();
	if (!CanvasSlot)
		return;

	// 애니메이션 없이 즉시 이동
	if (!InAnimate || AnimationSpeed <= 0.0f)
	{
		CanvasSlot->SetPosition(InTargetPosition);
		bIsAnimating = false;

		// 이동 완료 이벤트 발생
		OnMoveCompleted.Broadcast();
		return;
	}

	// 애니메이션으로 이동
	AnimStartPosition = CanvasSlot->GetPosition();
	AnimTargetPosition = InTargetPosition;
	AnimElapsedTime = 0.0f;
	bIsAnimating = true;

	// 타이머 시작 (기존 타이머가 있다면 제거)
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(AnimTimerHandle);
		GetWorld()->GetTimerManager().SetTimer(
			AnimTimerHandle,
			this,
			&UTabIndicator::TickAnimation,
			0.016f, // ~60 FPS
			true
		);
	}
}

void UTabIndicator::SetAnimationSpeed(float Speed)
{
	AnimationSpeed = FMath::Max(0.01f, Speed);
}

void UTabIndicator::TickAnimation()
{
	// Canvas Slot 유효성 검사
	UCanvasPanelSlot* CanvasSlot = GetCanvasSlot();
	if (!CanvasSlot)
	{
		if (GetWorld())
			GetWorld()->GetTimerManager().ClearTimer(AnimTimerHandle);
		
		bIsAnimating = false;
		return;
	}

	// 애니메이션 진행
	AnimElapsedTime += 0.016f; // ~60 FPS 기준
	const float Alpha = FMath::Clamp(AnimElapsedTime / AnimationSpeed, 0.0f, 1.0f);

	// Lerp를 사용한 부드러운 이동
	FVector2D CurPosition = FMath::Lerp(AnimStartPosition, AnimTargetPosition, Alpha);
	CanvasSlot->SetPosition(CurPosition);

	// 애니메이션 완료 확인
	if (Alpha >= 1.0f)
	{
		// 타이머 정지
		if (GetWorld())
		{
			GetWorld()->GetTimerManager().ClearTimer(AnimTimerHandle);
		}

		bIsAnimating = false;

		// 이동 완료 이벤트 발생
		OnMoveCompleted.Broadcast();
	}
}

UCanvasPanelSlot* UTabIndicator::GetCanvasSlot() const
{
	return Cast<UCanvasPanelSlot>(Slot);
}
