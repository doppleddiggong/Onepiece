// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "USoundData.h"
#include "GameFramework/Actor.h"
#include "ASpeakStageActor.generated.h"

/**
 * @brief Speak Stage 턴 관리 시스템
 * @details 순차적 음성 대화 학습을 위한 턴 기반 관리 Actor
 *
 * 핵심 기능:
 * - 한 번에 한 플레이어만 발화 가능
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

protected:
	/// @brief BeginPlay
	virtual void BeginPlay() override;

	//----------------------------------------------------------
	// Replicated Properties
	//----------------------------------------------------------

	/// @brief 현재 발화 권한을 가진 플레이어
	UPROPERTY(ReplicatedUsing = OnRep_CurrentSpeaker)
	TObjectPtr<APlayerState> CurrentSpeaker;

	/// @brief 현재 진행 단계 (질문 인덱스)
	UPROPERTY(ReplicatedUsing = OnRep_CurrentStepIndex)
	int32 CurrentStepIndex;

	/// @brief 대기 중인 플레이어 큐
	UPROPERTY(Replicated)
	TArray<TObjectPtr<APlayerState>> PlayerQueue;

	//----------------------------------------------------------
	// Scenario Data (Server Only)
	//----------------------------------------------------------

	/// @brief 시나리오 질문 목록 (서버 전용)
	UPROPERTY()
	TArray<FString> Questions;

	UPROPERTY()
	TArray<EGameSoundType> Questions_Voice;

	
	/// @brief 전체 질문 개수
	UPROPERTY()
	int32 TotalQuestions;

	//----------------------------------------------------------
	// RepNotify Functions
	//----------------------------------------------------------

	/// @brief CurrentSpeaker 복제 알림
	UFUNCTION()
	void OnRep_CurrentSpeaker();

	/// @brief CurrentStepIndex 복제 알림
	UFUNCTION()
	void OnRep_CurrentStepIndex();

public:
	//----------------------------------------------------------
	// Public Interface
	//----------------------------------------------------------

	/**
	 * @brief Speak Stage 플레이 시작 (서버에서만 호출)
	 * @param Players 참여할 플레이어 배열
	 *
	 * PlayStart는 명시적 초기화 함수입니다.
	 * - 플레이어 큐 설정
	 * - 첫 번째 플레이어를 CurrentSpeaker로 설정
	 * - CurrentStepIndex 초기화
	 */
	UFUNCTION(BlueprintCallable, Category = "SpeakStage")
	void PlayStart(const TArray<APlayerState*>& Players);

	/**
	 * @brief 플레이어가 발화 권한 요청 (Server RPC)
	 * @param Player 발화를 요청하는 플레이어
	 *
	 * CurrentSpeaker가 아니면 거부됩니다.
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RequestSpeak(APlayerState* Player);

	/**
	 * @brief 플레이어 답변 완료 알림 (Server RPC)
	 * @param Player 답변을 완료한 플레이어
	 *
	 * 다음 단계로 진행하거나, 모든 단계 완료 시 다음 플레이어로 전환합니다.
	 */
	UFUNCTION(Server, Reliable)
	void Server_NotifyAnswerComplete(APlayerState* Player);

	/**
	 * @brief 현재 발화자 Getter
	 * @return 현재 발화 권한을 가진 플레이어
	 */
	UFUNCTION(BlueprintCallable, Category = "SpeakStage")
	APlayerState* GetCurrentSpeaker() const { return CurrentSpeaker; }

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

	/**
	 * @brief 전체 질문 개수 Getter
	 * @return 시나리오의 전체 질문 개수
	 */
	UFUNCTION(BlueprintCallable, Category = "SpeakStage")
	int32 GetTotalQuestions() const { return TotalQuestions; }

private:
	//----------------------------------------------------------
	// Internal Logic
	//----------------------------------------------------------

	/**
	 * @brief 다음 질문으로 진행
	 *
	 * CurrentStepIndex를 증가시키고,
	 * 모든 질문 완료 시 AdvanceToNextPlayer() 호출
	 */
	void AdvanceStep();

	/**
	 * @brief 다음 플레이어로 전환
	 *
	 * 현재 플레이어를 큐에서 제거하고,
	 * 다음 플레이어를 CurrentSpeaker로 설정
	 * CurrentStepIndex를 0으로 초기화
	 */
	void AdvanceToNextPlayer();

	/**
	 * @brief 현재 질문을 Toast로 표시 (서버/클라이언트 공통)
	 *
	 * AdvanceStep()과 OnRep_CurrentStepIndex()에서 호출
	 */
	void ShowCurrentQuestionToast();

	/**
	 * @brief 테스트 시나리오 데이터 생성 (서버 전용)
	 *
	 * 입국 심사 시뮬레이션 질문 5개 생성
	 */
	void CreateTestScenarioData();
};
