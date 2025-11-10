// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file UKnockbackSystem.cpp
 * @brief FMath의 동작을 구현합니다.
 */

#include "UKnockbackSystem.h"

#include "APlayerActor.h"
#include "UGameDataManager.h"
#include "FKnockbackData.h"
#include "TimerManager.h"
#include "UBroadcastManager.h"

#include "GameFramework/CharacterMovementComponent.h"

static FORCEINLINE float Clamp01(float X){ return FMath::Clamp(X, 0.f, 1.f); }

UKnockbackSystem::UKnockbackSystem()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UKnockbackSystem::BeginPlay()
{
    Super::BeginPlay();
}

void UKnockbackSystem::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    RestoreMovement();
    GetWorld()->GetTimerManager().ClearTimer(FlyingTimer);
    Super::EndPlay(EndPlayReason);
}

void UKnockbackSystem::InitSystem(APlayerActor* InOwner)
{
    this->Owner = InOwner;
	
    MeshComp = Owner->GetMesh();
    MoveComp = Owner->GetCharacterMovement();

	if (auto EventManager = UBroadcastManager::Get(this))
	{
		EventManager->OnKnockback.AddDynamic(this, &UKnockbackSystem::OnKnockback);
	}
}

void UKnockbackSystem::OnKnockback( AActor* Target, AActor* Instigator, EDamageType Type, float Resistance)
{
	if ( Owner != Target )
		return;

	Knockback(Target, Instigator, Type, Resistance);
}

FVector UKnockbackSystem::ComputeKnockDir(const AActor* Target, const AActor* Instigator)
{
    FVector Dir = FVector::ZeroVector;

    if (Target && Instigator)
        Dir = Target->GetActorLocation() - Instigator->GetActorLocation();

    if (!Dir.Normalize())
    {
        if (Target)
        {
            FVector Fwd = Target->GetActorForwardVector();
            Dir = (-Fwd).GetSafeNormal();
        }
        else
        {
            Dir = FVector::ForwardVector;
        }
    }

    return Dir;
}

void UKnockbackSystem::Knockback(AActor* Target, AActor* Instigator, EDamageType Type, float Resistance)
{
    FKnockbackData Params;
    if (auto DataManager = UGameDataManager::Get(GetWorld()))
    {
	    if (!DataManager->GetKnockbackData(Type, Params))
	    	return;
    }

    const FVector Dir = ComputeKnockDir(Target, Instigator);

    if (MoveComp->MovementMode != MOVE_Falling)
        MoveComp->SetMovementMode(MOVE_Falling);
	
	if (!bFriction)
	{
		PrevBrakingFriction = MoveComp->BrakingFrictionFactor;
		bFriction = true;
	}
	MoveComp->BrakingFrictionFactor = FMath::Clamp(Params.BrakingFrictionFactor, 0.f, 1.f);

    const float Power = FMath::Max(0.f, Params.KnockbackPower) * (1.f - Clamp01(Resistance));
    
    FVector Launch = Dir * Power;
    Launch.Z += Params.UpPower; // Always apply UpPower for testing

    Owner->LaunchCharacter(Launch, true, true);

    GetWorld()->GetTimerManager().SetTimer(
        RestoreTimer, this, &UKnockbackSystem::RestoreMovement,
        FMath::Max(0.f, Params.Duration), false
    );

    if (Params.bAfterFlying)
    {
        GetWorld()->GetTimerManager().ClearTimer(FlyingTimer);
        GetWorld()->GetTimerManager().SetTimer(
            FlyingTimer, this, &UKnockbackSystem::EnterFlying,
            FMath::Max(0.f, Params.FlyingDelay), false
        );
    }
}

void UKnockbackSystem::RestoreMovement()
{
	if (bFriction)
	{
		MoveComp->BrakingFrictionFactor = PrevBrakingFriction;
		bFriction = false;
	}
}

void UKnockbackSystem::EnterFlying()
{
	Owner->SetFlying();
}
