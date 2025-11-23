// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file UTweenAnimInstance.h
 * @brief UTweenAnimInstance 클래스를 선언합니다.
 */
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "UTweenAnimInstance.generated.h"

/**
 * @brief 블루프린트에서 사용할 수 있는 간단한 트윈 애님 인스턴스.
 *
 * 애니메이션 블루프린트에서 보간 알파를 생성하고 커브를 통해 Ease 처리된 값을 제공한다.
 */
UCLASS()
class COFFEELIBRARY_API UTweenAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	/** @brief 트윈 진행 여부. true면 증가, false면 감소한다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tween")
	bool bActivate = false;

	/** @brief 0~1 알파를 1회 왕복하는 데 걸리는 시간(초). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tween", meta=(ClampMin="0.001"))
	float TweenDuration = 0.25f;

	/** @brief 선형 알파 값. 내부적으로 Tick 때마다 업데이트된다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Tween")
	float TweenAlpha = 0.f;

	/** @brief Ease 처리를 위한 커브. 없으면 선형 알파를 그대로 사용한다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tween")
	TObjectPtr<UCurveFloat> EaseCurve = nullptr;
public:
	/** @brief 애니메이션 갱신 시 트윈 알파를 갱신한다. */
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	/** @brief 트윈 활성화 상태를 수동으로 전환한다. */
	UFUNCTION(BlueprintCallable, Category="Tween")
	void ChangeState(bool bInActivate) { bActivate = bInActivate; }

	/** @brief 현재 알파를 지정 값으로 초기화한다. */
	UFUNCTION(BlueprintCallable, Category="Tween")
	void ResetAlpha(float InAlpha = 0.f) { TweenAlpha = FMath::Clamp(InAlpha, 0.f, 1.f); }

	/** @brief Ease 커브를 통과한 알파 값을 반환한다. */
	UFUNCTION(BlueprintPure, Category="Tween")
	float GetEasedAlpha() const;
};
