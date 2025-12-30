// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ASpeakStageActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpeakerChangedDelegate, APlayerState*, NewSpeaker);

/**
 * @brief Speak Stage 시스템
 * @details 한 번에 한 명의 플레이어와 순차적 음성 대화를 진행하는 액터.
 *
 * 핵심 기능:
 * - 한 번에 한 플레이어만 발화 가능 (사용 중/비사용 중 상태)
 * - Server Authority로 턴 제어
 * - 모든 클라이언트에 상태 복제
 * - 순차적 질문 진행 관리
 */
UCLASS()
class ONEPIECE_API ASpeakStageActor : public AActor
{
	GENERATED_BODY()

public:
	/// @brief 생성자
	ASpeakStageActor();

	/// @brief Replication 설정
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	UFUNCTION(BlueprintCallable, Category = "SpeakStage")
	FORCEINLINE class ALingoPlayerState* GetCurrentSpeaker() const { return CurrentSpeaker; }

	UFUNCTION(BlueprintCallable, Category = "SpeakStage")
	FORCEINLINE int32 GetCurrentStepIndex() const { return CurrentStepIndex; }

	UFUNCTION(BlueprintCallable, Category = "SpeakStage")
	FORCEINLINE FString GetCurrentQuestion() const;

	UFUNCTION(BlueprintCallable, Category = "SpeakStage")
	int32 GetTotalQuestionsCount();

	UFUNCTION(BlueprintCallable, Category = "SpeakStage")
	int32 GetTotalQuestions() const;

	/**
	 * @brief 특정 플레이어에 대해 Speak Stage를 시작합니다. (서버에서만 호출)
	 * @param Player [in] 스테이지를 시작할 플레이어의 PlayerState.
	 */
	UFUNCTION(BlueprintCallable, Category = "SpeakStage")
	void StartStageForPlayer(class ALingoPlayerState* Player);

	/**
	 * @brief 현재 플레이어의 스테이지를 강제로 종료합니다.
	 * currentSpeaker를 nullptr로 만들고 상태를 초기화합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "SpeakStage")
	void EndStage();
	
public:
	/**
	 * @brief 플레이어 답변 완료 알림 (Server에서 호출됨)
	 * @param Player [in] 답변을 완료한 플레이어
	 */
	void NotifyAnswerComplete(class ALingoPlayerState* Player);

	bool IsMyTurn(class ALingoPlayerState* lingo_player_state);

protected:
	/// @brief 모든 클라이언트에게 SpeakQuest 시작 알림 전송 (Multicast RPC)
	/// @param PlayerName [in] Quest를 시작한 플레이어 이름
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_NotifySpeakQuestStarted(const FString& PlayerName);

	
	/// @brief currentSpeaker 복제 알림
	UFUNCTION()
	void OnRep_CurrentSpeaker();

private:
	/**
	 * @brief 다음 질문으로 진행합니다.
	 * 모든 질문 완료 시 EndStage()를 호출합니다.
	 */
	void AdvanceStep();
	
public:
	/// @brief 현재 발화자가 변경될 때 호출되는 이벤트입니다.
	UPROPERTY(BlueprintAssignable, Category = "SpeakStage|Events")
	FOnSpeakerChangedDelegate OnSpeakerChanged;

protected:
	/// @brief 현재 발화 권한을 가진 플레이어. nullptr이면 스테이지가 비어있음을 의미.
	UPROPERTY(ReplicatedUsing = OnRep_CurrentSpeaker)
	TObjectPtr<class ALingoPlayerState> CurrentSpeaker;

	/// @brief 현재 진행 단계 (질문 인덱스)
	UPROPERTY(Replicated)
	int32 CurrentStepIndex;
};
