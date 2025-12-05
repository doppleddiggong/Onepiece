// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ADropper.generated.h"

UCLASS()
class ONEPIECE_API ADropper : public AActor
{
	GENERATED_BODY()

public:
	ADropper();

	UFUNCTION(BlueprintCallable, Category = "Spawning")
	class AActor* SpawnActor( TSubclassOf<class AActor> SpawnActorClass ); 
	
private:
	UFUNCTION()
	void PlayAnimationAndDisableCollision();

	UFUNCTION()
	void RestoreCollision();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	TObjectPtr<class USceneComponent> SpawnPos;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<class USkeletalMeshComponent> SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<class UStaticMeshComponent> BoxCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TObjectPtr<class UAnimationAsset> AnimToPlay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timing")
	float Delay = 2.0f;

private:
	FTimerHandle DelayTimerHandle;
	FTimerHandle RestoreTimerHandle;
};