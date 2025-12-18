// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "EQuestRole.h"
#include "NetworkData.h"
#include "ALingoPlayerState.generated.h"

UCLASS()
class ONEPIECE_API ALingoPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ALingoPlayerState();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	FString GetToken() { return AccessToken; }
	void SetToken(FString InToken) { this->AccessToken = InToken; }

	/// @brief Chat AI의 Context를 반환합니다.
	/// @details 기본값은 "You are a helpful assistant"이며,
	///          향후 GameMode, GameState, PlayerState 데이터를 참조하여 동적으로 생성할 수 있습니다.
	/// @return Chat AI에게 전달할 Context 문자열
	FString GetChatContext() const;

	void RefreshQuestState();
	
private:
	FString AccessToken;

public:
	//--------------------------------------------------------------//
	// Read Quest Functions
	//--------------------------------------------------------------//

	/// @brief 심볼 선택을 서버에 전송합니다
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetSelectedWord1(const FString& Word1);

	/// @brief 색상 선택을 서버에 전송합니다
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetSelectedWord2(const FString& Word2);

	UFUNCTION()
	void OnRep_QuestRole();
	
	/// @brief 심볼 선택 상태 복제 콜백
	UFUNCTION()
	void OnRep_SelectedWord1();

	/// @brief 색상 선택 상태 복제 콜백
	UFUNCTION()
	void OnRep_SelectedWord2();

	/// @brief 심볼 오답 플래그 복제 콜백
	UFUNCTION()
	void OnRep_WrongWord1();

	/// @brief 색상 오답 플래그 복제 콜백
	UFUNCTION()
	void OnRep_WrongWord2();
	
	//--------------------------------------------------------------//
	// Write Quest RPC Functions
	//--------------------------------------------------------------//
public:
	UPROPERTY(Transient, BlueprintReadOnly, Category = "WriteQuest")
	FQuestWriteInfo WriteQuestionData;
	
	UPROPERTY(Transient, BlueprintReadOnly, Category = "WriteQuest")
	FResponseWriteSubmit WriteSubmitResultData;
	
	UPROPERTY(Transient, BlueprintReadOnly, Category = "WriteQuest")
	FResponseWriteResult WriteWholeResultData;

	//--------------------------------------------------------------//
	// Speak Quest RPC Functions
	//--------------------------------------------------------------//

	/// @brief 클라이언트가 SpeakScenarioData 수신 준비 완료를 서버에 알립니다.
	UFUNCTION(Server, Reliable)
	void Server_NotifySpeakDataReady();

	/// @brief SpeakQuest 평가 결과를 저장합니다 (Server에서 호출됨)
	/// @param EvaluationResult 저장할 평가 결과
	void AddSpeakJudes(const FResponseSpeakingJudes& EvaluationResult);

	/// @brief SpeakQuest 완료 처리 (서버에서만 호출)
	UFUNCTION(BlueprintCallable, Category = "SpeakQuest")
	void SetSpeakQuestCompleted();

	/// @brief SpeakQuest 완료 여부 확인
	UFUNCTION(BlueprintPure, Category = "SpeakQuest")
	bool IsSpeakQuestCompleted() const { return bSpeakQuestCompleted; }

	/// @brief ReadQuest 완료 처리 (서버에서만 호출)
	UFUNCTION(BlueprintCallable, Category = "ReadQuest")
	void SetReadQuestCompleted();

	/// @brief ReadQuest 완료 여부 확인
	UFUNCTION(BlueprintPure, Category = "ReadQuest")
	bool IsReadQuestCompleted() const { return bReadQuestCompleted; }

	/// @brief ListenQuest 완료 처리 (서버에서만 호출)
	UFUNCTION(BlueprintCallable, Category = "ListenQuest")
	void SetListenQuestCompleted();

	/// @brief ListenQuest 완료 여부 확인
	UFUNCTION(BlueprintPure, Category = "ListenQuest")
	bool IsListenQuestCompleted() const { return bListenQuestCompleted; }

	/// @brief WriteQuest 완료 처리 (서버에서만 호출)
	UFUNCTION(BlueprintCallable, Category = "WriteQuest")
	void SetWriteQuestCompleted();

	/// @brief WriteQuest 완료 여부 확인
	UFUNCTION(BlueprintPure, Category = "WriteQuest")
	bool IsWriteQuestCompleted() const { return bWriteQuestCompleted; }

	/// @brief ReadQuest 진행 상태 설정 (서버에서만 호출)
	UFUNCTION(BlueprintCallable, Category = "ReadQuest")
	void SetReadQuestIng(bool bInProgress);

	/// @brief ListenQuest 진행 상태 설정 (서버에서만 호출)
	UFUNCTION(BlueprintCallable, Category = "ListenQuest")
	void SetListenQuestIng(bool bInProgress);

	/// @brief SpeakQuest 진행 상태 설정 (서버에서만 호출)
	UFUNCTION(BlueprintCallable, Category = "SpeakQuest")
	void SetSpeakQuestIng(bool bInProgress);

	/// @brief WriteQuest 진행 상태 설정 (서버에서만 호출)
	UFUNCTION(BlueprintCallable, Category = "WriteQuest")
	void SetWriteQuestIng(bool bInProgress);

	/// @brief SpeakScenarioData 복제 알림 함수
	UFUNCTION()
	void OnRep_SpeakScenarioData();

public:
	/// @brief 플레이어 역할 (싱글/멀티에서 문제1, 문제2 구분)
	UPROPERTY(ReplicatedUsing = OnRep_QuestRole, BlueprintReadOnly, Category = "Quest")
	EQuestRole QuestRole = EQuestRole::Both;

	/// @brief 시도 횟수
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Quest")
	int32 AttemptCount = 0;
	
	/// @brief 선택한 심볼 (문제1 답변)
	UPROPERTY(ReplicatedUsing = OnRep_SelectedWord1, BlueprintReadOnly, Category = "Quest")
	FString SelectedWord1;
	/// @brief 심볼 오답 플래그
	UPROPERTY(ReplicatedUsing = OnRep_WrongWord1, BlueprintReadOnly, Category = "Quest")
	bool bWrongWord1 = false;
	
	/// @brief 선택한 색상 (문제2 답변)
	UPROPERTY(ReplicatedUsing = OnRep_SelectedWord2, BlueprintReadOnly, Category = "Quest")
	FString SelectedWord2;
	/// @brief 색상 오답 플래그
	UPROPERTY(ReplicatedUsing = OnRep_WrongWord2, BlueprintReadOnly, Category = "Quest")
	bool bWrongWord2 = false;

	
public:
	//--------------------------------------------------------------//
	// Speak Quest Functions
	//--------------------------------------------------------------//

	bool GetCurrentSpeakQuestion(int32 StepIndex, FSpeakStageQuestion& Out) const;	

	/// @brief SpeakScenarioData가 업데이트될 때 Host와 Client 모두에서 호출되는 공통 함수입니다.
	void OnUpdateSpeakScenarioData();
	
	UPROPERTY(Transient, ReplicatedUsing = OnRep_SpeakScenarioData, BlueprintReadOnly, Category = "SpeakQuest")
	FResponseSpeakScenario SpeakScenarioData;

	FResponseSpeakResult SpeakResult;
	
	/// @brief SpeakQuest 평가 결과 목록
	/// @note RequestSpeakingJudges 응답 데이터를 쌓아둠
	UPROPERTY(Transient, Replicated, BlueprintReadOnly, Category = "SpeakQuest")
	TArray<struct FResponseSpeakingJudes> SpeakJudesResults;


	/// @brief ReadQuest 완료 여부 플래그
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "ReadQuest")
	bool bReadQuestCompleted = false;

	/// @brief ListenQuest 완료 여부 플래그
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "ListenQuest")
	bool bListenQuestCompleted = false;

	/// @brief SpeakQuest 완료 여부 플래그
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "SpeakQuest")
	bool bSpeakQuestCompleted = false;
	
	/// @brief WriteQuest 완료 여부 플래그
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "WriteQuest")
	bool bWriteQuestCompleted = false;


	/// @brief ReadQuest 진행 여부 플래그
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "ReadQuest")
	bool bReadQuestIng = false;

	/// @brief ListenQuest 진행 여부 플래그
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "ListenQuest")
	bool bListenQuestIng = false;

	/// @brief SpeakQuest 진행 여부 플래그
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "ListenQuest")
	bool bSpeakQuestIng = false;
	
	/// @brief WriteQuest 진행 여부 플래그
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "WriteQuest")
	bool bWriteQuestIng  = false;
};