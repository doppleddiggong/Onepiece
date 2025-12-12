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
	/**
	 * @brief 특정 플레이어에 대해 Speak Stage를 시작합니다. (서버에서만 호출)
	 * @param Player [in] 스테이지를 시작할 플레이어의 PlayerState.
	 */
	UFUNCTION(BlueprintCallable, Category = "SpeakStage")
	void StartStageForPlayer(class ALingoPlayerState* Player);

	/**
	 * @brief 플레이어 답변 완료 알림 (Server RPC)
	 * @param Player [in] 답변을 완료한 플레이어
	 */
	UFUNCTION(Server, Reliable)
	void ServerRPC_NotifyAnswerComplete(class ALingoPlayerState* Player);

	/**
	 * @brief 현재 발화자 Getter
	 * @return 현재 발화 권한을 가진 플레이어. 없으면 nullptr.
	 */
	UFUNCTION(BlueprintCallable, Category = "SpeakStage")
	class ALingoPlayerState* GetCurrentSpeaker() const { return CurrentSpeaker; }

	/**
	 * @brief 현재 단계 Getter
	 * @return 현재 질문 인덱스
	 */
	UFUNCTION(BlueprintCallable, Category = "SpeakStage")
	int32 GetCurrentStepIndex() const { return CurrentStepIndex; }

	/**
	 * @brief 현재 질문 Getter
	 * @return 현재 단계의 질문 문자열
	 */
	UFUNCTION(BlueprintCallable, Category = "SpeakStage")
	FString GetCurrentQuestion() const;
	int32 GetTotalQuestionsCount();

	/**
	 * @brief 전체 질문 개수 Getter
	 * @return 시나리오의 전체 질문 개수
	 */
	UFUNCTION(BlueprintCallable, Category = "SpeakStage")
	int32 GetTotalQuestions() const;

	/**
	 * @brief 현재 플레이어의 스테이지를 강제로 종료합니다.
	 * currentSpeaker를 nullptr로 만들고 상태를 초기화합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "SpeakStage")
	void EndStage();
	
protected:
	//----------------------------------------------------------
	// RepNotify Functions
	//----------------------------------------------------------

	/// @brief currentSpeaker 복제 알림
	UFUNCTION()
	void OnRep_CurrentSpeaker();

	/// @brief currentStepIndex 복제 알림
	UFUNCTION()
	void OnRep_CurrentStepIndex();

private:
	/**
	 * @brief 다음 질문으로 진행합니다.
	 * 모든 질문 완료 시 EndStage()를 호출합니다.
	 */
	void AdvanceStep();

	/**
	 * @brief 현재 질문을 Toast로 표시 (서버/클라이언트 공통)
	 * AdvanceStep()과 OnRep_CurrentStepIndex()에서 호출됩니다.
	 */
	void ShowCurrentQuestionToast();
	
public:
	/// @brief 현재 발화자가 변경될 때 호출되는 이벤트입니다.
	UPROPERTY(BlueprintAssignable, Category = "SpeakStage|Events")
	FOnSpeakerChangedDelegate OnSpeakerChanged;


protected:
	//----------------------------------------------------------
	// Replicated Properties
	//----------------------------------------------------------

	/// @brief 현재 발화 권한을 가진 플레이어. nullptr이면 스테이지가 비어있음을 의미.
	UPROPERTY(ReplicatedUsing = OnRep_CurrentSpeaker)
	TObjectPtr<class ALingoPlayerState> CurrentSpeaker;

	/// @brief 현재 진행 단계 (질문 인덱스)
	UPROPERTY(ReplicatedUsing = OnRep_CurrentStepIndex)
	int32 CurrentStepIndex;
};
