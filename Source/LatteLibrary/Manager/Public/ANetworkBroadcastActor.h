// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file ANetworkBroadcastActor.h
 * @brief 네트워크 복제를 위한 전역 브로드캐스트 Actor
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EDamageType.h"
#include "ANetworkBroadcastActor.generated.h"

/**
 * @brief 네트워크 복제를 위한 전역 브로드캐스트 Actor
 * @details GameMode에서 자동 생성되며, Server RPC와 Multicast RPC를 통해
 *          모든 클라이언트에게 이벤트를 전파합니다.
 *
 * @note UGameInstanceSubsystem은 RPC를 가질 수 없으므로 Actor로 구현되었습니다.
 *       이 Actor는 bAlwaysRelevant = true로 설정되어 모든 클라이언트에게 항상 복제됩니다.
 */
UCLASS()
class LATTELIBRARY_API ANetworkBroadcastActor : public AActor
{
	GENERATED_BODY()

public:
	ANetworkBroadcastActor();

	/**
	 * @brief 싱글톤 인스턴스 가져오기
	 * @param WorldContextObject World 컨텍스트
	 * @return NetworkBroadcastActor 인스턴스 (없으면 자동 생성)
	 */
	UFUNCTION(BlueprintCallable, Category = "Network Events", meta = (WorldContext = "WorldContextObject"))
	static ANetworkBroadcastActor* Get(const UObject* WorldContextObject);

	// ========================================
	// Door Message
	// ========================================

	/**
	 * @brief 문 상태 변경 메시지를 네트워크로 전송
	 * @param InDoorIndex 문 인덱스
	 * @param bOpen 열기 여부
	 * @param EventInstigator 이벤트를 발생시킨 Actor (권한 검증용)
	 */
	UFUNCTION(BlueprintCallable, Category = "Network Events")
	void SendDoorMessage(int InDoorIndex, bool bOpen, AActor* EventInstigator);

protected:
	UFUNCTION(Server, Reliable)
	void Server_SendDoorMessage(int InDoorIndex, bool bOpen, AActor* EventInstigator);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SendDoorMessage(int InDoorIndex, bool bOpen);

public:
	// ========================================
	// Weight Switch
	// ========================================

	/**
	 * @brief 무게 스위치 상태 변경 메시지를 네트워크로 전송
	 * @param InButtonIndex 버튼 인덱스
	 * @param bActive 활성화 여부
	 * @param EventInstigator 이벤트를 발생시킨 Actor (권한 검증용)
	 */
	UFUNCTION(BlueprintCallable, Category = "Network Events")
	void SendWeightSwitch(int InButtonIndex, bool bActive, AActor* EventInstigator);

protected:
	UFUNCTION(Server, Reliable)
	void Server_SendWeightSwitch(int InButtonIndex, bool bActive, AActor* EventInstigator);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SendWeightSwitch(int InButtonIndex, bool bActive);

public:
	// ========================================
	// HitStop
	// ========================================

	/**
	 * @brief 히트스톱 이벤트를 네트워크로 전송
	 * @param Target 타겟 Actor
	 * @param Type 데미지 타입
	 * @param EventInstigator 이벤트를 발생시킨 Actor (권한 검증용)
	 */
	UFUNCTION(BlueprintCallable, Category = "Network Events")
	void SendHitStop(AActor* Target, EDamageType Type, AActor* EventInstigator);

protected:
	UFUNCTION(Server, Reliable)
	void Server_SendHitStop(AActor* Target, EDamageType Type, AActor* EventInstigator);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SendHitStop(AActor* Target, EDamageType Type);

public:
	// ========================================
	// Knockback
	// ========================================

	/**
	 * @brief 넉백 이벤트를 네트워크로 전송
	 * @param Target 타겟 Actor
	 * @param KnockbackInstigator 넉백을 발생시킨 Actor
	 * @param Type 데미지 타입
	 * @param Resistance 저항력
	 * @param CallInstigator RPC 호출자 (권한 검증용)
	 */
	UFUNCTION(BlueprintCallable, Category = "Network Events")
	void SendKnockback(AActor* Target, AActor* KnockbackInstigator, EDamageType Type, float Resistance, AActor* CallInstigator);

protected:
	UFUNCTION(Server, Reliable)
	void Server_SendKnockback(AActor* Target, AActor* KnockbackInstigator, EDamageType Type, float Resistance, AActor* CallInstigator);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SendKnockback(AActor* Target, AActor* KnockbackInstigator, EDamageType Type, float Resistance);

public:
	// ========================================
	// Mission Timer State
	// ========================================

	/**
	 * @brief 미션 타이머 상태 변경 이벤트를 네트워크로 전송
	 * @param bIsActive 타이머 활성화 여부
	 * @param TimeLimit 타이머 제한 시간 (초 단위)
	 * @param EventInstigator 이벤트를 발생시킨 Actor (권한 검증용)
	 */
	UFUNCTION(BlueprintCallable, Category = "Network Events")
	void SendUpdateMissionTimerState(bool bIsActive, float TimeLimit, AActor* EventInstigator);

protected:
	UFUNCTION(Server, Reliable)
	void Server_SendUpdateMissionTimerState(bool bIsActive, float TimeLimit, AActor* EventInstigator);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SendUpdateMissionTimerState(bool bIsActive, float TimeLimit);

private:
	/**
	 * @brief 로컬 BroadcastManager 가져오기
	 * @return 현재 World의 UBroadcastManager 인스턴스
	 */
	class UBroadcastManager* GetLocalBroadcastManager() const;

	/**
	 * @brief 권한 검증 - EventInstigator가 유효한지 확인
	 * @param EventInstigator 검증할 Actor
	 * @return 유효하면 true
	 */
	bool ValidateInstigator(AActor* EventInstigator) const;

	/**
	 * @brief World별 싱글톤 인스턴스 맵
	 */
	static TMap<UWorld*, ANetworkBroadcastActor*> Instances;
};
