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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReadResultUpdated, const FResponseReadResult&, Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnListenResultUpdated, const FResponseListenResult&, Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnQuestScenarioDataUpdated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRoomIdUpdated, int64, NewRoomId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRoomLevelUpdated, int32, NewRoomLevel);

UCLASS()
class ONEPIECE_API ALingoGameState : public AGameState
{
	GENERATED_BODY()

public:
	ALingoGameState();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaSeconds) override;

	FORCEINLINE int64 GetRoomId() { return RoomId; };
	FORCEINLINE int32 GetRoomLevel() { return RoomLevel; };
	FORCEINLINE float GetRemainMissionTime() { return RemainMissionTime; }
	FORCEINLINE float GetTimeTaken() { return TimeLimit - RemainMissionTime; }
	FORCEINLINE bool IsQuestIng() { return QuestType != EQuestType::None;	}
	FORCEINLINE EQuestType GetCurrentQuestType() const { return QuestType; }
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
	void Multicast_UpdateQuestType(const EQuestType InQuestType);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ShowReadQuestPopup(const FResponseReadScenario& InScenarioData);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ShowListenQuestPopup(const FResponseListenScenario& InScenarioData);

	UFUNCTION()
	void OnRep_ReadScenarioData();

	UFUNCTION()
	void OnRep_ReadResult();

	UFUNCTION()
	void OnRep_ListenScenarioData();

	UFUNCTION()
	void OnRep_ListenResult();

	UFUNCTION()
	void OnRep_RoomId();

	UFUNCTION()
	void OnRep_RoomLevel();
	
public:
	// Chat System
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_SendChat(const FText& inMessage);
	
private:
	/// @brief 타이머 종료 시 호출됩니다 (서버에서만 실행)
	void OnMissionTimerEnd();
	
public:
	FOnReadResultUpdated OnReadResultUpdated;
	FOnListenResultUpdated OnListenResultUpdated;
	FOnQuestScenarioDataUpdated OnQuestScenarioDataUpdated;
	FOnRoomIdUpdated OnRoomIdUpdated;
	FOnRoomLevelUpdated OnRoomLevelUpdated;

	FORCEINLINE int GetWrongReadAnswerNum() { return WrongReadAnswerList.Num(); }
	void AddWrongReadAnswer(int32 Value);
	
	UPROPERTY(ReplicatedUsing = OnRep_ReadScenarioData)
	FResponseReadScenario ReadScenarioData;

	UPROPERTY(Replicated)
	TArray<int32> WrongReadAnswerList;

	UPROPERTY(Replicated)
	FResponseReadResult ReqReadResult;
	
	UPROPERTY(ReplicatedUsing = OnRep_ReadResult)
	FResponseReadResult ReadResult;
	//--------------------------------------------------------------//
	
	FORCEINLINE int GetWrongListenAnswerNum() { return WrongListenAnswerList.Num(); }
	void AddWrongListenAnswer(int32 Value);
	// 서버에서 받아오는 원본
	UPROPERTY(ReplicatedUsing = OnRep_ListenScenarioData)
	FResponseListenScenario ListenScenarioData;
	// 시도한 선택지들 저장
	UPROPERTY(Replicated)
	FResponseListenScenario TryListenAnswerData;
	// 서버로 정답률 보내기 위한 형식상 인덱스 리스트
	UPROPERTY(Replicated)
	TArray<int32> WrongListenAnswerList;

	UPROPERTY(Replicated)
	FResponseListenResult ReqListenResult;

	UPROPERTY(ReplicatedUsing = OnRep_ListenResult)
	FResponseListenResult ListenResult;
	//--------------------------------------------------------------//

protected:
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Mission")
	float RemainMissionTime = 0.f;

	/// @brief 타이머 활성화 상태
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Mission")
	bool bIsTimerActive = false;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Quest")
	EQuestType QuestType;

	/// @brief 방 ID (Host가 생성하고 Guest들과 공유)
	UPROPERTY(ReplicatedUsing=OnRep_RoomId, BlueprintReadOnly, Category = "Room")
	int64 RoomId = 0;

	UPROPERTY(ReplicatedUsing=OnRep_RoomLevel, BlueprintReadOnly, Category = "Room")
	int32 RoomLevel = 1;
	
private:
	float TimeLimit = 0;
};
