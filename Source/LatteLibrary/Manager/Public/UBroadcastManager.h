// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file UBroadcastManger.h
 * @brief UBroadcastManger 선언에 대한 Doxygen 주석을 제공합니다.
 */
#pragma once

#include "CoreMinimal.h"
#include "Macro.h"
#include "EDamageType.h"

#include "Subsystems/GameInstanceSubsystem.h"
#include "UBroadcastManager.generated.h"

/**
 * @brief 게임 내 전역 이벤트를 중계하는 중앙 이벤트 버스(Event Bus) 서브시스템입니다.
 * @details 이 매니저는 델리게이트를 사용하여 시스템 간의 직접적인 종속성을 제거하고,
 *          느슨한 결합(Loose Coupling)을 통해 유연하고 확장 가능한 아키텍처를 지원합니다.
 *          각 서브시스템은 이 매니저를 통해 이벤트를 보내거나(Send) 구독(Bind)할 수 있습니다.
 */
UCLASS()
class LATTELIBRARY_API UBroadcastManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	DEFINE_SUBSYSTEM_GETTER_INLINE(UBroadcastManager);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMessage, FString, Msg);
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnMessage OnMessage;
	
	UFUNCTION(BlueprintCallable, Category="Events")
	void SendMessage(const FString& InMsg);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNetworkWaitCount, int, RequestCount);
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnNetworkWaitCount OnNetworkWaitCount;

	UFUNCTION(BlueprintCallable, Category="Events")
	void SendNetworkWaitCount(int RequestCount);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHitStop, AActor*, Target, EDamageType, Type);
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnHitStop OnHitStop;

	UFUNCTION(BlueprintCallable, Category="Events")
	void SendHitStop(AActor* Target, const EDamageType Type);

	UFUNCTION(BlueprintCallable, Category="Events")
	void SendHitStopPair(AActor* Attacker, const EDamageType AttackerType,
						 AActor* Target,   const EDamageType TargetType);

	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnKnockback,
		AActor*,		   Target,
		AActor*,           Instigator,
		EDamageType,  Type,
		float,             Resistance );
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnKnockback OnKnockback;

	UFUNCTION(BlueprintCallable, Category="Events")
	void SendKnockback(AActor* Target, AActor* Instigator, EDamageType Type, float Resistance);
};
