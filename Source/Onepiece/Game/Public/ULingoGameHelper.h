// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "ALingoGameState.h"
#include "FColorStyleData.h"
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
	static int64 GetUnixTimestampInt64();

	UFUNCTION(BlueprintCallable, Category = "Utils")
	static int32 GetUserId(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable, Category = "Utils")
	static int GetMultiPlayerCount(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Utils")
	static bool IsMultiPlay(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Utils")
	static class ALingoGameMode* GetLingoGameMode(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable, Category = "Utils")
	static class ALingoGameState* GetLingoGameState(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Utils")
	static class ALingoPlayerState* GetLingoPlayerStateByPC(const AController* PC);
	
	UFUNCTION(BlueprintCallable, Category = "Utils")
	static class ALingoPlayerState* GetLingoPlayerState(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Utils")
	static TArray<class ALingoPlayerState*> GetLingoPlayerStateList(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Utils")
	static bool IsLocalPlayerPawn(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Utils")
	static APawn* GetLocalPawn(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Utils")
	static FString GetStageStartMessage(const EQuestType QuestType);
	
	UFUNCTION(BlueprintCallable, Category = "Utils")
	static FString GetStageEndMessage(const EQuestType QuestType);
	
	UFUNCTION(BlueprintCallable, Category = "Utils")
	static int32 GetStageTypeIndex(const EQuestType QuestType);
	
	UFUNCTION(BlueprintCallable, Category = "Utils")
	static float GetMissionPlayTime();

	UFUNCTION(BlueprintCallable, Category = "Utils")
	static FString GetFormatTimer(const float InRemainTime );

	UFUNCTION(BlueprintCallable, Category = "Utils")
	static EResourceTextureType ConvertGradeScore(const float Score);

	UFUNCTION(BlueprintCallable, Category = "Utils")
	static EResourceTextureType ConvertGradeString(const FString& Grade);
	
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

	static class APlayerControl* GetPlayerControl(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Utils")
	static class ASpeakStageActor* GetSpeakStageActor(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Utils")
	static class AWheatly* GetWheatly(const UObject* WorldContextObject);

	/// @brief PlayerState에서 PlayerControl을 통해 사용자 이름 가져오기
	/// @param PlayerState [in] 플레이어 상태
	/// @return 사용자 이름 (UserInfo.username), 실패 시 PlayerState의 기본 이름
	UFUNCTION(BlueprintCallable, Category = "Utils")
	static FString GetPlayerNameFromState(const class ALingoPlayerState* PlayerState);

	UFUNCTION(BlueprintCallable, Category = "Utils")
	static FString GetTimeRank(float InTimeTaken);

	UFUNCTION(BlueprintCallable, Category = "Utils")
	static FLinearColor GetRankColor(float InScore);

	UFUNCTION(BlueprintCallable, Category = "Utils")
	static EColorStyleType GetRankColorType(float InScore);

	UFUNCTION(BlueprintCallable, Category = "Utils")
	static FString GetAccuracyPercentage(int WrongCnt);
};
