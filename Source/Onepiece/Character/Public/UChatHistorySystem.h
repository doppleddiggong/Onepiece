// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/// @file UChatHistorySystem.h
/// @brief Chat 대화 기록을 GConfig를 이용하여 관리하는 컴포넌트입니다.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FChatHistoryItem.h"
#include "UChatHistorySystem.generated.h"


/// @brief Chat 대화 기록을 GConfig로 저장/로드하는 컴포넌트
/// @details UserId 기반으로 각 유저의 대화 히스토리를 독립적으로 관리합니다.
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ONEPIECE_API UChatHistorySystem : public UActorComponent
{
	GENERATED_BODY()

public:
	/// @brief Chat 대화를 히스토리에 저장합니다.
	/// @param Question [in] 질문 내용
	/// @param Answer [in] 답변 내용
	UFUNCTION(BlueprintCallable, Category = "ChatHistory")
	void SaveChatHistory(const FString& Question, const FString& Answer);

	/// @brief 저장된 모든 Chat 히스토리를 불러옵니다.
	/// @param OutHistoryList [out] 히스토리 리스트 (Index 순으로 정렬됨)
	/// @return 히스토리 개수
	UFUNCTION(BlueprintCallable, Category = "ChatHistory")
	int32 LoadAllChatHistory(TArray<FChatHistoryItem>& OutHistoryList);

	/// @brief 저장된 모든 Chat 히스토리를 삭제합니다.
	UFUNCTION(BlueprintCallable, Category = "ChatHistory")
	void ClearChatHistory();

	/// @brief 현재 유저의 히스토리 개수를 반환합니다.
	UFUNCTION(BlueprintCallable, Category = "ChatHistory")
	int32 GetHistoryCount() const;

private:
	/// @brief GConfig에서 다음 인덱스를 가져오거나 생성합니다.
	int32 GetNextIndex() const;
};
