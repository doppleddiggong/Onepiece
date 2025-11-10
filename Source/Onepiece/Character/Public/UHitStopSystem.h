// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file UHitStopSystem.h
 * @brief UHitStopSystem 클래스를 선언합니다.
 */
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FHitStopData.h"
#include "UHitStopSystem.generated.h"

/**
 * @brief 피격 시 커스텀 타임 딜레이션을 적용해 타격감을 강화하는 시스템.
 *
 * 소유 캐릭터의 피격 이벤트를 감지하여 일정 시간 동안 움직임과 애니메이션을 정지시킨다.
 * @ingroup Character
 */
UCLASS(Blueprintable, BlueprintType, ClassGroup=(Dopple),  meta=(BlueprintSpawnableComponent) )
class ONEPIECE_API UHitStopSystem : public UActorComponent
{
	GENERATED_BODY()

public:
	/** @brief 시스템 기본 파라미터를 초기화한다. */
	UHitStopSystem();

public:
	/** @brief 소유자 레퍼런스를 캐시하고 이벤트를 바인딩한다. */
	virtual void BeginPlay() override;
	/**
	 * @brief 틱마다 활성 상태를 확인하고 종료 타이밍을 관리한다.
	 * @param DeltaTime 틱 간격(초).
	 * @param TickType 틱 호출 유형.
	 * @param ThisTickFunction 내부 틱 함수 포인터.
	 */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	/**
	 * @brief 시스템을 초기화하고 소유 캐릭터를 저장한다.
	 * @param InOwner 피격 이벤트를 전달할 게임 캐릭터.
	 */
	UFUNCTION(BlueprintCallable, Category="HitStop")
	void InitSystem(ACharacter* InOwner);

	/**
	 * @brief 피격 이벤트를 수신해 적절한 히트스톱을 적용한다.
	 * @param Target 피격된 대상 액터.
	 * @param Type 데미지 타입.
	 */
	UFUNCTION(BlueprintCallable, Category="HitStop")
	void OnHitStop(AActor* Target, const EDamageType Type);
	
	/**
	 * @brief 지정된 데미지 타입에 대응하는 히트스톱을 적용한다.
	 * @param Type 플레이 중 재생할 히트스톱 타입.
	 */
	UFUNCTION(BlueprintCallable, Category="HitStop")
	void ApplyHitStop(const EDamageType Type);
	
private:
	/**
	 * @brief 히트스톱 동안 캐릭터 움직임과 애니메이션을 고정한다.
	 * @param Params 적용할 히트스톱 파라미터.
	 */
	void BeginFreeze(const FHitStopData& Params);
	/** @brief 진행 중인 히트스톱을 종료하고 원상 복구한다. */
	void EndFreeze();

private:
	/** @brief 소유 게임 캐릭터 레퍼런스. */
	UPROPERTY()
	TObjectPtr<class ACharacter> Owner;
	/** @brief 타임 딜레이션을 적용할 스켈레탈 메시 컴포넌트. */
	UPROPERTY()
	TObjectPtr<class USkeletalMeshComponent> MeshComp;
	/** @brief 이동 속도를 제어하기 위한 캐릭터 무브먼트 컴포넌트. */
	UPROPERTY()
	TObjectPtr<class UCharacterMovementComponent> MoveComp;

	/** @brief 마지막으로 적용된 히트스톱 파라미터. */
	FHitStopData LastParams;
	
	/** @brief 현재 히트스톱이 활성 상태인지 여부. */
	bool bActive = false;
	/** @brief 히트스톱이 종료될 실시간(초). */
	double EndRealTimeSeconds = 0.0;

	/** @brief 히트스톱 전 캐릭터의 커스텀 타임 딜레이션 값. */
	float SavedCustomTimeDilation = 1.0f;
};
