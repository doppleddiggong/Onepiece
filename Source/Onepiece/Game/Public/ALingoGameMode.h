// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "ALingoGameMode.generated.h"

UCLASS()
class ONEPIECE_API ALingoGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ALingoGameMode();

	virtual void PostLogin(APlayerController* NewPlayer);

	UFUNCTION(BlueprintCallable, Category = "Quest")
	void BeginReadQuest(const FResponseReadScenario& InResponseData);

	UFUNCTION(BlueprintCallable, Category = "Quest")
	void BeginListenQuest(const FResponseListenScenario& InResponseData);
	
	UFUNCTION()
	void HandleLuggageSelection(class APlayerState* Player, class Aluggage* luggage);

private:
	/// @brief 정답을 판정합니다
	UFUNCTION()
	bool ValidateAnswer(class ALingoPlayerState* Player, class Aluggage* Luggage);

	/// @brief 정답 처리
	UFUNCTION()
	void HandleCorrectAnswer(class ALingoPlayerState* Player);

	/// @brief 오답 처리
	/// @param bSymbolCorrect [in] 심볼이 맞는지 여부
	/// @param bColorCorrect [in] 색상이 맞는지 여부
	UFUNCTION()
	void HandleWrongAnswer(class ALingoPlayerState* Player, bool bSymbolCorrect, bool bColorCorrect);

	UFUNCTION()
	void UpdateQuestRole();
};
