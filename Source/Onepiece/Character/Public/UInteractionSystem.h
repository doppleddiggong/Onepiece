// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file UInteractionSystem.h
 * @brief 플레이어의 상호작용 감지 및 처리 시스템
 */

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UInteractionSystem.generated.h"

/**
 * @brief 플레이어의 상호작용 감지 및 처리 시스템
 * @details Overlap 등록 + LineTrace 타겟팅 방식 사용
 * @ingroup Character
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ONEPIECE_API UInteractionSystem : public UActorComponent
{
	GENERATED_BODY()

public:
	UInteractionSystem();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

public:
	// ========================================
	// 설정
	// ========================================

	/** LineTrace 최대 거리 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float InteractionDistance = 1200.0f;

	/** 디버그 표시 여부 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bShowDebugInfo = true;

	// ========================================
	// 상태
	// ========================================

	/** 현재 타겟팅 중인 객체 */
	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	TObjectPtr<class UInteractableComponent> CurrentTarget;

	/** 현재 들고 있는 객체 */
	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	TObjectPtr<class UInteractableComponent> HoldingInteractable;

	// ========================================
	// 공개 인터페이스
	// ========================================

	/** 상호작용 시도 (타입별 자동 처리) */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void TryInteract();

	/** PickUp 시도 (레거시 호환) */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void TryPickUp();

	/** Drop 시도 */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void TryDrop();

	/** InteractableComponent 등록 (Overlap 콜백용) */
	void RegisterInteractable(class UInteractableComponent* Interactable);

	/** InteractableComponent 해제 */
	void UnregisterInteractable(class UInteractableComponent* Interactable);

	/** 플레이어가 물건을 들고 있는 위치 */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	USceneComponent* GetHoldPosition() const;

protected:
	// ========================================
	// 감지 시스템
	// ========================================

	/** 화면 중앙 LineTrace로 타겟 감지 */
	UInteractableComponent* DetectInteractableTarget();

	/** LineTrace 실행 */
	bool PerformCenterLineTrace(FHitResult& OutHit);

private:
	// ========================================
	// 내부 데이터
	// ========================================

	/** Overlap으로 등록된 근처 상호작용 객체들 */
	UPROPERTY()
	TArray<TObjectPtr<class UInteractableComponent>> NearbyInteractables;

	/** Owner PlayerActor 캐싱 */
	UPROPERTY()
	TObjectPtr<class AOwlPlayer> OwnerPlayer;
};
