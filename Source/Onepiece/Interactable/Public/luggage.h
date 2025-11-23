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
	// Sets default values for this actor's properties
	Aluggage();

public:
	// Components
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UStaticMeshComponent* Mesh;
	
	UPROPERTY(VisibleAnywhere)
	class UInteractableComponent* InteractableComp;
};
