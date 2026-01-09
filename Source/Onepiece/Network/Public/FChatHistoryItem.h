#pragma once

#include "CoreMinimal.h"

#include "FChatHistoryItem.generated.h"

/**
 * @brief 채팅 히스토리 한 항목을 보관하는 구조체입니다.
 *
 * 사용자 질문과 답변을 한 쌍으로 저장해 재요청 없이 복기할 수 있게 합니다.
 * GConfig 기반 영속 저장을 전제로 하며, UI 표시 순서와 타임스탬프를 유지합니다.
 * 사용 위치: 채팅 히스토리 로딩/저장 및 UI 기록 갱신.
 */
USTRUCT(BlueprintType)
struct FChatHistoryItem
{
	GENERATED_BODY()

	/**
	 * @brief 히스토리 인덱스입니다.
	 *
	 * 범위: 0부터 시작하며 값이 작을수록 오래된 대화입니다.
	 * 소유권/수명: 히스토리 배열 소유자가 관리하며 저장 시 그대로 직렬화됩니다.
	 * 동기화: 네트워크 복제와 무관합니다.
	 */
	UPROPERTY(BlueprintReadWrite, Category = "ChatHistory")
	int32 Index = 0;

	/**
	 * @brief 질문 텍스트입니다.
	 *
	 * 사용자 입력의 원문을 저장하여 히스토리 리플레이에 사용합니다.
	 * 소유권/수명: 히스토리 배열 소유자가 관리하며 저장 시 그대로 직렬화됩니다.
	 * 동기화: 네트워크 복제와 무관합니다.
	 */
	UPROPERTY(BlueprintReadWrite, Category = "ChatHistory")
	FString Question;

	/**
	 * @brief 답변 텍스트입니다.
	 *
	 * 서버 응답을 그대로 보관하여 UI 기록 표시와 재사용에 활용합니다.
	 * 소유권/수명: 히스토리 배열 소유자가 관리하며 저장 시 그대로 직렬화됩니다.
	 * 동기화: 네트워크 복제와 무관합니다.
	 */
	UPROPERTY(BlueprintReadWrite, Category = "ChatHistory")
	FString Answer;

	/**
	 * @brief 저장 시간 문자열입니다.
	 *
	 * 포맷: YYYY-MM-DD HH:MM:SS(로컬 시간 기준)로 기록됩니다.
	 * 소유권/수명: 히스토리 배열 소유자가 관리하며 저장 시 그대로 직렬화됩니다.
	 * 동기화: 네트워크 복제와 무관합니다.
	 */
	UPROPERTY(BlueprintReadWrite, Category = "ChatHistory")
	FString Timestamp;

	/**
	 * @brief 현재 로컬 타임스탬프를 문자열로 반환합니다.
	 *
	 * 저장 포맷을 통일해 UI/설정 로딩에서 파싱 조건을 안정화하기 위한 헬퍼입니다.
	 * @return YYYY-MM-DD HH:MM:SS 형식의 타임스탬프 문자열입니다.
	 */
	static FString CurrentTimestamp();

	/**
	 * @brief 구조체를 JSON 문자열로 변환합니다.
	 *
	 * 설정 저장/로드 시 JSON 기반 직렬화를 통일하기 위한 인터페이스입니다.
	 * @return JSON 문자열입니다.
	 */
	FString ToJson() const;

	/**
	 * @brief JSON 문자열을 구조체로 변환합니다.
	 *
	 * 저장된 문자열을 안전하게 역직렬화하기 위한 단일 진입점을 제공합니다.
	 * @param JsonString [in] JSON 문자열입니다.
	 * @param OutItem [out] 변환된 구조체입니다.
	 * @return 변환에 성공하면 true입니다.
	 */
	static bool FromJson(const FString& JsonString, FChatHistoryItem& OutItem);
};
