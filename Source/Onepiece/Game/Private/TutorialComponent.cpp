// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "TutorialComponent.h"

#include "APedestalSwitch.h"
#include "APlayerControl.h"
#include "luggage.h"
#include "UBroadcastManager.h"
#include "GameFramework/Character.h"


// Sets default values for this component's properties
UTutorialComponent::UTutorialComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...

	CurrentStep = ETutorialStep::Waiting;
	OwnerController = nullptr;
}


// Called when the game starts
void UTutorialComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	OwnerController = Cast<APlayerController>(GetOwner());
	if (!OwnerController)
	{
		UE_LOG(LogTemp, Error, TEXT("TutorialComponent: Owner is not a PlayerController!"));
		return;
	}

	LastControlRotation = OwnerController->GetControlRotation();
}


// Called every frame
void UTutorialComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                       FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (OwnerController)
	{
		FRotator CurrentRotation = OwnerController->GetControlRotation();

		// 튜토리얼 중이면 입력 조건 체크
		if (CurrentStep != ETutorialStep::Completed && CurrentStep != ETutorialStep::Waiting)
		{
			CheckInputConditions();
		}

		LastControlRotation = CurrentRotation;
	}
}

void UTutorialComponent::StartTutorial()
{
	SetStep(ETutorialStep::MouseLook);
}

void UTutorialComponent::AdvanceToNextStep()
{
	UE_LOG(LogTemp, Warning, TEXT("[AdvanceToNextStep] Called! CurrentStep: %d"), (int32)CurrentStep);

	// 현재 메시지 숨김
	if (UBroadcastManager* BM = UBroadcastManager::Get(GetWorld()))
	{
		BM->SendHideTutorialMessage();
	}

	ETutorialStep NextStep = GetNextStep(CurrentStep);
	UE_LOG(LogTemp, Warning, TEXT("[AdvanceToNextStep] NextStep: %d"), (int32)NextStep);
	SetStep(NextStep);
}

void UTutorialComponent::SetStep(ETutorialStep NewStep)
{
	CurrentStep = NewStep;
	bInputConditionMet = false; // 플래그 초기화

	// 상호작용 플래그 초기화
	bGrabbedLuggage = false;
	bPickedUpSomething = false;
	bInteractedWithSwitch = false;

	if (UBroadcastManager* BM = UBroadcastManager::Get(GetWorld()))
	{
		BM->SendTutorialStepChanged(OwnerController, NewStep);
		
		FText TutorialMessage = GetTutorialMessage(NewStep);
		if (NewStep == ETutorialStep::Completed)
		{
			BM->SendTutorMessage(TutorialMessage);

			// 튜토리얼 완료 시 PlayerControl에 알림
			if (APlayerControl* PC = Cast<APlayerControl>(OwnerController))
			{
				PC->OnTutorialCompleted();
			}
		}
		else
		{
			BM->SendShowTutorialMessage(TutorialMessage);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Tutorial Step Changed: %d"), (int32)NewStep);
}

bool UTutorialComponent::IsTutorialCompleted() const
{
	return CurrentStep == ETutorialStep::Completed;
}

void UTutorialComponent::CheckInputConditions()
{
	if (!OwnerController || bInputConditionMet) return;
	
	bool bConditionMet = false;

	switch (CurrentStep)
	{
	case ETutorialStep::MouseLook:
		bConditionMet = CheckMouseLookInput();
		break;
	case ETutorialStep::Movement:
		bConditionMet = CheckMovementInput();
		break;
	case ETutorialStep::Sprint:
		bConditionMet = CheckSprintInput();
		break;
	case ETutorialStep::Jump:
		bConditionMet = CheckJumpInput();
		break;
	case ETutorialStep::PickUp:
		bConditionMet = CheckPickUpInput();
		break;
	case ETutorialStep::GrabGun:
		bConditionMet = CheckGrabGunInput();
		break;
	case ETutorialStep::Interaction:
		bConditionMet = CheckInteractionInput();
		break;
	default:
		break;
	}

	if (bConditionMet && !bInputConditionMet)
	{
		UE_LOG(LogTemp, Warning, TEXT("[CheckInputConditions] Condition met! Advancing to next step..."));
		bInputConditionMet = true;
		OnInputConditionMet();
	}
}

void UTutorialComponent::OnInputConditionMet()
{
	if (AdvanceDelayTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(AdvanceDelayTimerHandle);
	}

	GetWorld()->GetTimerManager().SetTimer(AdvanceDelayTimerHandle, this,
		&UTutorialComponent::AdvanceToNextStep, 1.5f, false);
}

ETutorialStep UTutorialComponent::GetNextStep(ETutorialStep Step) const
{
	switch (Step)
	{
		case ETutorialStep::Waiting:		return ETutorialStep::MouseLook;
		case ETutorialStep::MouseLook:		return ETutorialStep::Movement;
		case ETutorialStep::Movement:		return ETutorialStep::Sprint;
		case ETutorialStep::Sprint:			return ETutorialStep::Jump;
		case ETutorialStep::Jump:			return ETutorialStep::GrabGun;
		case ETutorialStep::GrabGun:		return ETutorialStep::PickUp;
		case ETutorialStep::PickUp:			return ETutorialStep::Interaction;
		case ETutorialStep::Interaction:	return ETutorialStep::Completed;
		default: return ETutorialStep::Completed;
	}
}

bool UTutorialComponent::CheckMouseLookInput() const
{
	if (!OwnerController) return false;
		
	FRotator CurrentRotation = OwnerController->GetControlRotation();
	FRotator DeltaRotation = CurrentRotation - LastControlRotation;
	
	// 회전 변화가 임계값 이상인지 확인 (Pitch = 상하, Yaw = 좌우)                                                                                                                                                              
	return FMath::Abs(DeltaRotation.Pitch) > 0.5f || FMath::Abs(DeltaRotation.Yaw) > 0.5f;
}

bool UTutorialComponent::CheckMovementInput() const
{
	if (!OwnerController) return false;

	APawn* Pawn = OwnerController->GetPawn();
	if (!Pawn) return false;

	// WASD
	FVector InputVector = Pawn->GetLastMovementInputVector();
	return !InputVector.IsNearlyZero();
}

bool UTutorialComponent::CheckSprintInput() const
{
	if (!OwnerController) return false;

	// Shift
	return OwnerController->IsInputKeyDown(EKeys::LeftShift);
}

bool UTutorialComponent::CheckJumpInput() const
{
	if (!OwnerController) return false;

	return OwnerController->IsInputKeyDown(EKeys::SpaceBar);
}

bool UTutorialComponent::CheckPickUpInput() const
{
	//UE_LOG(LogTemp, Warning, TEXT("[CheckPickUpInput] bPickedUpSomething: %d"), bPickedUpSomething);
	return bPickedUpSomething;
}

bool UTutorialComponent::CheckGrabGunInput() const
{
	// 오른쪽 마우스 버튼 클릭
	//return OwnerController->WasInputKeyJustPressed(EKeys::RightMouseButton);
	return bGrabbedLuggage;
}

bool UTutorialComponent::CheckInteractionInput() const
{
	// E키
	//return OwnerController->WasInputKeyJustPressed(EKeys::E);
	return bInteractedWithSwitch;
}

FText UTutorialComponent::GetTutorialMessage(ETutorialStep Step) const
{
	switch (Step)
	{
	case ETutorialStep::MouseLook:
		return FText::FromString(TEXT("Move mouse to look around"));
	case ETutorialStep::Movement:
		return FText::FromString(TEXT("Press W/A/S/D to walk"));
	case ETutorialStep::Sprint:
		return FText::FromString(TEXT("Press Shift to sprint"));
	case ETutorialStep::Jump:
		return FText::FromString(TEXT("Press Spacebar to jump"));
	case ETutorialStep::PickUp:
		return FText::FromString(TEXT("Left Click to pick up"));
	case ETutorialStep::GrabGun:
		return FText::FromString(TEXT("Right Click to grab things from far away"));
	case ETutorialStep::Interaction:
		return FText::FromString(TEXT("Press E to activate switch"));
	case ETutorialStep::Completed:
		return FText::FromString(TEXT("Tutorial finished!"));
	default:
		return FText::GetEmpty();
	}
}

void UTutorialComponent::OnObjectGrabbed(AActor* GrabbedObject)
{
	if (CurrentStep != ETutorialStep::GrabGun) return;

	if (GrabbedObject && GrabbedObject->IsA(Aluggage::StaticClass()))
	{
		bGrabbedLuggage = true;
	}
}

void UTutorialComponent::OnObjectPickedUp(AActor* PickedObject)
{
	if (CurrentStep != ETutorialStep::PickUp) return;

	if (PickedObject && PickedObject->IsA(Aluggage::StaticClass()))
	{
		bPickedUpSomething = true;
	}
}

void UTutorialComponent::OnObjectInteracted(AActor* InteractedObject)
{
	if (CurrentStep != ETutorialStep::Interaction) return;

	if (InteractedObject && InteractedObject->IsA(APedestalSwitch::StaticClass()))
	{
		bInteractedWithSwitch = true;
	}
}

