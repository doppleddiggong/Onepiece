// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file UConfigLibrary.h
 * @brief GConfig 래퍼 설정 관리 시스템
 * @details GConfig를 래핑하여 간단한 Get/Set API 제공
 *
 * **사용 예시:**
 * @code
 * // Global Settings
 * UConfigLibrary::SetInt(TEXT("MasterVolume"), 80);
 * int32 Volume = UConfigLibrary::GetInt(TEXT("MasterVolume"), 100);
 *
 * // User-Specific Settings
 * const int32 UserId = ULingoGameHelper::GetUserId(this);
 * UConfigLibrary::SetUserBool(UserId, TEXT("TutorialCompleted"), true);
 * bool bCompleted = UConfigLibrary::GetUserBool(UserId, TEXT("TutorialCompleted"), false);
 * @endcode
 */

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UConfigLibrary.generated.h"

/**
 * @brief GConfig 래퍼 라이브러리
 * @details 간단한 Get/Set 인터페이스로 설정을 저장/로드합니다.
 *
 * **특징:**
 * - Global Settings: 전역 설정 (볼륨, 그래픽 등)
 * - User-Specific Settings: 유저별 설정 (튜토리얼 완료, 진행 상황 등)
 * - 자동 저장: bAutoSave=true일 때 즉시 디스크에 기록
 * - Blueprint 지원: 모든 함수 Blueprint에서 사용 가능
 */
UCLASS()
class COFFEELIBRARY_API UConfigLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// =====================================================================
	// INTEGER OPERATIONS - GLOBAL
	// =====================================================================

	/// @brief 전역 정수 설정 읽기
	/// @param Key 설정 키 (예: "MasterVolume")
	/// @param DefaultValue 키가 없을 때 반환할 기본값
	/// @return 저장된 정수 값 또는 기본값
	UFUNCTION(BlueprintPure, Category = "Config|Global")
	static int32 GetInt(const FString& Key, int32 DefaultValue = 0);

	/// @brief 전역 정수 설정 저장
	/// @param Key 설정 키
	/// @param Value 저장할 값
	/// @param bAutoSave true일 경우 즉시 디스크에 저장 (기본값: true)
	UFUNCTION(BlueprintCallable, Category = "Config|Global")
	static void SetInt(const FString& Key, int32 Value, bool bAutoSave = true);

	// =====================================================================
	// FLOAT OPERATIONS - GLOBAL
	// =====================================================================

	/// @brief 전역 실수 설정 읽기
	/// @param Key 설정 키 (예: "MusicVolume")
	/// @param DefaultValue 키가 없을 때 반환할 기본값
	/// @return 저장된 실수 값 또는 기본값
	UFUNCTION(BlueprintPure, Category = "Config|Global")
	static float GetFloat(const FString& Key, float DefaultValue = 0.0f);

	/// @brief 전역 실수 설정 저장
	/// @param Key 설정 키
	/// @param Value 저장할 값
	/// @param bAutoSave true일 경우 즉시 디스크에 저장 (기본값: true)
	UFUNCTION(BlueprintCallable, Category = "Config|Global")
	static void SetFloat(const FString& Key, float Value, bool bAutoSave = true);

	// =====================================================================
	// STRING OPERATIONS - GLOBAL
	// =====================================================================

	/// @brief 전역 문자열 설정 읽기
	/// @param Key 설정 키 (예: "Language")
	/// @param DefaultValue 키가 없을 때 반환할 기본값
	/// @return 저장된 문자열 또는 기본값
	UFUNCTION(BlueprintPure, Category = "Config|Global")
	static FString GetString(const FString& Key, const FString& DefaultValue = TEXT(""));

	/// @brief 전역 문자열 설정 저장
	/// @param Key 설정 키
	/// @param Value 저장할 값
	/// @param bAutoSave true일 경우 즉시 디스크에 저장 (기본값: true)
	UFUNCTION(BlueprintCallable, Category = "Config|Global")
	static void SetString(const FString& Key, const FString& Value, bool bAutoSave = true);

	// =====================================================================
	// BOOLEAN OPERATIONS - GLOBAL
	// =====================================================================

	/// @brief 전역 불린 설정 읽기
	/// @param Key 설정 키 (예: "ShowFPS")
	/// @param bDefaultValue 키가 없을 때 반환할 기본값
	/// @return 저장된 불린 값 또는 기본값
	/// @note 내부적으로 int로 저장됨 (0=false, 1=true)
	UFUNCTION(BlueprintPure, Category = "Config|Global")
	static bool GetBool(const FString& Key, bool bDefaultValue = false);

	/// @brief 전역 불린 설정 저장
	/// @param Key 설정 키
	/// @param bValue 저장할 값
	/// @param bAutoSave true일 경우 즉시 디스크에 저장 (기본값: true)
	/// @note 내부적으로 int로 저장됨 (0=false, 1=true)
	UFUNCTION(BlueprintCallable, Category = "Config|Global")
	static void SetBool(const FString& Key, bool bValue, bool bAutoSave = true);

	// =====================================================================
	// INTEGER OPERATIONS - USER-SPECIFIC
	// =====================================================================

	/// @brief 유저별 정수 설정 읽기
	/// @param UserId 사용자 ID (ULingoGameHelper::GetUserId()로 획득)
	/// @param Key 설정 키 (예: "TutorialStep")
	/// @param DefaultValue 키가 없을 때 반환할 기본값
	/// @return 저장된 정수 값 또는 기본값
	UFUNCTION(BlueprintPure, Category = "Config|User")
	static int32 GetUserInt(int32 UserId, const FString& Key, int32 DefaultValue = 0);

	/// @brief 유저별 정수 설정 저장
	/// @param UserId 사용자 ID
	/// @param Key 설정 키
	/// @param Value 저장할 값
	/// @param bAutoSave true일 경우 즉시 디스크에 저장 (기본값: true)
	UFUNCTION(BlueprintCallable, Category = "Config|User")
	static void SetUserInt(int32 UserId, const FString& Key, int32 Value, bool bAutoSave = true);

	// =====================================================================
	// FLOAT OPERATIONS - USER-SPECIFIC
	// =====================================================================

	/// @brief 유저별 실수 설정 읽기
	/// @param UserId 사용자 ID
	/// @param Key 설정 키
	/// @param DefaultValue 키가 없을 때 반환할 기본값
	/// @return 저장된 실수 값 또는 기본값
	UFUNCTION(BlueprintPure, Category = "Config|User")
	static float GetUserFloat(int32 UserId, const FString& Key, float DefaultValue = 0.0f);

	/// @brief 유저별 실수 설정 저장
	/// @param UserId 사용자 ID
	/// @param Key 설정 키
	/// @param Value 저장할 값
	/// @param bAutoSave true일 경우 즉시 디스크에 저장 (기본값: true)
	UFUNCTION(BlueprintCallable, Category = "Config|User")
	static void SetUserFloat(int32 UserId, const FString& Key, float Value, bool bAutoSave = true);

	// =====================================================================
	// STRING OPERATIONS - USER-SPECIFIC
	// =====================================================================

	/// @brief 유저별 문자열 설정 읽기
	/// @param UserId 사용자 ID
	/// @param Key 설정 키 (예: "InterviewSkipDate")
	/// @param DefaultValue 키가 없을 때 반환할 기본값
	/// @return 저장된 문자열 또는 기본값
	UFUNCTION(BlueprintPure, Category = "Config|User")
	static FString GetUserString(int32 UserId, const FString& Key, const FString& DefaultValue = TEXT(""));

	/// @brief 유저별 문자열 설정 저장
	/// @param UserId 사용자 ID
	/// @param Key 설정 키
	/// @param Value 저장할 값
	/// @param bAutoSave true일 경우 즉시 디스크에 저장 (기본값: true)
	UFUNCTION(BlueprintCallable, Category = "Config|User")
	static void SetUserString(int32 UserId, const FString& Key, const FString& Value, bool bAutoSave = true);

	// =====================================================================
	// BOOLEAN OPERATIONS - USER-SPECIFIC
	// =====================================================================

	/// @brief 유저별 불린 설정 읽기
	/// @param UserId 사용자 ID
	/// @param Key 설정 키 (예: "TutorialCompleted")
	/// @param bDefaultValue 키가 없을 때 반환할 기본값
	/// @return 저장된 불린 값 또는 기본값
	/// @note 내부적으로 int로 저장됨 (0=false, 1=true)
	UFUNCTION(BlueprintPure, Category = "Config|User")
	static bool GetUserBool(int32 UserId, const FString& Key, bool bDefaultValue = false);

	/// @brief 유저별 불린 설정 저장
	/// @param UserId 사용자 ID
	/// @param Key 설정 키
	/// @param bValue 저장할 값
	/// @param bAutoSave true일 경우 즉시 디스크에 저장 (기본값: true)
	/// @note 내부적으로 int로 저장됨 (0=false, 1=true)
	UFUNCTION(BlueprintCallable, Category = "Config|User")
	static void SetUserBool(int32 UserId, const FString& Key, bool bValue, bool bAutoSave = true);

	// =====================================================================
	// JSON OPERATIONS - USER-SPECIFIC
	// =====================================================================

	/// @brief 유저별 JSON 데이터 저장
	/// @param UserId 사용자 ID
	/// @param Key 설정 키 (예: "ChatHistory")
	/// @param JsonData JSON 문자열
	/// @param bAutoSave true일 경우 즉시 디스크에 저장 (기본값: true)
	/// @details 구조체 배열을 FJsonObjectConverter로 직렬화한 후 저장
	UFUNCTION(BlueprintCallable, Category = "Config|User|JSON")
	static void SetUserJson(int32 UserId, const FString& Key, const FString& JsonData, bool bAutoSave = true);

	/// @brief 유저별 JSON 데이터 읽기
	/// @param UserId 사용자 ID
	/// @param Key 설정 키
	/// @param DefaultJson 키가 없을 때 반환할 기본 JSON (빈 배열: "[]")
	/// @return 저장된 JSON 문자열
	/// @details FJsonObjectConverter로 역직렬화하여 사용
	UFUNCTION(BlueprintPure, Category = "Config|User|JSON")
	static FString GetUserJson(int32 UserId, const FString& Key, const FString& DefaultJson = TEXT("[]"));

	// =====================================================================
	// KEY MANAGEMENT
	// =====================================================================

	/// @brief 전역 키 존재 여부 확인
	/// @param Key 확인할 키
	/// @return 키가 존재하면 true, 없으면 false
	UFUNCTION(BlueprintPure, Category = "Config|Management")
	static bool HasKey(const FString& Key);

	/// @brief 유저별 키 존재 여부 확인
	/// @param UserId 사용자 ID
	/// @param Key 확인할 키
	/// @return 키가 존재하면 true, 없으면 false
	UFUNCTION(BlueprintPure, Category = "Config|Management")
	static bool HasUserKey(int32 UserId, const FString& Key);

	/// @brief 전역 키 삭제
	/// @param Key 삭제할 키
	/// @param bAutoSave true일 경우 즉시 디스크에 반영 (기본값: true)
	UFUNCTION(BlueprintCallable, Category = "Config|Management")
	static void DeleteKey(const FString& Key, bool bAutoSave = true);

	/// @brief 유저별 키 삭제
	/// @param UserId 사용자 ID
	/// @param Key 삭제할 키
	/// @param bAutoSave true일 경우 즉시 디스크에 반영 (기본값: true)
	UFUNCTION(BlueprintCallable, Category = "Config|Management")
	static void DeleteUserKey(int32 UserId, const FString& Key, bool bAutoSave = true);

	/// @brief 모든 전역 설정 삭제 (주의!)
	/// @param bAutoSave true일 경우 즉시 디스크에 반영 (기본값: true)
	/// @warning 모든 전역 설정이 삭제됩니다. 신중하게 사용하세요.
	UFUNCTION(BlueprintCallable, Category = "Config|Management")
	static void DeleteAll(bool bAutoSave = true);

	/// @brief 특정 유저의 모든 설정 삭제
	/// @param UserId 사용자 ID
	/// @param bAutoSave true일 경우 즉시 디스크에 반영 (기본값: true)
	/// @warning 해당 유저의 모든 설정이 삭제됩니다. 신중하게 사용하세요.
	UFUNCTION(BlueprintCallable, Category = "Config|Management")
	static void DeleteAllUserData(int32 UserId, bool bAutoSave = true);

	/// @brief 대기 중인 모든 변경사항을 디스크에 저장
	/// @details bAutoSave=false로 저장한 경우, 이 함수로 수동 저장
	UFUNCTION(BlueprintCallable, Category = "Config|Management")
	static void Save();

private:
	// =====================================================================
	// INTERNAL HELPERS
	// =====================================================================

	/// @brief 전역 설정 Config Section 이름 반환
	/// @return "/Script/CoffeeLibrary.Config"
	static FString GetGlobalSection();

	/// @brief 유저별 설정 Config Section 이름 반환
	/// @return "/Script/CoffeeLibrary.UserConfig"
	static FString GetUserSection();

	/// @brief 유저별 키 생성 (UserId 접두사 추가)
	/// @param UserId 사용자 ID
	/// @param Key 원본 키
	/// @return "User_{UserId}_{Key}" 형식의 키
	static FString MakeUserKey(int32 UserId, const FString& Key);
};
