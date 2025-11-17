// Fill out your copyright notice in the Description page of Project Settings.


#include "RobotPlayer.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInput/Public/InputMappingContext.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"


// Sets default values
ARobotPlayer::ARobotPlayer()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// Constructor Helpers
	ConstructorHelpers::FObjectFinder<USkeletalMesh> meshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/RadicalMike/Mesh/SKM_MegaMikeZ.SKM_MegaMikeZ'"));
	if (meshRef.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(meshRef.Object);
	}
	
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
	GetMesh()->SetRelativeLocationAndRotation(FVector(0,0,-70), FRotator(0,-90,0));
	
	// Capsule Comp
	GetCapsuleComponent()->InitCapsuleSize(35.f, 70.0f);
	
	// Movement
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	
	// Camera Comp
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.f;
	CameraBoom->bUsePawnControlRotation = true;
	
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom);
	FollowCamera->bUsePawnControlRotation = false;
}

// Called when the game starts or when spawned
void ARobotPlayer::BeginPlay()
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

// Called every frame
void ARobotPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ARobotPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	auto playerInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (playerInput)
	{
		playerInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ARobotPlayer::OnMove);
		playerInput->BindAction(MoveAction, ETriggerEvent::Completed, this, &ARobotPlayer::OnStopMove);
		playerInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &ARobotPlayer::OnLook);
		playerInput->BindAction(JumpAction, ETriggerEvent::Started, this, &ARobotPlayer::OnJump);
		playerInput->BindAction(RunAction, ETriggerEvent::Started, this, &ARobotPlayer::OnRun);
	}
}

void ARobotPlayer::OnMove(const FInputActionValue& Value)
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

void ARobotPlayer::OnStopMove()
{
	bIsRunning = false;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void ARobotPlayer::OnLook(const FInputActionValue& Value)
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

void ARobotPlayer::OnJump()
{
	bIsJumpStart = true;
}

void ARobotPlayer::OnRun()
{
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

bool ARobotPlayer::GetIsRunning()
{
	return bIsRunning;
}

bool ARobotPlayer::GetIsJumpStart()
{
	return bIsJumpStart;
}

void ARobotPlayer::DoJump()
{
	bIsJumpStart = false;
	Jump();
}
