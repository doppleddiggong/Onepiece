// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "OutBoxTeleportTrigger.h"

#include "ATeleportOut.h"
#include "Food.h"
#include "GameLogging.h"
#include "luggage.h"
#include "LuggageManager.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"


AOutBoxTeleportTrigger::AOutBoxTeleportTrigger()
{
	PrimaryActorTick.bCanEverTick = false;
	
	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);
	
	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	BoxComp->SetupAttachment(GetRootComponent());
	BoxComp->SetBoxExtent(FVector(1, 1, 1));
	BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxComp->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	BoxComp->SetCollisionResponseToAllChannels(ECR_Overlap);
	
	CubeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CubeMesh"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> cubeMeshRef(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	if (cubeMeshRef.Succeeded())
	{
		CubeMesh->SetStaticMesh(cubeMeshRef.Object);
	}
	CubeMesh->SetupAttachment(GetRootComponent());
	CubeMesh->SetVisibility(false);
}

void AOutBoxTeleportTrigger::BeginPlay()
{
	Super::BeginPlay();
	
	AActor* luggageManager = UGameplayStatics::GetActorOfClass(GetWorld(), ALuggageManager::StaticClass());
	LuggageRespawnPoint = luggageManager->GetActorLocation();
	
	AActor* teleportOut = UGameplayStatics::GetActorOfClass(GetWorld(), ATeleportOut::StaticClass());
	FoodRespawnPoint = teleportOut->GetActorLocation();
	
	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &AOutBoxTeleportTrigger::OnBoxBeginOverlap);
}

void AOutBoxTeleportTrigger::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<Aluggage>(OtherActor))
	{
		OtherActor->SetActorLocation(LuggageRespawnPoint, false, nullptr, ETeleportType::TeleportPhysics);
	}
	else if (Cast<AFood>(OtherActor))
	{
		OtherActor->SetActorLocation(FoodRespawnPoint, false, nullptr, ETeleportType::TeleportPhysics);
	}
}

