// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UVoiceFunctionLibrary.generated.h"

/// @file UVoiceFunctionLibrary.h
/// @brief 음성 데이터 처리와 명령 파싱을 위한 블루프린트 함수 라이브러리를 선언합니다.

/// @brief 음성 파일 변환과 명령 문자열 파싱 유틸리티를 제공합니다.
UCLASS()
class ONEPIECE_API UVoiceFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /// @brief PCM 데이터를 WAV 포맷으로 감쌉니다.
    /// @param InPCMData [in] 원본 PCM 바이트 배열입니다.
    /// @param InSampleRate [in] 샘플레이트(Hz)입니다.
    /// @param InChannel [in] 채널 수입니다.
    /// @param InBitsPerSample [in] 샘플당 비트 수입니다.
    /// @return WAV 헤더가 포함된 바이트 배열입니다.
    UFUNCTION(BlueprintCallable, Category="Voice|Utility")
    static TArray<uint8> ConvertPCM2WAV(
        const TArray<uint8>& InPCMData,
        int32 InSampleRate,
        int32 InChannel,
        int32 InBitsPerSample
    );

    /// @brief WAV 데이터를 파일로 저장합니다.
    /// @param InWavData [in,out] 저장할 WAV 데이터입니다.
    /// @param InFileName [in] 저장할 파일 이름(비어있으면 임시 파일 생성)입니다.
    /// @return 실제 저장된 파일 경로입니다.
    UFUNCTION(BlueprintCallable, Category="Voice|Utility")
    static FString SaveWavToFile(TArray<uint8>& InWavData,
        const FString& InFileName = TEXT(""));

    /// @brief WAV 데이터를 기반으로 사운드 웨이브 객체를 생성합니다.
    /// @param WavData [in] WAV 형식의 바이트 배열입니다.
    /// @return 재생 가능한 USoundWave 객체입니다.
    UFUNCTION(BlueprintCallable, Category = "Voice|Utility")
    static USoundWave* CreateSoundWaveFromWavData(const TArray<uint8>& WavData);

    /// @brief 절차형 사운드 웨이브를 생성해 스트리밍 재생에 사용합니다.
    /// @param AudioData [in] PCM 데이터입니다.
    /// @return 생성된 USoundWaveProcedural 객체입니다.
    UFUNCTION(BlueprintCallable, Category = "Voice|Utility")
    static USoundWaveProcedural* CreateProceduralSoundWaveFromWavData(const TArray<uint8>& AudioData);

    /// @brief PCM 오디오 데이터를 다른 샘플레이트로 리샘플링합니다.
    /// @param InPCMData [in] 원본 PCM 데이터 (16bit)
    /// @param InSampleRate [in] 원본 샘플레이트
    /// @param OutSampleRate [in] 목표 샘플레이트
    /// @param InNumChannels [in] 채널 수
    /// @return 리샘플링된 PCM 데이터
    UFUNCTION(BlueprintCallable, Category="Voice|Utility")
    static TArray<uint8> ResampleAudio(const TArray<uint8>& InPCMData, int32 InSampleRate, int32 OutSampleRate, int32 InNumChannels);
};
