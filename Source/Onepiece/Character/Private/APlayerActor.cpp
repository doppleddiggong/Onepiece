// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file APlayerActor.cpp
 * @brief Provides Doxygen annotations for APlayerActor implementation.
 */

#include "APlayerActor.h"

#include "UFlySystem.h"

// Shared
#include "Macro.h"
#include "InputCoreTypes.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

APlayerActor::APlayerActor()
{
	PrimaryActorTick.bCanEverTick = true;

	FlySystem = CreateDefaultSubobject<UFlySystem>(TEXT("FlySystem"));
	
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(GetCapsuleComponent());
	SpringArmComp->TargetArmLength = 400.f;
	SpringArmComp->bUsePawnControlRotation = true;
	// SpringArmComp->bInheritPitch = false;
	SpringArmComp->bInheritRoll = false;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	SpringArmComp->bInheritPitch = true;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
}

void APlayerActor::BeginPlay()
{
	Super::BeginPlay();

	MoveComp = this->GetCharacterMovement();
	
	FlySystem->InitSystem(this, BIND_DYNAMIC_DELEGATE(FEndCallback, this, APlayerActor, OnFlyEnd));

	// --- Architecture Demo Start ---
	UE_LOG(LogTemp, Log, TEXT("APlayerActor: Setting up one-way dependency demo."));
}

void APlayerActor::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void APlayerActor::OnFlyEnd_Implementation()
{
}

void APlayerActor::SetFlying()
{
	MoveComp->SetMovementMode(MOVE_Flying);
	PrevMoveMode = EMovementMode::MOVE_Flying;

	this->bUseControllerRotationYaw = true;
	this->bUseControllerRotationPitch = true;
	MoveComp->bOrientRotationToMovement = false;
}

void APlayerActor::SetFallingToWalk()
{
	MoveComp->SetMovementMode( EMovementMode::MOVE_Falling );
	PrevMoveMode = EMovementMode::MOVE_Falling;

	this->bUseControllerRotationYaw = true;
	this->bUseControllerRotationPitch = false;
	MoveComp->bOrientRotationToMovement = false;
}

void APlayerActor::RecoveryMovementMode(const EMovementMode InMovementMode)
{
	if ( InMovementMode == MOVE_None)
		return;
	
	auto Movement = this->GetCharacterMovement();

	if ( InMovementMode == MOVE_Flying )
	{
		Movement->SetMovementMode( EMovementMode::MOVE_Flying );
		this->bUseControllerRotationYaw = true;
		this->bUseControllerRotationPitch = true;
		Movement->bOrientRotationToMovement = false;
	}
	else
	{
		Movement->SetMovementMode( InMovementMode );
		this->bUseControllerRotationYaw = false;
		this->bUseControllerRotationPitch = false;
		Movement->bOrientRotationToMovement = true;
	}
}


void APlayerActor::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if (FlySystem)
		FlySystem->OnLand(Hit);
}

void APlayerActor::Cmd_Move_Implementation(const FVector2D& Axis)
{
	if ( !Controller)
	{
		return;
	}

	// Use the controller's rotation to determine movement direction.
	const FRotator ControlRotation = Controller->GetControlRotation();
	const EMovementMode CurrentMovementMode = GetCharacterMovement()->MovementMode;

	if (CurrentMovementMode == MOVE_Walking || CurrentMovementMode == MOVE_Falling)
	{
		// For ground movement, only use the Yaw rotation to prevent pitching into the ground.
		const FRotator YawRotation(0.0f, ControlRotation.Yaw, 0.0f);

		// Calculate forward and right vectors based on the Yaw rotation.
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, Axis.Y);
		AddMovementInput(RightDirection, Axis.X);
	}
	else if (CurrentMovementMode == MOVE_Flying)
	{
		// For flying, use the full 3D rotation of the controller.
		const FRotator FullRotation(ControlRotation.Pitch, ControlRotation.Yaw, 0.0f);

		const FVector ForwardDirection = FRotationMatrix(FullRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(FullRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, Axis.Y);
		AddMovementInput(RightDirection, Axis.X);
	}
}

void APlayerActor::Cmd_Look_Implementation(const FVector2D& Axis)
{
	AddControllerYawInput(Axis.X);
	AddControllerPitchInput(Axis.Y);
}

void APlayerActor::Cmd_AltitudeUp_Implementation()
{
	FlySystem->OnAltitudePress(true);
}

void APlayerActor::Cmd_AltitudeDown_Implementation()
{
	FlySystem->OnAltitudePress(false);
}

void APlayerActor::Cmd_AltitudeReleased_Implementation()
{
	FlySystem->OnAltitudeRelease();
}

void APlayerActor::Cmd_Jump_Implementation()
{
	FlySystem->OnJump();
}

void APlayerActor::Cmd_Landing_Implementation()
{
	FHitResult HitResult;
	FlySystem->OnLand(HitResult);
}