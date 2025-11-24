// Fill out your copyright notice in the Description page of Project Settings.


#include "AOwlPlayer.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInput/Public/InputMappingContext.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"


// Sets default values
AOwlPlayer::AOwlPlayer()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// Constructor Helpers	
	ConstructorHelpers::FObjectFinder<UInputMappingContext> imcRef(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/ThirdPerson/Blueprints/Robots/Input/IMC_Robot.IMC_Robot'"));
	if (imcRef.Succeeded())
	{
		RobotIMC = imcRef.Object;
	}
	
	ConstructorHelpers::FObjectFinder<UInputAction> moveActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ThirdPerson/Blueprints/Robots/Input/IA_RobotMove.IA_RobotMove'"));
	if (moveActionRef.Succeeded())
	{
		MoveAction = moveActionRef.Object;
	}
	
	ConstructorHelpers::FObjectFinder<UInputAction> lookActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ThirdPerson/Blueprints/Robots/Input/IA_RobotLook.IA_RobotLook'"));
	if (lookActionRef.Succeeded())
	{
		LookAction = lookActionRef.Object;
	}
	
	ConstructorHelpers::FObjectFinder<UInputAction> jumpActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ThirdPerson/Blueprints/Robots/Input/IA_RobotJump.IA_RobotJump'"));
	if (jumpActionRef.Succeeded())
	{
		JumpAction = jumpActionRef.Object;
	}
	
	ConstructorHelpers::FObjectFinder<UInputAction> runActionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ThirdPerson/Blueprints/Robots/Input/IA_RobotRun.IA_RobotRun'"));
	if (runActionRef.Succeeded())
	{
		RunAction = runActionRef.Object;
	}
	
	// Mesh
	// Set Owl Mesh
	ConstructorHelpers::FObjectFinder<USkeletalMesh> playerMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/CustomContents/Character/Asset/Owl/SKM_GreenOwlRobot.SKM_GreenOwlRobot'"));
	if (playerMeshRef.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(playerMeshRef.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -70), FRotator(0, -90, 0));
	}
	
	// ABP
	// Set ABP_Owl
	ConstructorHelpers::FClassFinder<UAnimInstance> playerABPRef(TEXT("/Game/CustomContents/Animations/ABP_Owl.ABP_Owl_C"));
	if (playerABPRef.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(playerABPRef.Class);
	}
	
	// Capsule Comp
	GetCapsuleComponent()->InitCapsuleSize(35.f, 70.0f);
	
	// Movement
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;
	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	
	// Camera Comp
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 250.f;
	CameraBoom->bUsePawnControlRotation = false;
	
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(GetMesh(), TEXT("Head"));
	// First Person Camera
	FollowCamera->SetRelativeLocationAndRotation(FVector(0,-14.285715,14.285715), FRotator(90, 90, 0));
	FollowCamera->bUsePawnControlRotation = true;
}

void AOwlPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	auto pc = Cast<APlayerController>(Controller);
	if (pc)
	{
		auto subsys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());
		if (subsys)
		{
			subsys->AddMappingContext(RobotIMC, 0);
		}
	}
}

void AOwlPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AOwlPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	auto playerInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (playerInput)
	{
		playerInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AOwlPlayer::OnMove);
		playerInput->BindAction(MoveAction, ETriggerEvent::Completed, this, &AOwlPlayer::OnStopMove);
		playerInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &AOwlPlayer::OnLook);
		playerInput->BindAction(JumpAction, ETriggerEvent::Started, this, &AOwlPlayer::OnJump);
		playerInput->BindAction(RunAction, ETriggerEvent::Started, this, &AOwlPlayer::OnRun);
	}
}

void AOwlPlayer::OnMove(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();
	
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AOwlPlayer::OnStopMove()
{
	bIsRunning = false;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AOwlPlayer::OnLook(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AOwlPlayer::OnJump()
{
	bIsJumpStart = true;
}

void AOwlPlayer::OnRun()
{
	if (bIsJumpStart || GetMovementComponent()->IsFalling()) return;
	
	if (bIsRunning)
	{
		bIsRunning = false;
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}
	else
	{
		bIsRunning = true;
		GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
	}
}

bool AOwlPlayer::GetIsRunning()
{
	return bIsRunning;
}

bool AOwlPlayer::GetIsJumpStart()
{
	return bIsJumpStart;
}

void AOwlPlayer::DoJump()
{
	bIsJumpStart = false;
	Jump();
}
