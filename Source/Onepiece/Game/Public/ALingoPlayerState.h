// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
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
	void SetToken(FString InToken);
	FString GetToken() { return AccessToken; }

	void SetUserName(FString InUserName);
	FString GetUserName() { return UserName; }

	//--------------------------------------------------------------//
	// Read Quest Functions
	//--------------------------------------------------------------//

	/// @brief 심볼 선택을 서버에 전송합니다
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetSelectedSymbol(const FString& Symbol);

	/// @brief 색상 선택을 서버에 전송합니다
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetSelectedColor(const FString& Color);

	/// @brief 심볼 선택 상태 복제 콜백
	UFUNCTION()
	void OnRep_SelectedSymbol();

	/// @brief 색상 선택 상태 복제 콜백
	UFUNCTION()
	void OnRep_SelectedColor();

	/// @brief 심볼 오답 플래그 복제 콜백
	UFUNCTION()
	void OnRep_SymbolWrong();

	/// @brief 색상 오답 플래그 복제 콜백
	UFUNCTION()
	void OnRep_ColorWrong();

private:
	FString AccessToken;
	FString UserName;

	//--------------------------------------------------------------//
	// Read Quest Data
	//--------------------------------------------------------------//

public:
	/// @brief 플레이어 역할 (싱글/멀티에서 문제1, 문제2 구분)
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Quest")
	EReadQuestRole QuestRole = EReadQuestRole::Both;

	/// @brief 선택한 심볼 (문제1 답변)
	UPROPERTY(ReplicatedUsing = OnRep_SelectedSymbol, BlueprintReadOnly, Category = "Quest")
	FString SelectedSymbol;

	/// @brief 선택한 색상 (문제2 답변)
	UPROPERTY(ReplicatedUsing = OnRep_SelectedColor, BlueprintReadOnly, Category = "Quest")
	FString SelectedColor;

	/// @brief 심볼 오답 플래그
	UPROPERTY(ReplicatedUsing = OnRep_SymbolWrong, BlueprintReadOnly, Category = "Quest")
	bool bSymbolWrong = false;

	/// @brief 색상 오답 플래그
	UPROPERTY(ReplicatedUsing = OnRep_ColorWrong, BlueprintReadOnly, Category = "Quest")
	bool bColorWrong = false;

	/// @brief 시도 횟수
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Quest")
	int32 AttemptCount = 0;
};
