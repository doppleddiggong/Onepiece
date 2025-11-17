// Fill out your copyright notice in the Description page of Project Settings.


#include "RobotAnimInstance.h"

#include "RobotPlayer.h"
#include "GameFramework/PawnMovementComponent.h"

void URobotAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	
	AActor* owningActor = GetOwningActor();
	if (owningActor)
	{
		OwningRobot = Cast<ARobotPlayer>(owningActor);
	}
}

void URobotAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if (OwningRobot)
	{
		bIsRunning = OwningRobot->GetIsRunning();
		bIsJumpStart = OwningRobot->GetIsJumpStart();
		// 이동 속도 설정
		velocity = OwningRobot->GetVelocity();
		// 수평 이동 속력 설정
		groundSpeed = velocity.Size2D();
		// 공중 여부 설정
		isAir = OwningRobot->GetMovementComponent()->IsFalling();
		// 앞뒤 이동에 따른 값 설정
		FVector v = velocity;
		v.Z = 0;
	}
}

void URobotAnimInstance::AnimNotify_OnJumpStart()
{
	OwningRobot->DoJump();
}
