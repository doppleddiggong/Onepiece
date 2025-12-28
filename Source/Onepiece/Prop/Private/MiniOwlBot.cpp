// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "MiniOwlBot.h"


// Sets default values
AMiniOwlBot::AMiniOwlBot()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMiniOwlBot::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMiniOwlBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// up down
	float valY = amplitude * FMath::Sin(DeltaTime * cycle);
	float valX = 0;
	
	SetActorLocation(FVector(valX, valY, 0));
}

// Called to bind functionality to input
void AMiniOwlBot::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

