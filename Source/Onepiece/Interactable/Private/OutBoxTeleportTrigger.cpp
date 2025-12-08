// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "OutBoxTeleportTrigger.h"

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
	BoxComp->SetBoxExtent(FVector(835.0, 4000, 32));
}

void AOutBoxTeleportTrigger::BeginPlay()
{
	Super::BeginPlay();
	
	AActor* luggageManager = UGameplayStatics::GetActorOfClass(GetWorld(), ALuggageManager::StaticClass());
	RespawnPoint = luggageManager->GetActorLocation();
	
	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &AOutBoxTeleportTrigger::OnBoxBeginOverlap);
}

void AOutBoxTeleportTrigger::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<Aluggage>(OtherActor))
	{
		OtherActor->SetActorLocation(RespawnPoint, false, nullptr, ETeleportType::TeleportPhysics);
	}
}

