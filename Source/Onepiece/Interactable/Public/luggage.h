// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "luggage.generated.h"

UCLASS()
class ONEPIECE_API Aluggage : public AActor
{
	GENERATED_BODY()

public:
	Aluggage();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<class UStaticMeshComponent> Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<class UStaticMeshComponent> Mesh1Comp;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UInteractableComponent> InteractableComp;
	
public:
	// Color
	UPROPERTY(ReplicatedUsing=OnRep_ColorIndex)
	int32 ColorIndex = -1;

	UFUNCTION()
	void OnRep_ColorIndex();

	void ApplyColorToMesh(int32 InColorIdx);

	// Pattern
	UPROPERTY()
	FString PatternName;

	void ApplyPatternToMesh(FString InPattern);
};
