// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "AHolder.h"
#include "FoodHolder.generated.h"

UCLASS()
class ONEPIECE_API AFoodHolder : public AHolder
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFoodHolder();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	/**
	 * @brief 정답 Food 인덱스 설정
	 * @param InAnswerFoodIndex 정답 Food의 인덱스
	 */
	void SetAnswerFoodIndex(int32 InAnswerFoodIndex);

private:
	/**
	 * @brief BoxCollision Overlap 콜백 (AHolder의 OnBoxOverlapBegin 오버라이드)
	 */
	UFUNCTION()
	void OnFoodBoxOverlapBegin(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	/**
	 * @brief Food 검증 함수
	 * @param TargetFood 검증할 Food 액터
	 * @return 정답 여부
	 */
	bool CheckFood(class AFood* TargetFood);

protected:
	// Answer Settings
	/** 정답 Food 인덱스 (-1이면 모든 Food 허용) */
	int32 AnswerFoodIndex = -1;
};
