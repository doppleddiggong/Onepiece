// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "RespawnTrigger.h"

#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"

#define READ_LOCATION	FVector(2409.844832,-10722.498133,128.995287)
#define LISTEN_LOCATION FVector(4440.000000,-3730.000000,-4580.000000)

// Sets default values
ARespawnTrigger::ARespawnTrigger()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	SetRootComponent(Collision);

	RespawnQuest = EQuestType::None;
}

// Called when the game starts or when spawned
void ARespawnTrigger::BeginPlay()
{
	Super::BeginPlay();

	Collision->OnComponentBeginOverlap.AddDynamic(this, &ARespawnTrigger::BeginOverlap);
}

// Called every frame
void ARespawnTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ARespawnTrigger::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ACharacter* Player = Cast<ACharacter>(OtherActor))
	{
		Server_RepawnPlayer(Player);
	}
}

void ARespawnTrigger::Server_RepawnPlayer_Implementation(ACharacter* Player)
{
	switch (RespawnQuest)
	{
	case EQuestType::None:
	case EQuestType::Read:
		Player->SetActorLocation(READ_LOCATION);
		break;
	case EQuestType::Listen:
		Player->SetActorLocation(LISTEN_LOCATION);
		break;
	default:
		Player->SetActorLocation(READ_LOCATION);
		break;
	}
}

