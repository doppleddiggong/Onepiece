// Fill out your copyright notice in the Description page of Project Settings.


#include "UPlayerAnimInstance.h"

#include "AOwlPlayer.h"
#include "GameFramework/PawnMovementComponent.h"

void URobotAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	
	AActor* owningActor = GetOwningActor();
	if (owningActor)
	{
		OwningRobot = Cast<AOwlPlayer>(owningActor);
	}
}

void URobotAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if (OwningRobot)
	{
		bIsRunning = OwningRobot->GetIsRunning();
		bIsJumpStart = OwningRobot->GetIsJumpStart();
		velocity = OwningRobot->GetVelocity();
		horizontal = FVector::DotProduct(velocity, OwningRobot->GetActorRightVector());
		vertical = FVector::DotProduct(velocity, OwningRobot->GetActorForwardVector());
		bIsAir = OwningRobot->GetMovementComponent()->IsFalling();
	}
}

void URobotAnimInstance::AnimNotify_OnJumpStart()
{
	OwningRobot->DoJump();
}
