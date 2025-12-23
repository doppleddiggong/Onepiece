#pragma once

#include "CoreMinimal.h"

#include "FChatHistoryItem.generated.h"

/// @brief Chat 히스토리 아이템 구조체입니다.
/// @note GConfig에 저장되며, 순서와 타임스탬프를 포함합니다.
USTRUCT(BlueprintType)
struct FChatHistoryItem
{
	GENERATED_BODY()

	/// @brief 히스토리 인덱스 (0부터 시작, 작을수록 오래된 대화)
	UPROPERTY(BlueprintReadWrite, Category = "ChatHistory")
	int32 Index = 0;

	/// @brief 질문 내용
	UPROPERTY(BlueprintReadWrite, Category = "ChatHistory")
	FString Question;

	/// @brief 답변 내용
	UPROPERTY(BlueprintReadWrite, Category = "ChatHistory")
	FString Answer;

	/// @brief 저장 시간 (YYYY-MM-DD HH:MM:SS 형식)
	UPROPERTY(BlueprintReadWrite, Category = "ChatHistory")
	FString Timestamp;
};