// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MiniOwlBot.generated.h"

UCLASS()
class ONEPIECE_API AMiniOwlBot : public APawn
{
	GENERATED_BODY()

public:
	AMiniOwlBot();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	
private:
	// owlbot position values
	FVector targetLocation = FVector(150, 50, 50);
	float radius = 200.f;
	float angle = 25.f;
	float baseHeight;
	float targetHeight = 50.f;
	float amplitude = 10.f;
	float frequency = 0.25f;
	float speed = 20.f;
	float time = 0.f;
	
	// target array
	UPROPERTY()
	TArray<class AActor*> targets;
	
	UPROPERTY()
	TObjectPtr<class APlayerActor> currTarget;
	
	void UpdateRotation();
	void UpdateLocation();
	bool CheckAngleOutofCamera();
};
