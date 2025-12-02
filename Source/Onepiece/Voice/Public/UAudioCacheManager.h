// Copyright (c) 2025 Doppleddiggong. All rights reserved.
// Unauthorized copying, modification, or distribution of this file,
// via any medium is strictly prohibited. Proprietary and confidential.

/// @file UAudioCacheManager.h
/// @brief TTS 오디오 데이터를 메모리와 디스크에 캐싱하는 서브시스템을 선언합니다.
#pragma once

#include "CoreMinimal.h"
#include "Macro.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UAudioCacheManager.generated.h"

/**
 * @brief TTS 오디오 데이터를 캐싱하여 네트워크 요청을 최소화하는 서브시스템입니다.
 *
 * 2단계 캐싱 전략:
 * - 1단계: 메모리 캐시 (TMap) - 빠른 접근
 * - 2단계: 디스크 캐시 (파일 시스템) - 영구 저장
 *
 * AudioText를 키로 사용하며, 파일명은 MD5 해시로 생성됩니다.
 */
UCLASS()
class ONEPIECE_API UVoiceCacheManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	DEFINE_SUBSYSTEM_GETTER_INLINE(UVoiceCacheManager);

	/// @brief 서브시스템 초기화 시 캐시 디렉토리를 생성합니다.
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/// @brief 서브시스템 종료 시 메모리 캐시를 정리합니다.
	virtual void Deinitialize() override;

	/**
	 * @brief 캐시에서 오디오 데이터를 조회합니다.
	 * @param AudioText [in] TTS 텍스트 (캐시 키)
	 * @param OutAudioData [out] 캐시된 오디오 데이터 (WAV 형식)
	 * @return 캐시 히트 여부 (true: 히트, false: 미스)
	 */
	bool TryGetCachedAudio(const FString& AudioText, TArray<uint8>& OutAudioData);

	/**
	 * @brief 오디오 데이터를 메모리와 디스크 캐시에 저장합니다.
	 * @param AudioText [in] TTS 텍스트 (캐시 키)
	 * @param AudioData [in] TTS 오디오 데이터 (WAV 형식)
	 */
	void SaveToCache(const FString& AudioText, const TArray<uint8>& AudioData);

	/**
	 * @brief 전체 캐시를 초기화합니다 (메모리 + 디스크).
	 */
	UFUNCTION(BlueprintCallable, Category = "Voice|Cache")
	void ClearCache();

private:
	/**
	 * @brief AudioText를 MD5 해시하여 파일명을 생성합니다.
	 * @param AudioText [in] TTS 텍스트
	 * @return MD5 해시 기반 파일명 (예: "5d41402abc4b2a76b9719d911017c592.wav")
	 */
	static FString GenerateCacheFileName(const FString& AudioText);

	/**
	 * @brief 캐시 디렉토리 경로를 반환합니다.
	 * @return 캐시 디렉토리 절대 경로 (예: "[ProjectSavedDir]/AudioCache")
	 */
	static FString GetCacheDirectory();

	/**
	 * @brief 디스크에서 오디오 데이터를 로드합니다.
	 * @param AudioText [in] TTS 텍스트
	 * @param OutAudioData [out] 로드된 오디오 데이터
	 * @return 로드 성공 여부
	 */
	static bool LoadFromDisk(const FString& AudioText, TArray<uint8>& OutAudioData);

	/**
	 * @brief 디스크에 오디오 데이터를 저장합니다.
	 * @param AudioText [in] TTS 텍스트
	 * @param AudioData [in] 저장할 오디오 데이터
	 * @return 저장 성공 여부
	 */
	static bool SaveToDisk(const FString& AudioText, const TArray<uint8>& AudioData);

private:
	/// @brief 메모리 캐시 (Key: AudioText, Value: 오디오 데이터)
	TMap<FString, TArray<uint8>> MemoryCache;
};
