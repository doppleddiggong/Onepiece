// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "NetworkData.h"
#include "GameFramework/GameState.h"
#include "ALingoGameState.generated.h"

UENUM(Blueprintable)
enum class EGameState : uint8
{
	None,

	QuestStart,		// 퀘스트 시작
	QuestEnd,		// 퀘스트 종료 
	
	AllQuestsEnd,	// 모든 퀘스트 종료
};

UENUM(Blueprintable)
enum class EQuestType : uint8
{
	None = 0,
	
	Read = 1,
	Listen = 2,
	Write = 3,
	Speak = 4
};

USTRUCT(BlueprintType)
struct FQuestData
{
	GENERATED_BODY()

	// 시나리오 ID
	UPROPERTY(BlueprintReadOnly, Category = "QuestData")
	int32 ScenarioIndex;
	// 스테이지 ID
	UPROPERTY(BlueprintReadOnly, Category = "QuestData")
	EQuestType QuestType;
	// 레벨(난이도)
	UPROPERTY(BlueprintReadOnly, Category = "QuestData")
	int32 ScenarioLevel;

	// 기본 생성자
	FQuestData() : ScenarioIndex(1), QuestType(EQuestType::None), ScenarioLevel(1) {}
};

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
	

	void SetStageData(const int InStageIndex, int InQuestIndex, const FResponseScenario& InResponseData);

	/// @brief 미션 타이머를 시작합니다 (서버에서만 호출)
	UFUNCTION(BlueprintCallable, Category = "Mission")
	void StartMissionTimer(float TimeLimit);

	/// @brief 미션 타이머를 중지합니다
	UFUNCTION(BlueprintCallable, Category = "Mission")
	void StopMissionTimer();

	UFUNCTION(BlueprintCallable, Category = "Mission")
	float GetRemainMissionTime()
	{
		return RemainMissionTime;
	}

	UFUNCTION(BlueprintCallable, Category = "Mission")
	bool IsQuestIng()
	{
		return GameState == EGameState::QuestStart;
	}

	//--------------------------------------------------------------//
	// Read Quest Functions
	//--------------------------------------------------------------//
	/// @brief Read 퀘스트 성공 시 OnRep 콜백
	UFUNCTION()
	void OnRep_QuestSuccess();

	FORCEINLINE const FResponseScenario& GetScenarioData() const {return CurScenarioData;}
	//FORCEINLINE EGameState GetGameState() { return GameState; }

protected:
	/// @brief Stage1 시작 시 모든 클라이언트(서버 포함)에 Read Quest 팝업 표시 요청
	/// @param InStageIndex 스테이지 인덱스
	/// @param InScenarioData 시나리오 데이터
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ShowReadQuestPopup(int InStageIndex, const FResponseScenario& InScenarioData);

private:
	/// @brief 타이머 종료 시 호출됩니다 (서버에서만 실행)
	void OnMissionTimerEnd();
	
public:
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Mission")
	float RemainMissionTime = 0.f;

public:
	// 현재 퀘스트 정보
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Quest")
	FQuestData CurrentQuest;
	
	// 퀘스트 진행중인지 여부
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "State")
	EGameState GameState;

	/// @brief 타이머 활성화 상태
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Mission")
	bool bIsTimerActive = false;
	
	//--------------------------------------------------------------//

public:
	/// @brief 1. Reading Quest 데이터 (모든 클라이언트에 복제됨)
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Scenario")
	FResponseScenario CurScenarioData;

	/// @brief 오답 캐리어 목록
	UPROPERTY(Replicated)
	TArray<int32> WrongLuggageList;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "QuestResult")
	FResponseQuestResult CurQuestResult;
	//--------------------------------------------------------------//

public:

	//--------------------------------------------------------------//
	// Read Quest Data
	//--------------------------------------------------------------//

	/// @brief Step1(Read) 퀘스트 결과 기록
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Quest")
	FReadQuestResult QuestResult;

	/// @brief 퀘스트 성공 플래그
	UPROPERTY(ReplicatedUsing = OnRep_QuestSuccess, BlueprintReadOnly, Category = "Quest")
	bool bQuestSuccess = false;
	//--------------------------------------------------------------//
};