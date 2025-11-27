// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "EQuestRole.h"
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
	void SetToken(FString InToken);
	FString GetToken() { return AccessToken; }

	void SetUserName(FString InUserName);
	FString GetUserName() { return UserName; }

private:
	FString AccessToken;

	FString UserName;

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
};