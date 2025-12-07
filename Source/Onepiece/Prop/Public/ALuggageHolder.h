// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ALuggageHolder.generated.h"

UCLASS()
class ONEPIECE_API ALuggageHolder : public AActor
{
	GENERATED_BODY()

public:
	ALuggageHolder();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:
	void SetAnswerData(const int32 InAnswerColorIdx, const int32 InAnswerPatternIdx);

	// Events
	UFUNCTION(BlueprintImplementableEvent, Category = "Holder")
	void OnActivate(bool bSuccess);
	
private:
	UFUNCTION()
	void OnRep_IsActivated();

	UFUNCTION()
	void OnRep_CurTarget();

	UFUNCTION()
	void OnBoxOverlapBegin(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	bool CheckLuggage(class Aluggage* TargetLuggage);

	void UpdateActivateState(bool State);
	
public:
	// Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<class UBoxComponent> BoxCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<class USkeletalMeshComponent> MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	TObjectPtr<class USceneComponent> HoldPos;

protected:
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
	int32 AnswerColorIdx = -1;
	int32 AnswerPatternIdx = -1;
};