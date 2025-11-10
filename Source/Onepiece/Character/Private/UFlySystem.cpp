// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file UFlySystem.cpp
 * @brief UFlySystem의 동작을 구현합니다.
 */

#include "UFlySystem.h"

#include "APlayerActor.h"
#include "FEaseHelper.h"
#include "UEaseFunctionLibrary.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"


UFlySystem::UFlySystem()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UFlySystem::BeginPlay()
{
	Super::BeginPlay();
}

void UFlySystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    UpstreamTick(DeltaTime);
    DownstreamTick(DeltaTime);

	if ( IsValid(MoveComp ))
	{
		if ( !bIsUpstream && !bIsDownstream &&
			MoveComp->MovementMode == MOVE_Flying )
		{
			const FVector AltInput = AltitudeTick(DeltaTime);
			const FVector TotalInput = AltInput;

			if (!TotalInput.IsNearlyZero())
			{
				MoveComp->AddInputVector(TotalInput);
			}
		}
	}
}

void UFlySystem::InitSystem(APlayerActor* InOwner, FEndCallback InCallback)
{
	this->Owner = InOwner;
	this->Callback = InCallback;

	this->MoveComp = Owner->GetCharacterMovement();

	// 브레이킹 기반 감속 설정(무입력시 자연 감속)
	MoveComp->BrakingDecelerationFlying = BrakingDecelerationZ;
	MoveComp->BrakingFriction = BrakingFriction;
}

void UFlySystem::OnAltitudePress(const bool bIsUp)
{
	bAltitudeInput = true;
	bIsAltitudeUp = bIsUp;

	if ( bIsUp == true &&
		 MoveComp->MovementMode != MOVE_Flying )
	{
		Owner->SetFlying();
	}
}

void UFlySystem::OnAltitudeRelease()
{
	bAltitudeInput = false;
}

void UFlySystem::OnJump()
{
	JumpCount++;
	if (JumpCount > 3)
		return;
	
	switch (JumpCount)
	{
	case 1:
		{
			Owner->Jump();
		}
		break;

	case 2:
		{
			this->ActivateUpstream();
			Owner->SetFlying();
		}
		break;

	case 3:
		{
			FHitResult HitResult;
			this->OnLand(HitResult);
		}
		break;
	}
}

void UFlySystem::OnLand(const FHitResult& Hit)
{
	Owner->SetFallingToWalk();
	this->ActivateDownstream();

	JumpCount = 0;
}


void UFlySystem::ActivateUpstream()
{
	ElapsedTime = 0.0f;
	bIsUpstream = true;

	StartLocation = Owner->GetActorLocation();
	EndLocation = StartLocation + FVector(0,0, UpstreamHeight);
}

void UFlySystem::ActivateDownstream()
{
	ElapsedTime = 0.0f;
	bIsDownstream = true;

	StartLocation = Owner->GetActorLocation();
	FVector TempEndPos = StartLocation - FVector(0,0, UpstreamHeight*3);

	FHitResult HitInfo;

	FCollisionQueryParams CollisionParam;
	CollisionParam.AddIgnoredActor(Owner);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitInfo,
		StartLocation, TempEndPos,
		ECC_Visibility, CollisionParam);

	if ( bHit)
		{
			const float CapsuleHalfHeight = Owner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
			EndLocation = HitInfo.Location + FVector(0, 0, CapsuleHalfHeight);
		}
	else
	{
		EndLocation = TempEndPos;
	}
}

void UFlySystem::UpstreamTick(const float DeltaTime)
{
	if ( !bIsUpstream )
		return;

	ElapsedTime += DeltaTime;
	
	FVector ResultLocation = UEaseFunctionLibrary::LerpVectorEase(
		StartLocation, EndLocation,
		ElapsedTime / UpstreamDuration,
		EEaseType::EaseOutQuart);

	Owner->SetActorLocation(ResultLocation, true);

	float Dist = FVector::Dist(  Owner->GetActorLocation(), EndLocation );
	if ( Dist < AlmostDist || ElapsedTime > UpstreamDuration )
	{
		bIsUpstream = false;
		Owner->SetActorLocation(EndLocation, true);
	}
}

void UFlySystem::DownstreamTick(const float DeltaTime)
{
	if ( !bIsDownstream )
		return;

	ElapsedTime += DeltaTime;
	
	FVector ResultLocation = UEaseFunctionLibrary::LerpVectorEase(
		StartLocation, EndLocation,
		ElapsedTime / DownstreamDuration,
		EEaseType::EaseOutQuart);

	Owner->SetActorLocation(ResultLocation, true);

	float Dist = FVector::Dist( Owner->GetActorLocation(), EndLocation );
	if ( Dist < AlmostDist || ElapsedTime > DownstreamDuration )
	{
		bIsDownstream = false;
		Owner->SetActorLocation(EndLocation, true);
		
		Callback.ExecuteIfBound();
	}
}

FVector UFlySystem::AltitudeTick(const float DeltaTime)
{
    if (bIsUpstream || bIsDownstream || !bAltitudeInput)
        return FVector::ZeroVector;

    if ( MoveComp->MovementMode == MOVE_Flying)
    {
        CurrentFlySpeed += Acceleration * DeltaTime;
        CurrentFlySpeed = FMath::Clamp(CurrentFlySpeed, FlySpeedMin, FlySpeedMax);
        const FVector Direction = bIsAltitudeUp ? FVector::UpVector : FVector::DownVector;

        if (bIsAltitudeUp)
        {
        	// 고도 제한
            if (Owner->GetActorLocation().Z < MaxFlightHeight)
                return Direction * CurrentFlySpeed * DeltaTime;
        }
        else
        {
        	// 하강일 경우, 지면 근처까지 오면 낙하 중단
        	FVector Start = Owner->GetActorLocation();
        	FVector End = Start - FVector(0.f, 0.f, 88*1.5f); // 캐릭터 크기가 88 이니깐 적당히...1.5밴

        	FHitResult HitResult;
        	FCollisionQueryParams Params;
        	Params.AddIgnoredActor(Owner); // 본인 제외

        	bool bHit = GetWorld()->LineTraceSingleByChannel(
				HitResult,
				Start, End, ECC_Visibility,Params
			);

        	if (bHit)
        	{
        		// 지면에 가까워졌으면 낙하 중단
        		Owner->SetFallingToWalk();
        		return FVector::ZeroVector;
        	}

        	return Direction * CurrentFlySpeed * DeltaTime;
        }
    }

    return FVector::ZeroVector;
}
