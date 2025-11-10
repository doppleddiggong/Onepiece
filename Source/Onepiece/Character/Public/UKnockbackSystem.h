// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file UKnockbackSystem.h
 * @brief UKnockbackSystem 클래스를 선언합니다.
 */
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FKnockbackData.h"
#include "UKnockbackSystem.generated.h"


/**
 * @brief 피격 방향으로 캐릭터를 밀어내고 복구하는 넉백 시스템.
 *
 * 데미지 이벤트를 구독해 방향, 저항 값을 계산하고 이동 컴포넌트에 힘을 적용한다.
 * @ingroup Character
 */
UCLASS(Blueprintable, BlueprintType, ClassGroup=(Dopple), meta=(BlueprintSpawnableComponent) )
class ONEPIECE_API UKnockbackSystem : public UActorComponent
{
	GENERATED_BODY()

public:
	/** @brief 넉백 파라미터의 기본값을 준비한다. */
	UKnockbackSystem();

protected:
	/** @brief 소유자와 컴포넌트 레퍼런스를 캐시한다. */
	virtual void BeginPlay() override;
	/**
	 * @brief 종료 시점에 등록된 델리게이트를 해제한다.
	 * @param EndPlayReason 엔드플레이 사유.
	 */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	 
    /**
     * @brief 넉백 시스템을 초기화하고 이벤트를 바인딩한다.
     * @param InOwner 넉백을 적용할 게임 캐릭터.
     */
    UFUNCTION(BlueprintCallable, Category="Knockback")
    void InitSystem(class APlayerActor* InOwner);

    /**
     * @brief 넉백 이벤트를 처리하고 적절한 힘을 적용한다.
     * @param Target 넉백될 대상.
     * @param Instigator 공격 주체.
     * @param Type 데미지 타입.
     * @param Resistance 적용 저항 값.
     */
    UFUNCTION(BlueprintCallable, Category="Knockback")
    void OnKnockback(AActor* Target, AActor* Instigator, EDamageType Type, float Resistance);

private:
    /**
     * @brief 넉백 방향 벡터를 계산한다.
     * @param Target 타격 대상.
     * @param Instigator 공격 주체.
     * @return 정규화된 넉백 방향.
     */
    static FVector ComputeKnockDir(const AActor* Target, const AActor* Instigator);
    /**
     * @brief 계산된 파라미터를 바탕으로 넉백을 적용한다.
     * @param Target 넉백 대상.
     * @param Instigator 공격 주체.
     * @param Type 데미지 타입.
     * @param Resistance 저항 값.
     */
    void Knockback(AActor* Target, AActor* Instigator, EDamageType Type, float Resistance);
    /** @brief 넉백 후 이동 관련 속성을 복구한다. */
    void RestoreMovement();
    /** @brief 넉백 후 공중 상태에 진입했을 때 초기화를 수행한다. */
    void EnterFlying();

private:
	/** @brief 넉백 기능을 담당하는 소유 캐릭터. */
	UPROPERTY()
	TObjectPtr<class APlayerActor> Owner;
	/** @brief 넉백 시 애니메이션 조작을 위한 메시 컴포넌트. */
	UPROPERTY()
	TObjectPtr<class USkeletalMeshComponent> MeshComp;
	/** @brief 속도 및 마찰을 제어할 이동 컴포넌트. */
	UPROPERTY()
	TObjectPtr<class UCharacterMovementComponent> MoveComp;

	/** @brief 넉백 전 브레이킹 마찰 계수. */
	float PrevBrakingFriction = 1.f;
	/** @brief 마찰 계수를 수정했는지 여부. */
	bool  bFriction = false;
	/** @brief 이동 복구를 예약하는 타이머. */
	FTimerHandle RestoreTimer;
    /** @brief 비행 상태 복귀를 예약하는 타이머. */
    FTimerHandle FlyingTimer;
};
