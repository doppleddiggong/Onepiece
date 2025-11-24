// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "ALingoPlayerState.h"
#include "NetworkData.h"
#include "ALingoGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhaseChanged, EGamePhase, NewPhase);

/**
 * 
 */
UCLASS()
class ONEPIECE_API ALingoGameState : public AGameState
{
	GENERATED_BODY()

public:
	ALingoGameState();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	// Game state
	UPROPERTY(ReplicatedUsing=OnRep_CurrentPhase, BlueprintReadOnly, Category = "Game")
	EGamePhase CurrentPhase = EGamePhase::WaitingToStart;

	EGamePhase PreviousPhase = EGamePhase::WaitingToStart;

	UFUNCTION()
	void OnRep_CurrentPhase();
	
	// Phase 변경 delegate
	UPROPERTY(BlueprintAssignable, Category = "Game")
	FOnPhaseChanged OnPhaseChanged;

public:
	// Mission
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Mission")
	int32 CurrentMissionIndex = 0;

	// Mission helper : 모든 플레이어가 미션 완료했는지 순회하여 체크
	UFUNCTION(BlueprintCallable, Category = "Mission")
	bool AreAllPlayersMissionComplete() const;
	
public:
	// Timer
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Mission")
	float MissionTimeRemaining = 0.f;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Mission")
	float MissionTimeLimit = 300.f;

	// Timer 형식
	UFUNCTION(BlueprintCallable, Category = "Mission")
	FString GetFormattedTimer() const;


	//--------------------------------------------------------------//
	// 시나리오 ID
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Scenario")
	int32 ScenarioIndex = 1;
	// 스테이지 ID
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Scenario")
	int32 ScenarioStageIndex = 1;
	// 레벨(난이도)
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Scenario")
	int32 ScenarioLevel = 1;

	/// @brief 서버로부터 받은 시나리오 데이터 전체 (모든 클라이언트에 복제됨)
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Scenario")
	FResponseScenario CurrentScenarioData;
	//--------------------------------------------------------------//
};
