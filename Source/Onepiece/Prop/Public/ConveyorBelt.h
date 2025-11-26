// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ConveyorBelt.generated.h"

UCLASS()
class ONEPIECE_API AConveyorBelt : public AActor
{
	GENERATED_BODY()

public:
	AConveyorBelt();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	
	// Movement Func
	void ChangeConveyorMovement();
	
protected:
	// Components
	UPROPERTY()
	TObjectPtr<class USceneComponent> RootSceneComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UStaticMeshComponent> BeltComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UBoxComponent> BeltBoxComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UArrowComponent> MoveDirArrowComp;
	
	// Move Values
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsForward = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MoveSpeed = 200.f;
	
	// Move Overlapped Actors
	void MoveOverlappedSkeletals(float deltaDistance);
	void MoveOverlappedStatics(float deltaDistance);
};
