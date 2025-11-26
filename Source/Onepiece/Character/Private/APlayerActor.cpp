// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file APlayerActor.cpp
 * @brief Provides Doxygen annotations for APlayerActor implementation.
 */

#include "APlayerActor.h"

#include "UMainWidget.h"
#include "FComponentHelper.h"

// Shared
#include "GameLogging.h"
#include "Macro.h"
#include "InputCoreTypes.h"
#include "UDialogManager.h"
#include "UVoiceConversationSystem.h"
#include "UInteractionSystem.h"

#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Blueprint/UserWidget.h"
#include "Onepiece/Onepiece.h"

#define MAINWIDGET_PATH TEXT("/Game/CustomContents/UI/WBP_MainWidget.WBP_MainWidget_C")

APlayerActor::APlayerActor()
{
	PrimaryActorTick.bCanEverTick = false;
	
	
	GetCapsuleComponent()->InitCapsuleSize(45.f, 102.0f);
	GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -100), FRotator(0, -90, 0));
	GetMesh()->SetRelativeScale3D(FVector(1.5));
	
	// Movement
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;
	
	// SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	// SpringArmComp->SetupAttachment(GetMesh(), TEXT("Head"));
	// SpringArmComp->TargetArmLength = 0;
	// SpringArmComp->bUsePawnControlRotation = false;
	// SpringArmComp->SetRelativeLocationAndRotation(FVector(0,-14.285715,21.428572), FRotator(90, 90, 0));
	// // SpringArmComp->bInheritPitch = false;
	// SpringArmComp->bInheritRoll = false;
	// SpringArmComp->bInheritPitch = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	// FollowCamera->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	FollowCamera->SetupAttachment(GetMesh(), TEXT("Head"));
	FollowCamera->SetRelativeLocationAndRotation(FVector(0.235242,-14.285715,24.304751), FRotator(90, 90, 0));
	FollowCamera->bUsePawnControlRotation = true;

	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	HoldPosition = CreateDefaultSubobject<USceneComponent>(TEXT("HoldPosition"));
	HoldPosition->SetupAttachment(FollowCamera);
	HoldPosition->SetRelativeLocation(FVector(80.952382,0,-14.285714));

	LookPitch = 0.f;

	// System Component
	InteractionSystem = CreateDefaultSubobject<UInteractionSystem>(TEXT("InteractionSystem"));
	VoiceConversationSystem = CreateDefaultSubobject<UVoiceConversationSystem>(TEXT("VoiceConversationSystem"));

	// MainWidget 클래스 자동 로드
	MainWidgetClass = FComponentHelper::LoadClass<UMainWidget>(MAINWIDGET_PATH);
}

void APlayerActor::BeginPlay()
{
	Super::BeginPlay();

	MoveComp = this->GetCharacterMovement();

	VoiceConversationSystem->InitSystem(this);

	// 로컬 플레이어 컨트롤러인 경우에만 UI 생성
	if (IsLocallyControlled())
		CreateMainWidget();

	// --- Architecture Demo Start ---
	PRINTLOG( TEXT("APlayerActor: Setting up one-way dependency demo."));
}


void APlayerActor::CreateMainWidget()
{
	if (!MainWidgetClass)
	{
		PRINTLOG(TEXT("[PlayerActor] MainWidgetClass is not set!"));
		return;
	}

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		MainWidget = CreateWidget<UMainWidget>(PC, MainWidgetClass);
		if (MainWidget)
			MainWidget->AddToViewport();
	}
}


void APlayerActor::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void APlayerActor::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerActor, LookPitch);
}

void APlayerActor::RecoveryMovementMode(const EMovementMode InMovementMode)
{
	if ( InMovementMode == MOVE_None)
		return;
	
	auto Movement = this->GetCharacterMovement();

	Movement->SetMovementMode( InMovementMode );
	this->bUseControllerRotationYaw = false;
	this->bUseControllerRotationPitch = false;
	Movement->bOrientRotationToMovement = true;
}


void APlayerActor::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
}

void APlayerActor::PlayTTSAudio(const TArray<uint8>& AudioData)
{
	VoiceConversationSystem->PlayVoiceAudio(AudioData);
}

void APlayerActor::Cmd_StopMove_Implementation()
{
	bIsRunning = false;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void APlayerActor::Cmd_Run_Implementation()
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


void APlayerActor::OnRep_LookPitch()
{
	// if (SpringArmComp)
	// {
	// 	// 다른 클라이언트에서 보이는 서버 캐릭터의 복제본의 설정 변경
	// 	SpringArmComp->bUsePawnControlRotation = false;
	//
	// 	FRotator CurrentRotation = SpringArmComp->GetRelativeRotation();
	// 	CurrentRotation.Pitch = LookPitch;
	// 	SpringArmComp->SetRelativeRotation(CurrentRotation);
	// }
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
}

void APlayerActor::Cmd_Look_Implementation(const FVector2D& Axis)
{
	AddControllerYawInput(Axis.X);
	AddControllerPitchInput(Axis.Y);
}

void APlayerActor::Cmd_Jump_Implementation()
{
	bIsJumpStart = true;
}

void APlayerActor::Cmd_RecordStart_Implementation()
{
	VoiceConversationSystem->StartRecording();
}

void APlayerActor::Cmd_RecordEnd_Implementation()
{
	VoiceConversationSystem->StopRecording();
}

void APlayerActor::Cmd_Landing_Implementation()
{
	FHitResult HitResult;
}

void APlayerActor::OnGameMessage(const FString& Message)
{
	PRINT_STRING(TEXT("Event Received: %s"), *Message);

	// 퀘스트 시작이 뜨면 메세지 팝업을 띄우자
	UDialogManager::Get(GetWorld())->ShowToast(Message);
}

bool APlayerActor::GetIsRunning()
{
	return bIsRunning;
}

bool APlayerActor::GetIsJumpStart()
{
	return bIsJumpStart;
}

void APlayerActor::DoJump()
{
	bIsJumpStart = false;
	Jump();
}
