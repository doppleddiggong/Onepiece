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
#include "UHookSystem.h"
#include "ULingoGameHelper.h"
#include "UPopupManager.h"
#include "UPopup_ReadQuest.h"
#include "UBroadcastManager.h"
#include "ALingoGameState.h"

#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Blueprint/UserWidget.h"
#include "CableComponent.h"
#include "Components/StaticMeshComponent.h"
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
	HookSystem = CreateDefaultSubobject<UHookSystem>(TEXT("HookSystem"));
	VoiceConversationSystem = CreateDefaultSubobject<UVoiceConversationSystem>(TEXT("VoiceConversationSystem"));

	// Hook Cable Component
	HookCable = CreateDefaultSubobject<UCableComponent>(TEXT("HookCable"));
	HookCable->SetupAttachment(GetMesh());
	HookCable->SetVisibility(false);

	// Hook Projectile Mesh (훅 발사체 시각화)
	HookProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HookProjectileMesh"));
	HookProjectileMesh->SetupAttachment(GetMesh());
	HookProjectileMesh->SetVisibility(false);
	HookProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// 기본 구체 메시 사용 (에디터에서 변경 가능)
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereMesh.Succeeded())
	{
		HookProjectileMesh->SetStaticMesh(SphereMesh.Object);
		HookProjectileMesh->SetRelativeScale3D(FVector(0.3f)); // 작은 크기
	}

	// MainWidget 클래스 자동 로드
	MainWidgetClass = FComponentHelper::LoadClass<UMainWidget>(MAINWIDGET_PATH);
}

void APlayerActor::BeginPlay()
{
	Super::BeginPlay();

	MoveComp = this->GetCharacterMovement();

	VoiceConversationSystem->InitSystem(this);

	// Hook System 초기화 - Cable과 Projectile Mesh 전달
	if (HookSystem && HookCable && HookProjectileMesh)
	{
		HookSystem->InitSystem(HookCable, HookProjectileMesh);
	}

	if (IsLocallyControlled())
	{
		CreateMainWidget();
		
		// Map1(게임 맵)에서는 마우스 숨기기
		FString MapName = GetWorld()->GetMapName();
		// PIE 프리픽스 제거
		MapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);
		
		PRINTLOG(TEXT("[PlayerActor] Current Map: %s"), *MapName);
		
		if (MapName.Contains(TEXT("Map1")) || MapName.Contains(TEXT("Game")))
		{
			ULingoGameHelper::HideMouseCursor(this);
			PRINTLOG(TEXT("[PlayerActor] Mouse cursor hidden for game map"));
		}
	}
}

void APlayerActor::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerActor, LookPitch);
}

void APlayerActor::CreateMainWidget()
{
	if (!MainWidgetClass)
		return;

	if(MainWidget)
		return;

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC)
		return;

	MainWidget = CreateWidget<UMainWidget>(PC, MainWidgetClass);
	if (MainWidget)
		MainWidget->AddToViewport();
}


void APlayerActor::OnRep_Controller()
{
	Super::OnRep_Controller();

	if (IsLocallyControlled())
		CreateMainWidget();
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

void APlayerActor::PlayTTSAudio(const TArray<uint8>& AudioData)
{
	VoiceConversationSystem->PlayVoiceAudio(AudioData);
}


void APlayerActor::DoJump()
{
	bIsJumpStart = false;
	Jump();
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

void APlayerActor::Cmd_Info_Implementation()
{
	auto GS = ULingoGameHelper::GetLingoGameState(GetWorld());
	if ( !GS->IsQuestIng() )
		return;
	
	if (const auto PopupMgr = UPopupManager::Get(GetWorld()))
	{
		if (const auto Popup = Cast<UPopup_ReadQuest>(PopupMgr->ShowPopup(EPopupType::ReadQuest)))
		{
			Popup->InitPopup( GS->CurScenarioData);
		}
	}
}

void APlayerActor::OnGameMessage(const FString& Message)
{
	PRINT_STRING(TEXT("Event Received: %s"), *Message);

	// 서버에서 호출되면 클라이언트 RPC로 전달
	if (HasAuthority())
	{
		ClientRPC_ShowGameMessage(Message);
	}
	else
	{
		ClientRPC_ShowGameMessage_Implementation(Message);
	}
}

void APlayerActor::ClientRPC_ShowGameMessage_Implementation(const FString& Message)
{
	// 클라이언트에서만 실행됨 - LocalPlayerSubsystem 사용 가능
	if (UDialogManager* DialogManager = UDialogManager::Get(GetWorld()))
	{
		DialogManager->ShowToast(Message);
	}
}