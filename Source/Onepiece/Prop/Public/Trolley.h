// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Trolley.generated.h"

UCLASS()
class ONEPIECE_API ATrolley : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATrolley();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	// Components

	/** 수레 메시 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UStaticMeshComponent> MeshComp;

	/** 상호작용 컴포넌트 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UInteractableComponent> InteractableComp;

	// ========================================
	// Movement Properties
	// ========================================

	/** 밀었을 때 추가되는 힘 (cm/s) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float PushForce = 500.0f;

	/** 감속도 (cm/s²) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float Deceleration = 300.0f;

	/** 정지 판정 속도 임계값 (cm/s) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MinVelocityThreshold = 5.0f;

	/** 충돌 시 추가되는 힘의 배율 (PushForce 대비) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float CollisionPushMultiplier = 0.5f;

private:
	/** 현재 속도 벡터 */
	FVector CurrentVelocity;

	/**
	 * @brief 플레이어가 수레를 밀었을 때 호출
	 * @param Interactor 상호작용한 액터 (플레이어)
	 */
	UFUNCTION()
	void OnPushed(AActor* Interactor);

	/**
	 * @brief 수레 메시에 충돌이 발생했을 때 호출
	 * @param HitComponent 충돌한 컴포넌트
	 * @param OtherActor 충돌한 다른 액터
	 * @param OtherComp 충돌한 다른 컴포넌트
	 * @param NormalImpulse 충돌 임펄스
	 * @param Hit 충돌 정보
	 */
	UFUNCTION()
	void OnTrolleyHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
