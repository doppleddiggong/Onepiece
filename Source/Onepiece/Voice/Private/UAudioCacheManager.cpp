// Copyright (c) 2025 Doppleddiggong. All rights reserved.
// Unauthorized copying, modification, or distribution of this file,
// via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file UAudioCacheManager.cpp
 * @brief UAudioCacheManager의 동작을 구현합니다.
 */
#include "UAudioCacheManager.h"

#include "GameLogging.h"
#include "UCommonFunctionLibrary.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "Misc/SecureHash.h"
#include "HAL/PlatformFileManager.h"

void UVoiceCacheManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// 캐시 디렉토리 생성
	FString CacheDir = GetCacheDirectory();
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	if (!PlatformFile.DirectoryExists(*CacheDir))
	{
		if (PlatformFile.CreateDirectory(*CacheDir))
		{
			PRINTLOG(TEXT("[AudioCache] Cache directory created: %s"), *CacheDir);
		}
	}
}

void UVoiceCacheManager::Deinitialize()
{
	// 메모리 캐시 정리
	MemoryCache.Empty();
	Super::Deinitialize();
}

bool UVoiceCacheManager::TryGetCachedAudio(const FString& AudioText, TArray<uint8>& OutAudioData)
{
	// 메모리 캐시 확인
	if (TArray<uint8>* Found = MemoryCache.Find(AudioText))
	{
		OutAudioData = *Found;
		return true;
	}

	// 디스크 캐시 확인
	if (LoadFromDisk(AudioText, OutAudioData))
	{
		// 메모리 캐시에도 저장 (다음번 빠른 접근)
		if (!MemoryCache.Contains(AudioText))
			MemoryCache.Add(AudioText, OutAudioData);

		return true;
	}

	return false;
}

void UVoiceCacheManager::SaveToCache(const FString& AudioText, const TArray<uint8>& AudioData)
{
	if (AudioData.Num() == 0)
	{
		PRINTLOG(TEXT("[AudioCache] Cannot save empty audio data: %s"), *AudioText);
		return;
	}

	// 메모리 캐시 저장
	if (MemoryCache.Contains(AudioText))
		// 기존 항목 제거 후 다시 추가
		MemoryCache.Remove(AudioText);
	MemoryCache.Add(AudioText, AudioData);

	// [2] 디스크 캐시 저장
	SaveToDisk(AudioText, AudioData);
}

void UVoiceCacheManager::ClearCache()
{
	// [1] 메모리 캐시 초기화
	MemoryCache.Empty();

	// [2] 디스크 캐시 초기화
	FString CacheDir = GetCacheDirectory();
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	TArray<FString> Files;
	PlatformFile.FindFiles(Files, *CacheDir, TEXT(".wav"));

	int32 DeletedCount = 0;
	for (const FString& File : Files)
	{
		FString FullPath = CacheDir / File;
		if (PlatformFile.DeleteFile(*FullPath))
		{
			DeletedCount++;
		}
	}
}


FString UVoiceCacheManager::GenerateCacheFileName(const FString& AudioText)
{
	return UCommonFunctionLibrary::GererateMD5(AudioText) + TEXT(".wav");
}

FString UVoiceCacheManager::GetCacheDirectory()
{
	return FPaths::ProjectSavedDir() / TEXT("VoiceCache");
}

bool UVoiceCacheManager::LoadFromDisk(const FString& AudioText, TArray<uint8>& OutAudioData)
{
	FString CacheFilePath = GetCacheDirectory() / GenerateCacheFileName(AudioText);
	return FFileHelper::LoadFileToArray(OutAudioData, *CacheFilePath);
}

bool UVoiceCacheManager::SaveToDisk(const FString& AudioText, const TArray<uint8>& AudioData)
{
	FString CacheDir = GetCacheDirectory();
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	if (!PlatformFile.DirectoryExists(*CacheDir))
	{
		if (!PlatformFile.CreateDirectory(*CacheDir))
		{
			return false;
		}
	}

	// 파일 저장
	FString CacheFilePath = CacheDir / GenerateCacheFileName(AudioText);
	return FFileHelper::SaveArrayToFile(AudioData, *CacheFilePath);
}