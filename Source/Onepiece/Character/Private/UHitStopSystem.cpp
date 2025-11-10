// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file UHitStopSystem.cpp
 * @brief UHitStopSystem의 동작을 구현합니다.
 */

#include "UHitStopSystem.h"

#include "GameFramework/Character.h"
#include "UBroadcastManager.h"
#include "UGameDataManager.h"
#include "GameFramework/CharacterMovementComponent.h"

UHitStopSystem::UHitStopSystem()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bTickEvenWhenPaused = true;
}

void UHitStopSystem::BeginPlay()
{
	Super::BeginPlay();
}

void UHitStopSystem::InitSystem(ACharacter* InOwner )
{
	this->Owner = InOwner;
	
	MeshComp = Owner->GetMesh();
	MoveComp = Owner->GetCharacterMovement();

	if (auto EventManager = UBroadcastManager::Get(this))
	{
		EventManager->OnHitStop.AddDynamic(this, &UHitStopSystem::OnHitStop);
	}
}

void UHitStopSystem::TickComponent(float DeltaTime, ELevelTick TickType,
									  FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if ( !bActive )
		return;

	const double Now = GetWorld()->GetRealTimeSeconds();
	if (Now >= EndRealTimeSeconds)
		EndFreeze();
}

void UHitStopSystem::OnHitStop(AActor* Target, const EDamageType Type)
{
	if (Target != Owner)
		return;

	ApplyHitStop(Type);
}

void UHitStopSystem::ApplyHitStop(const EDamageType Type)
{
	const double Now = GetWorld()->GetRealTimeSeconds();

	FHitStopData Params;
	if ( UGameDataManager::Get(GetWorld())->GetHitStopData(Type, Params) == false )
		return;
	
	if (!bActive)
	{
		BeginFreeze(Params);
		EndRealTimeSeconds = Now + Params.Duration;
		LastParams = Params;
		return;
	}

	const bool Stronger = Params.TimeDilation < LastParams.TimeDilation;
	const bool Longer   = (Now + Params.Duration) > EndRealTimeSeconds;

	if (Params.bRefreshIfStronger ? (Stronger || Longer) : Longer)
	{
		if (Stronger)
			Owner->CustomTimeDilation = FMath::Clamp(Params.TimeDilation, 0.001f, 1.0f);

		EndRealTimeSeconds = Now + Params.Duration;
		LastParams = Params;
	}
}

void UHitStopSystem::BeginFreeze(const FHitStopData& Params)
{
	bActive = true;

	SavedCustomTimeDilation = Owner->CustomTimeDilation;

	Owner->CustomTimeDilation = FMath::Clamp(Params.TimeDilation, 0.001f, 1.0f);
	MoveComp->StopMovementImmediately();
}

void UHitStopSystem::EndFreeze()
{
	Owner->CustomTimeDilation = SavedCustomTimeDilation;
	bActive = false;
}