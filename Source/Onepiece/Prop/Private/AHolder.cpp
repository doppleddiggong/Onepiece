// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "AHolder.h"


// Sets default values
AHolder::AHolder()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AHolder::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHolder::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

