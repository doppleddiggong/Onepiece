// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file UInteractionSystem.cpp
 * @brief UInteractionSystem 구현
 */

#include "UInteractionSystem.h"

#include "APlayerActor.h"
#include "GameLogging.h"
#include "InteractableComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/PlayerController.h"

UInteractionSystem::UInteractionSystem()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = TG_PostUpdateWork;
}

void UInteractionSystem::BeginPlay()
{
	Super::BeginPlay();

	// Owner PlayerActor 캐싱
	OwnerPlayer = Cast<APlayerActor>(GetOwner());
	if (!OwnerPlayer)
	{
		PRINTLOG( TEXT("UInteractionSystem: Owner is not APlayerActor!"));
		SetComponentTickEnabled(false);
	}
}

void UInteractionSystem::TickComponent(float DeltaTime, ELevelTick TickType,
                                        FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 타겟 감지
	CurrentTarget = DetectInteractableTarget();

	// 디버그 표시
	if (bShowDebugInfo && CurrentTarget)
	{
		CurrentTarget->ShowDebugInfo(OwnerPlayer);
	}
}

void UInteractionSystem::TryInteract()
{
	if (!CurrentTarget || !CurrentTarget->bCanInteract)
		return;

	PRINT_STRING(TEXT("부엉 부엉 %s"), *OwnerPlayer->GetName());
	// 타입별 처리
	switch (CurrentTarget->InteractionType)
	{
		case EInteractionType::PickUp:
			PRINT_STRING(TEXT("부엉 부엉 %s"), *OwnerPlayer->GetName());
			TryPickUp();
			break;

		case EInteractionType::Button:
			PRINT_STRING(TEXT("부엉 부엉 %s"), *OwnerPlayer->GetName());
			CurrentTarget->TriggerInteraction(OwnerPlayer);
			break;

		default:
			break;
	}
}

void UInteractionSystem::TryPickUp()
{
	// 이미 들고 있으면 무시
	if (HoldingInteractable)
		return;

	// CurrentTarget = DetectInteractableTarget();
	
	// 타겟이 PickUp 타입인지 확인
	if (!CurrentTarget || CurrentTarget->InteractionType != EInteractionType::PickUp)
		return;

	// 유효성 검사 강화
	if (!IsValid(CurrentTarget))
	{
		PRINTLOG( TEXT("UInteractionSystem: CurrentTarget is invalid!"));
		CurrentTarget = nullptr;
		return;
	}

	AActor* TargetOwner = CurrentTarget->GetOwner();
	if (!TargetOwner || !IsValid(TargetOwner))
	{
		PRINTLOG( TEXT("UInteractionSystem: Target owner is null or invalid!"));
		return;
	}

	// 로그 출력 (PickUp 호출 전)
	PRINTLOG( TEXT("UInteractionSystem: Picking up %s"), *TargetOwner->GetName());

	HoldingInteractable = CurrentTarget;
	HoldingInteractable->PickUp(OwnerPlayer);
}

void UInteractionSystem::TryDrop()
{
	if (!HoldingInteractable)
		return;

	// 유효성 검사
	if (!IsValid(HoldingInteractable))
	{
		PRINTLOG( TEXT("UInteractionSystem: HoldingInteractable is invalid!"));
		HoldingInteractable = nullptr;
		return;
	}

	// 로그 출력 (Drop 호출 전)
	AActor* DroppedOwner = HoldingInteractable->GetOwner();
	if (DroppedOwner && IsValid(DroppedOwner))
	{
		PRINTLOG( TEXT("UInteractionSystem: Dropping %s"), *DroppedOwner->GetName());
	}
	
	HoldingInteractable->Drop();
	HoldingInteractable = nullptr;

	PRINTLOG( TEXT("UInteractionSystem: Drop complete"));
}

void UInteractionSystem::RegisterInteractable(UInteractableComponent* Interactable)
{
	if (!Interactable || !IsValid(Interactable))
	{
		PRINTLOG( TEXT("UInteractionSystem: Attempted to register null/invalid interactable"));
		return;
	}

	if (NearbyInteractables.Contains(Interactable))
		return;

	NearbyInteractables.Add(Interactable);
}

void UInteractionSystem::UnregisterInteractable(UInteractableComponent* Interactable)
{
	if (!Interactable)
		return;

	NearbyInteractables.Remove(Interactable);
	
	// 현재 타겟이었다면 해제
	if (CurrentTarget == Interactable)
	{
		CurrentTarget = nullptr;
	}
}

USceneComponent* UInteractionSystem::GetHoldPosition() const
{
	return OwnerPlayer ? OwnerPlayer->HoldPosition : nullptr;
}

UInteractableComponent* UInteractionSystem::DetectInteractableTarget()
{
	// 근처에 상호작용 가능한 객체가 없으면 nullptr
	if (NearbyInteractables.Num() == 0)
		return nullptr;

	// LineTrace 실행
	FHitResult HitResult;
	if (!PerformCenterLineTrace(HitResult))
		return nullptr;

	// Hit된 액터에서 InteractableComponent 찾기
	AActor* HitActor = HitResult.GetActor();
	if (!HitActor)
		return nullptr;

	UInteractableComponent* InteractComp = HitActor->FindComponentByClass<UInteractableComponent>();
	if (!InteractComp)
		return nullptr;

	// NearbyInteractables에 포함되어 있는지 확인 (범위 내인지 검증)
	if (NearbyInteractables.Contains(InteractComp))
	{
		return InteractComp;
	}

	return nullptr;
}

bool UInteractionSystem::PerformCenterLineTrace(FHitResult& OutHit)
{
	if (!OwnerPlayer)
		return false;

	// 플레이어 컨트롤러 가져오기
	APlayerController* PC = Cast<APlayerController>(OwnerPlayer->GetController());
	if (!PC)
		return false;

	APlayerCameraManager* CameraManager = PC->PlayerCameraManager;
	if (!CameraManager)
		return false;

	FVector CameraLocation = CameraManager->GetCameraLocation();
	FVector CameraForward = CameraManager->GetCameraRotation().Vector();

	// 레이 트레이스 시작/끝 지점                                                                                                                                                    
	FVector TraceStart = CameraLocation;
	FVector TraceEnd = TraceStart + (CameraForward * InteractionDistance);

	// Ray trace 실행
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		OutHit, TraceStart, TraceEnd, ECC_Visibility
	);

	// 디버그 라인
	if (bShowDebugInfo)
	{
		DrawDebugLine(
			GetWorld(), TraceStart, TraceEnd,
			bHit ? FColor::Green : FColor::Red,
			false, 0.0f, 0, 0.5f
		);
	}

	return bHit;
}