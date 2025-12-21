// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file UBroadcastManger.h
 * @brief UBroadcastManger 선언에 대한 Doxygen 주석을 제공합니다.
 */
#pragma once

#include "CoreMinimal.h"
#include "Macro.h"
#include "EDamageType.h"
#include "EQuestRole.h"
#include "ETutorialStep.h" 
#include "FResultStatData.h"
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

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAudioSpectrum, float, Spectrum);
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnAudioSpectrum OnAudioSpectrum;
	
	UFUNCTION(BlueprintCallable, Category="Events")
	void SendAudioSpectrum(float Spectrum);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAudioCapture, bool, bRecording);
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnAudioCapture OnAudioCapture;

	UFUNCTION(BlueprintCallable, Category="Events")
	void SendAudioCapture(bool bRecording);



	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnDoorMessage, int, DoorIndex, bool, Open, AActor*, EventInstigator);
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnDoorMessage OnDoorMessage;

	UFUNCTION(BlueprintCallable, Category="Events")
	void SendDoorMessage(int InDoorIndex, bool InOpen, AActor* EventInstigator);


	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWeightSwitch, int, ButtonIndex, bool, bIsActive);
	UPROPERTY(BlueprintAssignable, Category="Events")
	FWeightSwitch OnWeightSwitch;

	UFUNCTION(BlueprintCallable, Category="Events")
	void SendWeightSwitch(int InButtonIndex, bool InActive);


	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUpdateMissionTimerState, bool, bIsActive, float, TimeLimit);
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnUpdateMissionTimerState OnUpdateMissionTimerState;

	UFUNCTION(BlueprintCallable, Category="Events")
	void SendUpdateMissionTimerState(bool bIsActive, float TimeLimit);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpdateQuestRole, EQuestRole, QuestRole);
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnUpdateQuestRole OnUpdateQuestRole;

	UFUNCTION(BlueprintCallable, Category="Events")
	void SendUpdateQuestRole(EQuestRole QuestRole);
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMovie, int, Group, bool, State);
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnMovie OnMovie;

	UFUNCTION(BlueprintCallable, Category="Events")
	void SendMovie(const int& InGroup, bool InState);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTutorMessage, const FText&, Message);
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnTutorMessage OnTutorMessage;

	UFUNCTION(BlueprintCallable, Category="Events")
	void SendTutorMessage(const FText& Message);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAddItemToBoxList, const TArray<FResultStatData>&, ItemDataList);
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnAddItemToBoxList OnAddItemToBoxList;

	UFUNCTION(BlueprintCallable, Category="Events")
	void SendAddItemToBoxList(const TArray<FResultStatData>& ItemDataList);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTeleportAllPlayers, FVector, TargetLocation);
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnTeleportAllPlayers OnTeleport;

	UFUNCTION(BlueprintCallable, Category="Events")
	void SendTeleport(const FVector& TargetLocation);

	// Tutorial
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTutorialStepChanged,
		APlayerController*, Player, ETutorialStep, NewStep);

	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnTutorialStepChanged OnTutorialStepChanged;

	UFUNCTION(BlueprintCallable, Category="Events")
	void SendTutorialStepChanged(APlayerController* Player, ETutorialStep NewStep);

	// Tutorial Message 표시 (자동숨김 없는 버전)
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShowTutorialMessage, const FText&, Message);
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnShowTutorialMessage OnShowTutorialMessage;

	UFUNCTION(BlueprintCallable, Category="Events")
	void SendShowTutorialMessage(const FText& Message);

	// Tutorial Message 숨김
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHideTutorialMessage);
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnHideTutorialMessage OnHideTutorialMessage;

	UFUNCTION(BlueprintCallable, Category="Events")
	void SendHideTutorialMessage();

	
};
