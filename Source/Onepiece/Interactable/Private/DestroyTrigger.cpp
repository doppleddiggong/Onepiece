// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "DestroyTrigger.h"

#include "Food.h"
#include "Components/BoxComponent.h"


// Sets default values
ADestroyTrigger::ADestroyTrigger()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	SetRootComponent(Trigger);
}

// Called when the game starts or when spawned
void ADestroyTrigger::BeginPlay()
{
	Super::BeginPlay();

	Trigger->OnComponentBeginOverlap.AddDynamic(this, &ADestroyTrigger::BeginOverlap);
}

// Called every frame
void ADestroyTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADestroyTrigger::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AFood* Food = Cast<AFood>(OtherActor))
	{
		Server_Destroy(Food);
	}
}

void ADestroyTrigger::Server_Destroy_Implementation(AActor* ActorToDestroy)
{
	ActorToDestroy->Destroy();
}

