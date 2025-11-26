// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file ANetworkBroadcastActor.cpp
 * @brief 네트워크 복제를 위한 전역 브로드캐스트 Actor 구현
 */

#include "ANetworkBroadcastActor.h"
#include "UBroadcastManager.h"
#include "GameLogging.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"

TMap<UWorld*, ANetworkBroadcastActor*> ANetworkBroadcastActor::Instances;

ANetworkBroadcastActor::ANetworkBroadcastActor()
{
	PrimaryActorTick.bCanEverTick = false;

	// 네트워크 복제 설정
	bReplicates = true;
	bAlwaysRelevant = true;  // 모든 클라이언트에게 항상 복제

	// Root component
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

ANetworkBroadcastActor* ANetworkBroadcastActor::Get(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		PRINTLOG(TEXT("NetworkBroadcastActor::Get - WorldContextObject is null"));
		return nullptr;
	}

	UWorld* World = WorldContextObject->GetWorld();
	if (!World)
	{
		PRINTLOG(TEXT("NetworkBroadcastActor::Get - World is null"));
		return nullptr;
	}

	// 이미 존재하는 인스턴스 확인
	if (Instances.Contains(World))
	{
		ANetworkBroadcastActor* Instance = Instances[World];
		if (IsValid(Instance))
		{
			return Instance;
		}
		else
		{
			// 유효하지 않은 인스턴스 제거
			Instances.Remove(World);
		}
	}

	// 새 인스턴스 생성 (Server에서만)
	if (World->GetAuthGameMode())
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = nullptr;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		ANetworkBroadcastActor* NewInstance = World->SpawnActor<ANetworkBroadcastActor>(
			ANetworkBroadcastActor::StaticClass(),
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			SpawnParams
		);

		if (NewInstance)
		{
			Instances.Add(World, NewInstance);
			PRINTLOG(TEXT("NetworkBroadcastActor: Created new instance"));
			return NewInstance;
		}
		else
		{
			PRINTLOG(TEXT("NetworkBroadcastActor: Failed to spawn actor"));
		}
	}
	else
	{
		PRINTLOG(TEXT("NetworkBroadcastActor: Not on server, cannot create instance"));
	}

	return nullptr;
}

// ========================================
// Door Message
// ========================================

void ANetworkBroadcastActor::SendDoorMessage(int InDoorIndex, bool bOpen, AActor* EventInstigator)
{
	if (!EventInstigator)
	{
		PRINTLOG(TEXT("NetworkBroadcastActor: SendDoorMessage - EventInstigator is null"));
		return;
	}

	PRINTLOG(TEXT("NetworkBroadcastActor: SendDoorMessage called - Index: %d, Open: %d, EventInstigator: %s"),
		InDoorIndex, bOpen, *EventInstigator->GetName());

	Server_SendDoorMessage(InDoorIndex, bOpen, EventInstigator);
}

void ANetworkBroadcastActor::Server_SendDoorMessage_Implementation(int InDoorIndex, bool bOpen, AActor* EventInstigator)
{
	if (!ValidateInstigator(EventInstigator))
	{
		PRINTLOG(TEXT("NetworkBroadcastActor: Invalid instigator for DoorMessage"));
		return;
	}

	PRINTLOG(TEXT("NetworkBroadcastActor: Server received DoorMessage - Index: %d, Open: %d"), InDoorIndex, bOpen);

	// Multicast로 모든 클라이언트에게 전파
	Multicast_SendDoorMessage(InDoorIndex, bOpen);
}

void ANetworkBroadcastActor::Multicast_SendDoorMessage_Implementation(int InDoorIndex, bool bOpen)
{
	PRINTLOG(TEXT("NetworkBroadcastActor: Multicast DoorMessage - Index: %d, Open: %d, Role: %s"),
		InDoorIndex, bOpen, GetLocalRole() == ROLE_Authority ? TEXT("Server") : TEXT("Client"));

	UBroadcastManager* LocalBroadcast = GetLocalBroadcastManager();
	if (LocalBroadcast)
	{
		LocalBroadcast->SendDoorMessage(InDoorIndex, bOpen);
		PRINTLOG(TEXT("NetworkBroadcastActor: Local BroadcastManager triggered for DoorMessage"));
	}
	else
	{
		PRINTLOG(TEXT("NetworkBroadcastActor: Failed to get local BroadcastManager"));
	}
}

// ========================================
// Weight Switch
// ========================================

void ANetworkBroadcastActor::SendWeightSwitch(int InButtonIndex, bool bActive, AActor* EventInstigator)
{
	if (!EventInstigator)
	{
		PRINTLOG(TEXT("NetworkBroadcastActor: SendWeightSwitch - EventInstigator is null"));
		return;
	}

	PRINTLOG(TEXT("NetworkBroadcastActor: SendWeightSwitch called - Index: %d, Active: %d"), InButtonIndex, bActive);

	Server_SendWeightSwitch(InButtonIndex, bActive, EventInstigator);
}

void ANetworkBroadcastActor::Server_SendWeightSwitch_Implementation(int InButtonIndex, bool bActive, AActor* EventInstigator)
{
	if (!ValidateInstigator(EventInstigator))
	{
		PRINTLOG(TEXT("NetworkBroadcastActor: Invalid instigator for WeightSwitch"));
		return;
	}

	PRINTLOG(TEXT("NetworkBroadcastActor: Server received WeightSwitch - Index: %d, Active: %d"), InButtonIndex, bActive);

	Multicast_SendWeightSwitch(InButtonIndex, bActive);
}

void ANetworkBroadcastActor::Multicast_SendWeightSwitch_Implementation(int InButtonIndex, bool bActive)
{
	PRINTLOG(TEXT("NetworkBroadcastActor: Multicast WeightSwitch - Index: %d, Active: %d, Role: %s"),
		InButtonIndex, bActive, GetLocalRole() == ROLE_Authority ? TEXT("Server") : TEXT("Client"));

	UBroadcastManager* LocalBroadcast = GetLocalBroadcastManager();
	if (LocalBroadcast)
	{
		LocalBroadcast->SendWeightSwitch(InButtonIndex, bActive);
		PRINTLOG(TEXT("NetworkBroadcastActor: Local BroadcastManager triggered for WeightSwitch"));
	}
}

// ========================================
// HitStop
// ========================================

void ANetworkBroadcastActor::SendHitStop(AActor* Target, EDamageType Type, AActor* EventInstigator)
{
	if (!EventInstigator || !Target)
	{
		PRINTLOG(TEXT("NetworkBroadcastActor: SendHitStop - Invalid parameters"));
		return;
	}

	Server_SendHitStop(Target, Type, EventInstigator);
}

void ANetworkBroadcastActor::Server_SendHitStop_Implementation(AActor* Target, EDamageType Type, AActor* EventInstigator)
{
	if (!ValidateInstigator(EventInstigator) || !Target)
	{
		PRINTLOG(TEXT("NetworkBroadcastActor: Invalid parameters for HitStop"));
		return;
	}

	Multicast_SendHitStop(Target, Type);
}

void ANetworkBroadcastActor::Multicast_SendHitStop_Implementation(AActor* Target, EDamageType Type)
{
	UBroadcastManager* LocalBroadcast = GetLocalBroadcastManager();
	if (LocalBroadcast && Target)
	{
		LocalBroadcast->SendHitStop(Target, Type);
	}
}

// ========================================
// Knockback
// ========================================

void ANetworkBroadcastActor::SendKnockback(AActor* Target, AActor* KnockbackInstigator, EDamageType Type, float Resistance, AActor* CallInstigator)
{
	if (!CallInstigator || !Target)
	{
		PRINTLOG(TEXT("NetworkBroadcastActor: SendKnockback - Invalid parameters"));
		return;
	}

	Server_SendKnockback(Target, KnockbackInstigator, Type, Resistance, CallInstigator);
}

void ANetworkBroadcastActor::Server_SendKnockback_Implementation(AActor* Target, AActor* KnockbackInstigator, EDamageType Type, float Resistance, AActor* CallInstigator)
{
	if (!ValidateInstigator(CallInstigator) || !Target)
	{
		PRINTLOG(TEXT("NetworkBroadcastActor: Invalid parameters for Knockback"));
		return;
	}

	Multicast_SendKnockback(Target, KnockbackInstigator, Type, Resistance);
}

void ANetworkBroadcastActor::Multicast_SendKnockback_Implementation(AActor* Target, AActor* KnockbackInstigator, EDamageType Type, float Resistance)
{
	UBroadcastManager* LocalBroadcast = GetLocalBroadcastManager();
	if (LocalBroadcast && Target)
	{
		LocalBroadcast->SendKnockback(Target, KnockbackInstigator, Type, Resistance);
	}
}

// ========================================
// Stage Started
// ========================================

void ANetworkBroadcastActor::SendStageStarted(int StageIndex, AActor* EventInstigator)
{
	if (!EventInstigator)
	{
		PRINTLOG(TEXT("NetworkBroadcastActor: SendStageStarted - EventInstigator is null"));
		return;
	}

	PRINTLOG(TEXT("NetworkBroadcastActor: SendStageStarted called - StageIndex: %d"), StageIndex);

	Server_SendStageStarted(StageIndex, EventInstigator);
}

void ANetworkBroadcastActor::Server_SendStageStarted_Implementation(int StageIndex, AActor* EventInstigator)
{
	if (!ValidateInstigator(EventInstigator))
	{
		PRINTLOG(TEXT("NetworkBroadcastActor: Invalid instigator for StageStarted"));
		return;
	}

	PRINTLOG(TEXT("NetworkBroadcastActor: Server received StageStarted - StageIndex: %d"), StageIndex);

	Multicast_SendStageStarted(StageIndex);
}

void ANetworkBroadcastActor::Multicast_SendStageStarted_Implementation(int StageIndex)
{
	PRINTLOG(TEXT("NetworkBroadcastActor: Multicast StageStarted - StageIndex: %d, Role: %s"),
		StageIndex, GetLocalRole() == ROLE_Authority ? TEXT("Server") : TEXT("Client"));

	UBroadcastManager* LocalBroadcast = GetLocalBroadcastManager();
	if (LocalBroadcast)
	{
		LocalBroadcast->SendStageStarted(StageIndex);
		PRINTLOG(TEXT("NetworkBroadcastActor: Local BroadcastManager triggered for StageStarted"));
	}
}

// ========================================
// Utility Functions
// ========================================

UBroadcastManager* ANetworkBroadcastActor::GetLocalBroadcastManager() const
{
	if (!GetWorld())
	{
		PRINTLOG(TEXT("NetworkBroadcastActor: GetWorld() returned nullptr"));
		return nullptr;
	}

	UBroadcastManager* LocalBroadcast = UBroadcastManager::Get(GetWorld());
	if (!LocalBroadcast)
	{
		PRINTLOG(TEXT("NetworkBroadcastActor: Failed to get UBroadcastManager"));
	}

	return LocalBroadcast;
}

bool ANetworkBroadcastActor::ValidateInstigator(AActor* EventInstigator) const
{
	if (!EventInstigator || !IsValid(EventInstigator))
	{
		PRINTLOG(TEXT("NetworkBroadcastActor: EventInstigator is null or invalid"));
		return false;
	}

	if (!EventInstigator->GetWorld())
	{
		PRINTLOG(TEXT("NetworkBroadcastActor: EventInstigator not in world"));
		return false;
	}

	return true;
}
