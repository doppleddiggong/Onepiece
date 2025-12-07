// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Food.generated.h"

UCLASS()
class ONEPIECE_API AFood : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFood();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	// Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* FoodMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UWidgetComponent* FoodName;

protected:
	// 음식 인덱스
	FString Name = "";
	int32 Index = -1;

public:
	void SetFoodInfo(int32 InIndex, FString InName);
};
