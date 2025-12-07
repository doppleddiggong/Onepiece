// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "OrderKiosk.h"

#include "APlayerActor.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"


// Sets default values
AOrderKiosk::AOrderKiosk()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	Collision->SetupAttachment(GetRootComponent());

	InteractWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractWidget"));
	InteractWidget->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AOrderKiosk::BeginPlay()
{
	Super::BeginPlay();

	Collision->OnComponentBeginOverlap.AddDynamic(this, &AOrderKiosk::BeginOverlap);
	Collision->OnComponentEndOverlap.AddDynamic(this, &AOrderKiosk::EndOverlap);

	InteractWidget->SetVisibility(false);
}

// Called every frame
void AOrderKiosk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AOrderKiosk::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (APlayerActor* Player = Cast<APlayerActor>(OtherActor))
	{
		InteractWidget->SetVisibility(true);
	}
}

void AOrderKiosk::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (APlayerActor* Player = Cast<APlayerActor>(OtherActor))
	{
		InteractWidget->SetVisibility(false);
	}
}

