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

	static FString CurrentTimestamp();

	/// @brief 구조체를 JSON 문자열로 변환합니다.
	/// @return JSON 문자열
	FString ToJson() const;

	/// @brief JSON 문자열을 구조체로 변환합니다.
	/// @param JsonString [in] JSON 문자열
	/// @param OutItem [out] 변환된 구조체
	/// @return 변환 성공 여부
	static bool FromJson(const FString& JsonString, FChatHistoryItem& OutItem);
};