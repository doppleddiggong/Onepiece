// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "ConveyorBelt.h"

#include "ANPCBase.h"
#include "APlayerActor.h"
#include "GameLogging.h"
#include "luggage.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"


// Sets default values
AConveyorBelt::AConveyorBelt()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	RootSceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComp"));
	SetRootComponent(RootSceneComp);
	
	BeltComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BeltComp"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> beltMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/LevelPrototyping/Meshes/SM_ChamferCube.SM_ChamferCube'"));
	if (beltMeshRef.Succeeded())
	{
		BeltComp->SetStaticMesh(beltMeshRef.Object);
		BeltComp->SetRelativeScale3D(FVector(1.5,5.5,0.05));
		BeltComp->SetupAttachment(GetRootComponent());
	}
	
	BeltBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BeltBoxComp"));
	BeltBoxComp->SetupAttachment(GetRootComponent());
	BeltBoxComp->SetBoxExtent(FVector(75,275,5));
	
	MoveDirArrowComp = CreateDefaultSubobject<UArrowComponent>(TEXT("MoveDirArrowComp"));
	MoveDirArrowComp->SetupAttachment(GetRootComponent());
	MoveDirArrowComp->SetRelativeRotation(FRotator(0, 90, 0));
	
	// Set Collision Preset
	BeltComp->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	BeltBoxComp->SetCollisionProfileName(TEXT("ConveyorBelt"));
}

// Called when the game starts or when spawned
void AConveyorBelt::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AConveyorBelt::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	MoveOverlappedSkeletals(MoveSpeed * DeltaTime);
	MoveOverlappedStatics(MoveSpeed * DeltaTime);
}

void AConveyorBelt::ChangeConveyorMovement()
{
	ServerRPC_ChangeConveyorMovement();
}

void AConveyorBelt::ServerRPC_ChangeConveyorMovement_Implementation()
{
	MulticastRPC_ChangeConveyorMovement();
}

void AConveyorBelt::MulticastRPC_ChangeConveyorMovement_Implementation()
{
	bIsForward = !bIsForward;
	PRINT_STRING(TEXT("%d"), bIsForward);
}

void AConveyorBelt::MoveOverlappedSkeletals(float deltaDistance)
{
	TArray<AActor*> overlappedActors;
	BeltBoxComp->GetOverlappingActors(overlappedActors);
	for (const auto& actor : overlappedActors)
	{
		if (Cast<APlayerActor>(actor) || Cast<ANPCBase>(actor))
		{
			FVector deltaLoc = MoveDirArrowComp->GetForwardVector() * deltaDistance;
			deltaLoc = bIsForward ? deltaLoc : -deltaLoc;
			actor->AddActorWorldOffset(deltaLoc);
		}
	}
}

void AConveyorBelt::MoveOverlappedStatics(float deltaDistance)
{
	TArray<UPrimitiveComponent*> overlappedComponents;
	BeltBoxComp->GetOverlappingComponents(overlappedComponents);
	for (const auto& comp : overlappedComponents)
	{
		AActor* owner = comp->GetOwner();
		if (Cast<Aluggage>(owner) && Cast<UStaticMeshComponent>(comp))
		{
			FVector deltaLoc = MoveDirArrowComp->GetForwardVector() * deltaDistance;
			deltaLoc = bIsForward ? deltaLoc : -deltaLoc;
			owner->AddActorWorldOffset(deltaLoc);
		}
	}
}
