// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FoodHolder.generated.h"

UCLASS()
class ONEPIECE_API AFoodHolder : public AActor
{
	GENERATED_BODY()

public:
	AFoodHolder();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:
	// Events
	UFUNCTION(BlueprintImplementableEvent, Category = "Holder")
	void OnActivate(bool bSuccess);

	/**
	 * @brief [Multicast RPC] 모든 클라이언트에 정답 결과 팝업 표시
	 * @param CorrectAnswerIndex 정답 인덱스
	 */
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ShowResultPopup(int32 CorrectAnswerIndex);

	/**
	 * @brief [Multicast RPC] 모든 클라이언트에 오답 메시지 표시
	 * @param FoodName 선택한 Food 이름
	 */
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ShowWrongPopup(const FString& FoodName);

	/**
	 * @brief 정답 Food 인덱스 설정
	 * @param InAnswerFoodIndex 정답 Food의 인덱스
	 */
	void SetAnswerFoodIndex(int32 InAnswerFoodIndex);

private:
	UFUNCTION()
	void OnRep_IsActivated();

	UFUNCTION()
	void OnRep_CurTarget();

	/**
	 * @brief BoxCollision Overlap 콜백
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

	void UpdateActivateState(bool State);

public:
	// Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<class UBoxComponent> BoxCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<class USkeletalMeshComponent> MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	TObjectPtr<class USceneComponent> HoldPos;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Index = 0;

protected:
	// 시도 횟수
	UPROPERTY(Replicated)
	int32 TryIdx = -1;
	
	// 현재 올라가 있는 액터
	UPROPERTY(ReplicatedUsing=OnRep_CurTarget)
	TObjectPtr<class AActor> CurTarget;

	// State
	UPROPERTY(ReplicatedUsing=OnRep_IsActivated, VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsActivated = false;

	// Visual Settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	float ActivatedHeightOffset = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	float RotationSpeed = 90.0f;
	
	// Answer Settings
	/** 정답 Food 인덱스 (-1이면 모든 Food 허용) */
	int32 AnswerFoodIndex = -1;
	
};
