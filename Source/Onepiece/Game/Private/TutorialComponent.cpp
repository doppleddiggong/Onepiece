// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "TutorialComponent.h"

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
	// 현재 메시지 숨김
	if (UBroadcastManager* BM = UBroadcastManager::Get(GetWorld()))
	{
		BM->SendHideTutorialMessage();
	}
	
	ETutorialStep NextStep = GetNextStep(CurrentStep);
	SetStep(NextStep);
}

void UTutorialComponent::SetStep(ETutorialStep NewStep)
{
	CurrentStep = NewStep;
	bInputConditionMet = false; // 플래그 초기화

	if (UBroadcastManager* BM = UBroadcastManager::Get(GetWorld()))
	{
		BM->SendTutorialStepChanged(OwnerController, NewStep);

		FText TutorialMessage = GetTutorialMessage(NewStep);
		BM->SendShowTutorialMessage(TutorialMessage);
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
		&UTutorialComponent::AdvanceToNextStep, 2.f, false);
}

ETutorialStep UTutorialComponent::GetNextStep(ETutorialStep Step) const
{
	switch (Step)
	{
		case ETutorialStep::Waiting:		return ETutorialStep::MouseLook;
		case ETutorialStep::MouseLook:		return ETutorialStep::Movement;
		case ETutorialStep::Movement:		return ETutorialStep::Sprint;
		case ETutorialStep::Sprint:			return ETutorialStep::Jump;
		case ETutorialStep::Jump:			return ETutorialStep::PickUp;
		case ETutorialStep::PickUp:			return ETutorialStep::GrabGun;
		case ETutorialStep::GrabGun:		return ETutorialStep::Interaction;
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

	APawn* Pawn = OwnerController->GetPawn();
	if (!Pawn) return false;

	// 점프중인지
	if (ACharacter* Character = Cast<ACharacter>(Pawn))
	{
		return Character->bPressedJump;
	}
	return false;
}

bool UTutorialComponent::CheckPickUpInput() const
{
	if (!OwnerController) return false;
	// 왼쪽 마우스 버튼 클릭
	return OwnerController->WasInputKeyJustPressed(EKeys::LeftMouseButton);
}

bool UTutorialComponent::CheckGrabGunInput() const
{
	if (!OwnerController) return false;
	// 오른쪽 마우스 버튼 클릭
	return OwnerController->WasInputKeyJustPressed(EKeys::RightMouseButton);
}

bool UTutorialComponent::CheckInteractionInput() const
{
	if (!OwnerController) return false;
	// E키
	return OwnerController->WasInputKeyJustPressed(EKeys::E);
}

FText UTutorialComponent::GetTutorialMessage(ETutorialStep Step) const
{
	switch (Step)
	{
	case ETutorialStep::MouseLook:
		return FText::FromString(TEXT("마우스를 움직여 주변을 둘러보세요"));
	case ETutorialStep::Movement:
		return FText::FromString(TEXT("WASD 키로 이동해보세요"));
	case ETutorialStep::Sprint:
		return FText::FromString(TEXT("Shift 키를 눌러 달려보세요"));
	case ETutorialStep::Jump:
		return FText::FromString(TEXT("스페이스바를 눌러 점프해보세요"));
	case ETutorialStep::PickUp:
		return FText::FromString(TEXT("왼쪽 마우스 버튼으로 물건을 집어보세요"));
	case ETutorialStep::GrabGun:
		return FText::FromString(TEXT("오른쪽 마우스 버튼으로 멀리 있는 물건을 당겨보세요"));
	case ETutorialStep::Interaction:
		return FText::FromString(TEXT("E 키를 눌러 상호작용해보세요"));
	case ETutorialStep::Completed:
		return FText::FromString(TEXT("튜토리얼 완료!"));
	default:
		return FText::GetEmpty();
	}
}

