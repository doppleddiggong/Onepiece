// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "NetworkData.h"
#include "ALingoGameState.generated.h"

// DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhaseChanged, EGamePhase, NewPhase);

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
	virtual void Tick(float DeltaSeconds) override;

	void SetStageData(const int InStageIndex, const FResponseScenario& InResponseData);
	
public:
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Mission")
	float RemainMissionTime = 0.f;

	/// @brief 타이머 활성화 상태
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Mission")
	bool bIsTimerActive = false;

	/// @brief 미션 타이머를 시작합니다 (서버에서만 호출)
	UFUNCTION(BlueprintCallable, Category = "Mission")
	void StartMissionTimer(float TimeLimit);

	/// @brief 미션 타이머를 중지합니다
	UFUNCTION(BlueprintCallable, Category = "Mission")
	void StopMissionTimer();

protected:
	/// @brief 타이머 종료 시 호출됩니다 (서버에서만 실행)
	void OnMissionTimerEnd();

	//--------------------------------------------------------------//
	// 시나리오 ID
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Scenario")
	int32 ScenarioIndex = 1;
	// 스테이지 ID
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Scenario")
	int32 StageIndex = 1;
	// 레벨(난이도)
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Scenario")
	int32 ScenarioLevel = 1;

	/// @brief 서버로부터 받은 시나리오 데이터 전체 (모든 클라이언트에 복제됨)
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Scenario")
	FResponseScenario CurScenarioData;
	//--------------------------------------------------------------//
};
