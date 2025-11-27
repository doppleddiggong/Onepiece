// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file APlayerControl.cpp
 * @brief APlayerControl 구현에 대한 Doxygen 주석을 제공합니다.
 */
#include "APlayerControl.h"

#include "APlayerActor.h"
#include "IControllable.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

#include "InputMappingContext.h"
#include "InputAction.h"

#include "FComponentHelper.h"
#include "UBroadcastManager.h"
#include "UInteractionSystem.h"

#define IMC_DEFAULT_PATH			TEXT("/Game/CustomContents/Input/IMC_Game_Player.IMC_Game_Player")
#define IA_MOVE_PATH				TEXT("/Game/CustomContents/Input/IA_Game_Movement.IA_Game_Movement")
#define IA_LOOK_PATH				TEXT("/Game/CustomContents/Input/IA_Game_LookAround.IA_Game_LookAround")
#define IA_JUMP_PATH				TEXT("/Game/CustomContents/Input/IA_Game_Jump.IA_Game_Jump")
#define IA_RECORD_PATH				TEXT("/Game/CustomContents/Input/IA_Game_Record.IA_Game_Record")
#define IA_GRAB_PATH				TEXT("/Game/CustomContents/Input/IA_Game_Grab.IA_Game_Grab")
#define IA_INTERACT_PATH			TEXT("/Game/CustomContents/Input/IA_Game_Interact.IA_Game_Interact")
#define IA_RUN_PATH					TEXT("/Game/CustomContents/Input/IA_Game_Run.IA_Game_Run")
#define IA_INFO_PATH				TEXT("/Game/CustomContents/Input/IA_Game_Info.IA_Game_Info")


APlayerControl::APlayerControl()
{
	IMC_Default = FComponentHelper::LoadAsset<UInputMappingContext>(IMC_DEFAULT_PATH);

	IA_Move = FComponentHelper::LoadAsset<UInputAction>(IA_MOVE_PATH);
	IA_Look = FComponentHelper::LoadAsset<UInputAction>(IA_LOOK_PATH);
	IA_Jump = FComponentHelper::LoadAsset<UInputAction>(IA_JUMP_PATH);
	IA_Record = FComponentHelper::LoadAsset<UInputAction>(IA_RECORD_PATH);
	IA_Grab = FComponentHelper::LoadAsset<UInputAction>(IA_GRAB_PATH);
	IA_Interact = FComponentHelper::LoadAsset<UInputAction>(IA_INTERACT_PATH);
	IA_Run = FComponentHelper::LoadAsset<UInputAction>(IA_RUN_PATH);
	IA_Info = FComponentHelper::LoadAsset<UInputAction>(IA_INFO_PATH);
}

void APlayerControl::BeginPlay()
{
	Super::BeginPlay();

	if (auto LP = GetLocalPlayer())
	{
		if (auto SubSystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (IMC_Default)
			{
				SubSystem->ClearAllMappings();
				SubSystem->AddMappingContext(IMC_Default, 0);
			}
		}
	}
}

void APlayerControl::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EIC->BindAction(IA_Move, ETriggerEvent::Triggered,  this, &APlayerControl::OnMove);
		EIC->BindAction(IA_Move, ETriggerEvent::Completed,  this, &APlayerControl::OnStopMove);
		EIC->BindAction(IA_Look, ETriggerEvent::Triggered,  this, &APlayerControl::OnLook);
		
		EIC->BindAction(IA_Jump, ETriggerEvent::Started,    this, &APlayerControl::OnJump);

		EIC->BindAction(IA_Record, ETriggerEvent::Started, this, &APlayerControl::OnRecordPressed);
		EIC->BindAction(IA_Record, ETriggerEvent::Completed, this, &APlayerControl::OnRecordReleased);

		EIC->BindAction(IA_Grab, ETriggerEvent::Started, this, &APlayerControl::OnGrab);
		EIC->BindAction(IA_Grab, ETriggerEvent::Completed, this, &APlayerControl::OnGrabRelease);

		EIC->BindAction(IA_Interact, ETriggerEvent::Started, this, &APlayerControl::OnInteract);
		EIC->BindAction(IA_Run, ETriggerEvent::Started, this, &APlayerControl::OnRun);

		EIC->BindAction(IA_Info, ETriggerEvent::Started, this, &APlayerControl::OnInfo);
	}
}

void APlayerControl::UpdateQuestRole(EQuestRole QuestRole)
{
	UBroadcastManager::Get(GetWorld())->SendUpdateQuestRole(QuestRole);
}

IControllable* APlayerControl::GetControllable() const
{
	APawn* P = GetPawn();
	if (!P)
		return nullptr;

	// UObject 기반 UInterface 라면 Cast 가능
	if (IControllable* C = Cast<IControllable>(P))
		return C;

	return nullptr;
}

void APlayerControl::OnMove(const FInputActionValue& Value)
{
	if (IControllable* C = GetControllable())
		C->Cmd_Move(Value.Get<FVector2D>());
}

void APlayerControl::OnLook(const FInputActionValue& Value)
{
	if (IControllable* C = GetControllable())
		C->Cmd_Look(Value.Get<FVector2D>());
}

void APlayerControl::OnStopMove(const FInputActionValue& Value)
{
	if (IControllable* C = GetControllable())
	{
		C->Cmd_StopMove();
	}
}

void APlayerControl::OnJump(const FInputActionValue&)
{
	if (IControllable* C = GetControllable())
		C->Cmd_Jump();
}

void APlayerControl::OnRun(const FInputActionValue& Value)
{
	if (IControllable* C = GetControllable())
	{
		C->Cmd_Run();
	}
}

void APlayerControl::OnRecordPressed(const FInputActionValue& Value)
{
	if (IControllable* C = GetControllable())
		C->Cmd_RecordStart();
}

void APlayerControl::OnRecordReleased(const FInputActionValue& Value)
{
	if (IControllable* C = GetControllable())
		C->Cmd_RecordEnd();
}

void APlayerControl::OnInfo(const FInputActionValue& Value)
{
	if (IControllable* C = GetControllable())
	{
		C->Cmd_Info();
	}
}

void APlayerControl::OnGrab(const FInputActionValue& Value)
{
	Server_OnGrab();
}

void APlayerControl::OnGrabRelease(const FInputActionValue& Value)
{
	Server_OnGrabRelease();
}

void APlayerControl::OnInteract(const FInputActionValue& Value)
{
	Server_OnInteract();
}

void APlayerControl::Server_OnGrab_Implementation()
{
	APlayerActor* MyPlayer = Cast<APlayerActor>(GetPawn());
	if (MyPlayer && MyPlayer->InteractionSystem)
	{
		MyPlayer->InteractionSystem->TryPickUp();
	}
}

void APlayerControl::Server_OnGrabRelease_Implementation()
{
	APlayerActor* MyPlayer = Cast<APlayerActor>(GetPawn());
	if (MyPlayer && MyPlayer->InteractionSystem)
	{
		MyPlayer->InteractionSystem->TryDrop();
	}
}


void APlayerControl::Server_OnInteract_Implementation()
{
	APlayerActor* MyPlayer = Cast<APlayerActor>(GetPawn());
	if (MyPlayer && MyPlayer->InteractionSystem)
	{
		MyPlayer->InteractionSystem->TryInteract();
	}
}