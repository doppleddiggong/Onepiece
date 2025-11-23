// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "Onepiece/Interactable/Public/luggage.h"

#include "Onepiece/Interactable/Public/InteractableComponent.h"


// Sets default values
Aluggage::Aluggage()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	InteractableComp = CreateDefaultSubobject<UInteractableComponent>(TEXT("Interactable"));

	// Initial settings
	Mesh->SetSimulatePhysics(true);
	Mesh->SetEnableGravity(true);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetCollisionProfileName(TEXT("PhysicsActor"));

	// 무게 설정
	Mesh->SetMassOverrideInKg(NAME_None, 5.f, true);

	// Replication
	bReplicates = true;
	SetReplicateMovement(true);
}