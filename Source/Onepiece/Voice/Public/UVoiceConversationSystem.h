// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/// @file UVoiceConversationSystem.h
/// @brief STT·GPT·TTS 파이프라인을 연결하는 음성 대화 컴포넌트를 선언합니다.
#pragma once

#include "CoreMinimal.h"
#include "NetworkData.h"
#include "AudioCapture.h"
#include "Components/ActorComponent.h"
#include "UVoiceConversationSystem.generated.h"

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Voice), meta=(BlueprintSpawnableComponent))
class ONEPIECE_API UVoiceConversationSystem : public UActorComponent
{
    GENERATED_BODY()

public:
    /// @brief 기본 속성을 설정하는 생성자입니다.
    UVoiceConversationSystem();

protected:
    /// @brief 녹음 장치와 델리게이트를 해제합니다.
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
    /// @brief 소유 플레이어와 브로드캐스트 매니저를 연결합니다.
    /// @param InOwner [in] 음성 대화를 제어하는 플레이어 액터입니다.
    void InitSystem(class APlayerActor* InOwner);

    /// @brief 음성 녹음을 시작하고 버퍼를 초기화합니다.
    UFUNCTION(BlueprintCallable, Category = "Voice|Conversation")
    void StartRecording();

    /// @brief 녹음을 중지하고 ASK 요청을 전송합니다.
    UFUNCTION(BlueprintCallable, Category = "Voice|Conversation")
    void StopRecording();

    /// @brief TTS 오디오를 재생합니다. 녹음 중일 때는 재생하지 않습니다.
    /// @param AudioData [in] TTS로 생성된 오디오 데이터 (WAV)
    /// @return 재생 성공 여부 (녹음 중이면 false)
    UFUNCTION(BlueprintCallable, Category = "Voice|Conversation")
    bool PlayVoiceAudio(const TArray<uint8>& AudioData);

    /// @brief TTS 오디오 재생 완료 시 호출되는 콜백입니다.
    UFUNCTION()
    void OnVoiceAudioFinished();

    /// @brief 현재 녹음 중인지 확인합니다.
    UFUNCTION(BlueprintPure, Category = "Voice|Conversation")
    bool IsRecording() const { return bIsRecording; }

private:
    /// @brief 오디오 캡처 콜백에서 호출되어 버퍼를 누적합니다.
    /// @param InAudio [in] 캡처된 PCM 데이터입니다.
    /// @param InNumFrames [in] 프레임 수입니다.
    /// @param InNumChannels [in] 채널 수입니다.
    /// @param InSampleRate [in] 샘플레이트입니다.
    void HandleOnCapture(const float* InAudio, int32 InNumFrames, int32 InNumChannels, int32 InSampleRate);

    UFUNCTION()
    void OnResponseSpeakingsJudges(FResponseSpeakingJudes& Response, bool bSuccess);

private:
    UPROPERTY()
    TObjectPtr<class APlayerActor> Owner;

    UPROPERTY()
    TObjectPtr<class UBroadcastManager> BroadcastManager;

    // --- 녹음 관련 변수 ---
    TUniquePtr<Audio::FAudioCapture> AudioCapture;

    TArray<uint8> WAVData;
    TArray<uint8> PCMData;

    int32 LastSampleRate = 16000;
    int32 LastNumChannels = 1;
    FString LastRecordedFilePath;

    // --- 상태 변수 ---
    bool bIsRecording = false;
    bool bIsProcessing = false;

    /** @brief 현재 재생 중인 TTS 오디오 컴포넌트입니다. */
    UPROPERTY()
    TObjectPtr<class UAudioComponent> CurVoiceAudio;

    /** @brief TTS 재생 완료를 감지하는 타이머 핸들입니다. */
    FTimerHandle VoiceFinishTimerHandle;
};
