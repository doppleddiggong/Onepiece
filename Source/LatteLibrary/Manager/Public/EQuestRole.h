#pragma once

/// @brief Read 퀘스트에서 플레이어의 역할을 정의합니다.
UENUM(BlueprintType)
enum class EQuestRole : uint8
{
	Both           UMETA(DisplayName = "Both"),          // 싱글: 문제1, 2 모두 조작
	OnlyQuestion1  UMETA(DisplayName = "OnlyQuestion1"), // 멀티: 문제1만 조작
	OnlyQuestion2  UMETA(DisplayName = "OnlyQuestion2")  // 멀티: 문제2만 조작
};