// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "Trolley.h"

#include "InteractableComponent.h"
#include "GameFramework/Character.h"


// Sets default values
ATrolley::ATrolley()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create root mesh component
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;
	MeshComp->SetSimulatePhysics(false); // Custom velocity control, no physics simulation
	MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComp->SetCollisionProfileName(TEXT("BlockAllDynamic")); // Block physics objects like Food
	MeshComp->SetMobility(EComponentMobility::Movable); // Must be movable to change location at runtime
	MeshComp->SetNotifyRigidBodyCollision(true); // Enable hit events for OnComponentHit

	// Create interactable component
	InteractableComp = CreateDefaultSubobject<UInteractableComponent>(TEXT("InteractableComp"));
	InteractableComp->InteractionType = EInteractionType::Button; // Use Button type for pushing
	InteractableComp->InteractionPrompt = TEXT("Press E to Push");

	// Initialize velocity
	CurrentVelocity = FVector::ZeroVector;
}

// Called when the game starts or when spawned
void ATrolley::BeginPlay()
{
	Super::BeginPlay();

	// Bind interaction delegate
	if (InteractableComp)
	{
		InteractableComp->OnInteractionTriggered.AddDynamic(this, &ATrolley::OnPushed);
	}

	// Bind collision delegate
	if (MeshComp)
	{
		MeshComp->OnComponentHit.AddDynamic(this, &ATrolley::OnTrolleyHit);
	}
}

// Called every frame
void ATrolley::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Check if moving
	if (CurrentVelocity.SizeSquared() > MinVelocityThreshold * MinVelocityThreshold)
	{
		// Keep velocity parallel to ground (ignore Z axis)
		FVector HorizontalVelocity = CurrentVelocity;
		HorizontalVelocity.Z = 0.0f;

		// Calculate movement delta
		FVector DeltaMovement = HorizontalVelocity * DeltaTime;

		// Move the trolley
		FVector OldLocation = GetActorLocation();
		FVector NewLocation = OldLocation + DeltaMovement;
		SetActorLocation(NewLocation, true); // true = sweep for collision

		// Apply deceleration
		FVector DecelerationVector = HorizontalVelocity.GetSafeNormal() * Deceleration * DeltaTime;
		CurrentVelocity -= DecelerationVector;

		// Stop if velocity is too small
		if (CurrentVelocity.SizeSquared() < MinVelocityThreshold * MinVelocityThreshold)
		{
			CurrentVelocity = FVector::ZeroVector;
		}
	}
}

void ATrolley::OnPushed(AActor* Interactor)
{
	if (!Interactor) return;

	// Get push direction from interactor's forward vector
	FVector PushDirection = Interactor->GetActorForwardVector();
	PushDirection.Z = 0.0f; // Keep it parallel to ground
	PushDirection.Normalize();

	// Add velocity in push direction
	CurrentVelocity += PushDirection * PushForce;
}

void ATrolley::OnTrolleyHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor)
		return;

	// Check if the other actor is a player character
	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (!Character || !Character->IsPlayerControlled())
		return;

	// Calculate push direction (from player to trolley)
	FVector PushDirection = GetActorLocation() - OtherActor->GetActorLocation();
	PushDirection.Z = 0.0f; // Keep it parallel to ground
	PushDirection.Normalize();

	// Add velocity in push direction with reduced force
	CurrentVelocity += PushDirection * PushForce * CollisionPushMultiplier;
}
