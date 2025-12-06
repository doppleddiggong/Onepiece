// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "NetworkData.h"
#include "GameFramework/GameState.h"
#include "ALingoGameState.generated.h"

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

	// 스테이지 ID
	UPROPERTY(BlueprintReadOnly, Category = "QuestData")
	EQuestType QuestType;

	// 기본 생성자
	FQuestData() : QuestType(EQuestType::None) {}
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

	FORCEINLINE int64 GetRoomId() { return RoomId; };
	FORCEINLINE float GetRemainMissionTime() { return RemainMissionTime; }
	FORCEINLINE float GetTimeTaken() { return TimeLimit - RemainMissionTime;	}
	FORCEINLINE bool IsQuestIng() { return CurrentQuestData.QuestType != EQuestType::None;	}
	FORCEINLINE const FResponseReadScenario& GetReadScenarioData() const {return ReadScenarioData;}
	FORCEINLINE const FResponseListenScenario& GetListenScenarioData() const {return ListenScenarioData;}

	/// @brief 미션 타이머를 시작합니다 (서버에서만 호출)
	UFUNCTION(BlueprintCallable, Category = "Mission")
	void StartMissionTimer(float TimeLimit);

	/// @brief 미션 타이머를 중지합니다
	UFUNCTION(BlueprintCallable, Category = "Mission")
	void StopMissionTimer();

	UFUNCTION(BlueprintCallable, Category = "Mission")
	void DecreaseMissionTimer(const float InValue);

	void UpdateRemainMissionTime(const float InTimeLimit);

	void SetReadScenarioData(const FResponseReadScenario& InResponseData);
	void SetListenScenarioData(const FResponseListenScenario& InResponseData);
	
protected:
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ShowReadQuestPopup(const FResponseReadScenario& InScenarioData);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ShowListenQuestPopup(const FResponseListenScenario& InScenarioData);

private:
	/// @brief 타이머 종료 시 호출됩니다 (서버에서만 실행)
	void OnMissionTimerEnd();
	
public:
	FORCEINLINE int GetWrongReadAnswerNum() { return WrongReadAnswerList.Num(); }
	
	UPROPERTY(Replicated)
	FResponseReadScenario ReadScenarioData;

	UPROPERTY(Replicated)
	TArray<int32> WrongReadAnswerList;

	UPROPERTY(Replicated)
	FResponseReadResult ReqReadResult;
	
	UPROPERTY(Replicated)
	FResponseReadResult ReadResult;
	//--------------------------------------------------------------//

	FORCEINLINE int GetWrongListenAnswerNum() { return WrongListenAnswerList.Num(); }

	UPROPERTY(Replicated)
	FResponseListenScenario ListenScenarioData;
	
	UPROPERTY(Replicated)
	TArray<int32> WrongListenAnswerList;

	UPROPERTY(Replicated)
	FResponseListenResult ReqListenResult;

	UPROPERTY(Replicated)
	FResponseListenResult ListenResult;
	//--------------------------------------------------------------//


protected:
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Mission")
	float RemainMissionTime = 0.f;

	/// @brief 타이머 활성화 상태
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Mission")
	bool bIsTimerActive = false;

	// 현재 퀘스트 정보
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Quest")
	FQuestData CurrentQuestData;
		
private:
	int64 RoomId = 0;
	float TimeLimit = 0;
};