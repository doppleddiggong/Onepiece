// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "ALingoGameState.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ULingoGameHelper.generated.h"

/**
 * 
 */
UCLASS()
class ONEPIECE_API ULingoGameHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Utils")
	static int GetMultiPlayerCount(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Utils")
	static bool IsMultiPlay(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Utils")
	static ALingoGameMode* GetLingoGameMode(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable, Category = "Utils")
	static ALingoGameState* GetLingoGameState(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable, Category = "Utils")
	static ALingoPlayerState* GetLingoPlayerState(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Utils")
	static TArray<ALingoPlayerState*> GetLingoPlayerStateList(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable, Category = "Utils")
	static FString GetStageStartMessage(const int StageIndex);

	UFUNCTION(BlueprintCallable, Category = "Utils")
	static FString GetStageEndMessage(const int StageIndex);

	UFUNCTION(BlueprintCallable, Category = "Utils")
	static float GetMissionPlayTime(const int Level);

	UFUNCTION(BlueprintCallable, Category = "Utils")
	static FString GetFormatTimer(const float InRemainTime );

	/// @brief 마우스 커서를 표시하고 게임+UI 입력 모드로 설정합니다.
	/// @param WorldContextObject [in] World 컨텍스트 객체
	UFUNCTION(BlueprintCallable, Category = "Utils")
	static void ShowMouseCursor(const UObject* WorldContextObject);

	/// @brief 마우스 커서를 숨기고 게임 전용 입력 모드로 설정합니다.
	/// @param WorldContextObject [in] World 컨텍스트 객체
	UFUNCTION(BlueprintCallable, Category = "Utils")
	static void HideMouseCursor(const UObject* WorldContextObject);

	/// @brief 첫 번째 플레이어의 PlayerActor를 가져옵니다.
	/// @param WorldContextObject [in] World 컨텍스트 객체
	/// @return PlayerActor 인스턴스 (없으면 nullptr)
	UFUNCTION(BlueprintCallable, Category = "Utils")
	static class APlayerActor* GetPlayerActor(const UObject* WorldContextObject);
};
