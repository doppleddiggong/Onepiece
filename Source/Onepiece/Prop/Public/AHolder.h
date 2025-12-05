// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AHolder.generated.h"

UCLASS()
class ONEPIECE_API AHolder : public AActor
{
	GENERATED_BODY()

public:
	AHolder();

protected:
	virtual void BeginPlay() override;

public:
	// Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<class UBoxComponent> BoxCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<class USkeletalMeshComponent> MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	TObjectPtr<class USceneComponent> HoldPos;

	// Answer Settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Answer")
	int32 AnswerColorIdx = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Answer")
	int32 AnswerPatternIdx = -1;

	// Animation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TObjectPtr<class UAnimationAsset> AnimToPlay;

	// State
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsActivated = false;

	// Events
	UFUNCTION(BlueprintImplementableEvent, Category = "Holder")
	void OnActivate(bool bSuccess);

private:
	UFUNCTION()
	void OnBoxOverlapBegin(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	void CheckLuggage(class Aluggage* Luggage);

	// 현재 올라가 있는 Luggage 추적
	UPROPERTY()
	TObjectPtr<class Aluggage> CurrentLuggage;
};
