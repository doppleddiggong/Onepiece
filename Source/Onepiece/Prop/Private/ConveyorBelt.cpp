// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "ConveyorBelt.h"

#include "ANPCBase.h"
#include "APlayerActor.h"
#include "GameLogging.h"
#include "InteractableComponent.h"
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
		// AActor* owner = comp->GetOwner();
		// if (Cast<Aluggage>(owner) && Cast<UStaticMeshComponent>(comp))
		// {
		// 	FVector deltaLoc = MoveDirArrowComp->GetForwardVector() * deltaDistance;
		// 	deltaLoc = bIsForward ? deltaLoc : -deltaLoc;
		// 	owner->AddActorWorldOffset(deltaLoc);
		// }

		AActor* owner = comp->GetOwner();
		Aluggage* luggage = Cast<Aluggage>(owner);
		if (luggage && Cast<UStaticMeshComponent>(comp))
		{
			// Hook이나 PickUp 중이면 컨베이어 이동 무시
			if (luggage->InteractableComp && luggage->InteractableComp->IsPickedUp())
			{
				continue;
			}

			FVector moveDirection = MoveDirArrowComp->GetForwardVector();
			moveDirection = bIsForward ? moveDirection : -moveDirection;

			// 물리 객체인지 확인
			UPrimitiveComponent* primComp = Cast<UPrimitiveComponent>(comp);
			if (primComp && primComp->IsSimulatingPhysics())
			{
				// 물리 기반 이동 - 속도 설정 방식
				float speed = MoveSpeed;
				FVector targetVelocity = moveDirection * speed;

				// 현재 속도를 가져와서 컨베이어 방향 속도만 덮어씀 (Z축 중력은 유지)
				FVector currentVelocity = primComp->GetPhysicsLinearVelocity();
				FVector newVelocity = targetVelocity;
				newVelocity.Z = currentVelocity.Z; // Z축(중력) 속도는 유지

				primComp->SetPhysicsLinearVelocity(newVelocity, false);
			}
			else
			{
				// 물리가 아닌 경우 (Hook/PickUp 중) - 이동하지 않음
				// Hook 중에는 물리가 꺼져있으므로 여기서 멈춤
			}
		}
	}
}
