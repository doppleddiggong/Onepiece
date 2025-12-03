// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "ALingoGameMode.generated.h"

struct FResponseScenario;

/**
 *
 */
UCLASS()
class ONEPIECE_API ALingoGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ALingoGameMode();
	//--------------------------------------------------------------//
	// Read Quest Functions
	//--------------------------------------------------------------//

	/// @brief Read 퀘스트를 시작하고 역할과 데이터를 모두 설정합니다 (서버에서만 호출)
	UFUNCTION(BlueprintCallable, Category = "Quest")
	void BeginReadQuest(int32 InStageIndex, const FResponseScenario& InResponseData);

	UFUNCTION(BlueprintCallable, Category = "Quest")
	void BeginListenQuest(int32 InStageIndex, const FResponseScenario& InResponseData);

	//--------------------------------------------------------------//
	// Speak Quest Functions
	//--------------------------------------------------------------//

	/// @brief Speak 퀘스트 시작 (서버에서만 호출)
	/// @param InStageIndex 스테이지 인덱스
	UFUNCTION(BlueprintCallable, Category = "Quest")
	void BeginSpeakQuest(int32 InStageIndex);
	
	/// @brief 캐리어 선택을 처리합니다 (서버에서만 호출)
	UFUNCTION()
	void HandleCarrierSelection(class APlayerState* Player, class Aluggage* Carrier);

	/// @brief 정답을 판정합니다
	UFUNCTION()
	bool ValidateAnswer(class ALingoPlayerState* Player, class Aluggage* Carrier);

	/// @brief 정답 처리
	UFUNCTION()
	void HandleCorrectAnswer(class ALingoPlayerState* Player);

	/// @brief 오답 처리
	/// @param bSymbolCorrect [in] 심볼이 맞는지 여부
	/// @param bColorCorrect [in] 색상이 맞는지 여부
	UFUNCTION()
	void HandleWrongAnswer(class ALingoPlayerState* Player, bool bSymbolCorrect, bool bColorCorrect);
};
