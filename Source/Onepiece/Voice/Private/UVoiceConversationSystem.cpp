// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file UVoiceConversationSystem.cpp
 * @brief UVoiceConversationSystem의 동작을 구현합니다.
 */
#include "UVoiceConversationSystem.h"

#include "ALingoPlayerState.h"
#include "GameLogging.h"
#include "UBroadcastManager.h"
#include "UDialogManager.h"
#include "UVoiceFunctionLibrary.h"
#include "UGameSoundManager.h"
#include "UKLingoNetworkSystem.h"
#include "ASpeakStageActor.h"
#include "APlayerActor.h"
#include "ULingoGameHelper.h"
#include "FResultStatData.h"
#include "Sound/SoundWaveProcedural.h"
#include "GameFramework/PlayerState.h"

UVoiceConversationSystem::UVoiceConversationSystem()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UVoiceConversationSystem::InitSystem(APlayerActor* InOwner)
{
	this->Owner = InOwner;

	BroadcastManager = UBroadcastManager::Get(GetWorld());
}

void UVoiceConversationSystem::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (AudioCapture.IsValid())
		AudioCapture->CloseStream();
	Super::EndPlay(EndPlayReason);
}

// --- HTTP 방식 음성 대화 ---

void UVoiceConversationSystem::StartRecording()
{
	if (bIsRecording || bIsProcessing)
	{
		PRINTLOG( TEXT("[VoiceConversation] Already recording or processing."));
		return;
	}

	// --- SpeakStage 턴 체크 (Phase 4) ---
	if (UWorld* World = GetWorld())
	{
		if (auto SpeakStageActor = ULingoGameHelper::GetSpeakStageActor(World))
		{
			// 로컬 플레이어의 PlayerState 가져오기
			APlayerState* LocalPlayerState = nullptr;
			if (Owner)
			{
				APlayerController* PC = Cast<APlayerController>(Owner->GetController());
				if (PC)
				{
					LocalPlayerState = PC->GetPlayerState<APlayerState>();
				}
			}

			// 현재 발화자 확인
			APlayerState* CurrentSpeaker = SpeakStageActor->GetCurrentSpeaker();

			if (!CurrentSpeaker)
			{
				PRINTLOG(TEXT("[VoiceConversation] Recording blocked: Stage not started."));

				if (auto DM = UDialogManager::Get(World))
				{
					DM->ShowToast(TEXT("NPC와 대화하여 심사를 시작하세요."));
				}
				return;
			}

			// 내 턴이 아니면 녹음 차단
			if (LocalPlayerState && CurrentSpeaker && CurrentSpeaker != LocalPlayerState)
			{
				PRINTLOG(TEXT("[VoiceConversation] Recording blocked: Not your turn (Current: %s)"),
					*CurrentSpeaker->GetPlayerName());

				if (auto DM = UDialogManager::Get(World))
				{
					DM->ShowToast(FString::Printf(TEXT("지금은 %s님의 차례입니다"), *CurrentSpeaker->GetPlayerName()));
				}
				return;
			}
		}
	}
	// --- 턴 체크 종료 ---

	// 재생 중인 대화 음성이 있으면 정지 (UGameSoundManager 사용)
	if (UWorld* World = GetWorld())
	{
		if (auto SoundManager = UGameSoundManager::Get(World))
		{
			if (SoundManager->IsConversationVoicePlaying())
			{
				SoundManager->StopConversationVoice();

				// 타이머 정리
				World->GetTimerManager().ClearTimer(VoiceFinishTimerHandle);

				OnVoiceAudioFinished(); // 수동으로 호출하여 이전 상태를 정리합니다.
			}
		}
	}

	PCMData.Reset();

	if (!AudioCapture)
		AudioCapture = MakeUnique<Audio::FAudioCapture>();

	// 사용 가능한 오디오 디바이스 목록 확인
	TArray<Audio::FCaptureDeviceInfo> DeviceInfos;
	AudioCapture->GetCaptureDevicesAvailable(DeviceInfos);

	PRINTLOG(TEXT("[VoiceConversation] Available Audio Devices:"));
	for (int32 i = 0; i < DeviceInfos.Num(); ++i)
	{
		PRINTLOG(TEXT("  [%d] %s (Channels: %d, SampleRate: %d, bSupportsHardwareAEC: %d)"),
			i,
			*DeviceInfos[i].DeviceName,
			DeviceInfos[i].InputChannels,
			DeviceInfos[i].PreferredSampleRate,
			DeviceInfos[i].bSupportsHardwareAEC ? 1 : 0);
	}

	Audio::FAudioCaptureDeviceParams Params;
	Params.DeviceIndex = 0;  // TODO: 사용자가 선택할 수 있도록 개선 필요
	Params.NumInputChannels = 1;

	const bool bStreamOpened = AudioCapture->OpenAudioCaptureStream(
		Params,
		[this](const void* InAudio, int32 NumFrames, int32 InNumChannels, int32 InSampleRate, double StreamTime, bool bOverFlow)
		{
			// 첫 캡처 시 샘플레이트 로그 출력
			static bool bLoggedOnce = false;
			if (!bLoggedOnce)
			{
				PRINTLOG(TEXT("[VoiceConversation] Audio Capture Settings: SampleRate=%d, Channels=%d, Frames=%d"),
					InSampleRate, InNumChannels, NumFrames);
				bLoggedOnce = true;
			}
			HandleOnCapture(static_cast<const float*>(InAudio), NumFrames, InNumChannels, InSampleRate);
		},
		1024  // 버퍼 크기 증가 (512 → 1024) - 더 안정적인 녹음
	);

	if (!bStreamOpened || !AudioCapture->StartStream() )
	{
		if (auto DM = UDialogManager::Get(GetWorld()))
		{
			DM->ShowToast(TEXT("연결된 마이크가 없습니다"));
		}

		return;
	}

	// 스트림 시작 성공 후에 녹음 플래그 설정
	bIsRecording = true;

	BroadcastManager->SendAudioCapture(true);
	PRINTLOG( TEXT("[VoiceConversation] Recording started."));
}


void UVoiceConversationSystem::HandleOnCapture(const float* InAudio, int32 InNumFrames, int32 InNumChannels, int32 InSampleRate)
{
	LastSampleRate  = InSampleRate;
	LastNumChannels = InNumChannels;
	
	const int32 SampleCount = InNumFrames * InNumChannels;
	PCMData.Reserve(PCMData.Num() + SampleCount * sizeof(int16));

	float TotalVolume = 0.f;
	
	for (int32 i = 0; i < SampleCount; ++i)
	{
		float Sample = InAudio[i];
		Sample = FMath::Clamp(Sample, -1.0f, 1.0f);

		TotalVolume += FMath::Abs(Sample);

		int16 Int16Sample = static_cast<int16>(Sample * 32767.0f);
		const uint8* SampleBytes = reinterpret_cast<const uint8*>(&Int16Sample);

		PCMData.Append(SampleBytes, sizeof(int16));
	}

	const float AverageVolume = (SampleCount > 0) ? (TotalVolume / SampleCount) : 0.f;
	const float AmplifiedVolume = AverageVolume * 10.f;
	
	BroadcastManager->SendAudioSpectrum(FMath::Clamp(AmplifiedVolume, 0.f, 1.f));
}

void UVoiceConversationSystem::StopRecording()
{
	if (!bIsRecording)
	{
		PRINTLOG( TEXT("[VoiceConversation] Not currently recording."));
		return;
	}

	bIsRecording = false;
	bIsProcessing = true;

	AudioCapture->StopStream();
	AudioCapture->CloseStream();

	BroadcastManager->SendAudioCapture(false);

	PRINTLOG(TEXT("[VoiceConversation] Recording stopped. Original: SampleRate=%d, Channels=%d, PCM Size=%d bytes"),
		LastSampleRate, LastNumChannels, PCMData.Num());

	// STT 최적화: 16kHz로 리샘플링
	TArray<uint8> ProcessedPCM = PCMData;
	int32 TargetSampleRate = 16000;

	if (LastSampleRate != TargetSampleRate)
	{
		PRINTLOG(TEXT("[VoiceConversation] Resampling from %dHz to %dHz..."), LastSampleRate, TargetSampleRate);
		ProcessedPCM = UVoiceFunctionLibrary::ResampleAudio(PCMData, LastSampleRate, TargetSampleRate, LastNumChannels);
		PRINTLOG(TEXT("[VoiceConversation] Resampled PCM Size=%d bytes"), ProcessedPCM.Num());
	}

	WAVData = UVoiceFunctionLibrary::ConvertPCM2WAV(ProcessedPCM, TargetSampleRate, LastNumChannels, 16);
	LastRecordedFilePath = UVoiceFunctionLibrary::SaveWavToFile(WAVData);

	PRINTLOG(TEXT("[VoiceConversation] Recording saved to: %s"), *LastRecordedFilePath);
	if (LastRecordedFilePath.IsEmpty())
	{
		PRINTLOG( TEXT("[VoiceConversation] FilePath is Empty") );
		return;
	}
	
	UKLingoNetworkSystem* HttpSystem = UKLingoNetworkSystem::Get(GetWorld());
	if (!HttpSystem)
	{
		PRINTLOG( TEXT("HttpSystem을 찾을 수 없습니다."));
		bIsProcessing = false;
		return;
	}

	FString Question;
	if ( auto SpeakStageActor = ULingoGameHelper::GetSpeakStageActor(GetWorld()) )
		Question = SpeakStageActor->GetCurrentQuestion();

	HttpSystem->RequestSpeakingJudges(
		Question,
		LastRecordedFilePath,
		FResponseSpeakingJudesDelegate::CreateUObject(this, &UVoiceConversationSystem::OnResponseSpeakingsJudges));
}

void UVoiceConversationSystem::OnResponseSpeakingsJudges(FResponseSpeakingJudes& Response, bool bSuccess)
{
	bIsProcessing = false;

	if (bSuccess)
	{
		PRINTLOG( TEXT("--- Network Response Received : %s"), *Response.final_feedback);

		// Broadcast final_feedback to UTutorMessage
		if (BroadcastManager)
		{
			BroadcastManager->SendTutorMessage(FText::FromString(Response.final_feedback));
			BroadcastManager->SendAddItemToBoxList(	Response.GetResultStatData());
		}

		if (UWorld* World = GetWorld())
		{
			if ( auto SpeakStageActor = ULingoGameHelper::GetSpeakStageActor(World) )
			{
				if (auto LocalPlayerState = ULingoGameHelper::GetLingoPlayerStateByPC(Owner->GetController()))
				{
					// Store evaluation result in PlayerState
					LocalPlayerState->Server_AddSpeakJudes(Response);

					SpeakStageActor->ServerRPC_NotifyAnswerComplete(LocalPlayerState);
				}
			}
		}
	}
	else
	{
		PRINTLOG( TEXT("--- Network Response Received (FAIL) ---"));
	}
}

bool UVoiceConversationSystem::PlayVoiceAudio(const TArray<uint8>& AudioData)
{
	// 녹음 중일 때는 TTS 재생 차단
	if (bIsRecording)
	{
		PRINTLOG(TEXT("[VoiceConversation] TTS playback blocked: recording in progress"));
		return false;
	}

	// 오디오 데이터가 없으면 재생 불가
	if (AudioData.Num() == 0)
	{
		PRINTLOG(TEXT("[VoiceConversation] TTS playback failed: empty audio data"));
		return false;
	}

	// SoundWave 생성 (Procedural 사용)
	auto SoundWave = UVoiceFunctionLibrary::CreateProceduralSoundWaveFromWavData(AudioData);
	if (!IsValid(SoundWave))
	{
		PRINTLOG(TEXT("[VoiceConversation] TTS playback failed: invalid sound wave"));
		return false;
	}

	// UGameSoundManager를 통해 대화 음성 재생 (기존 음성 자동 중지)
	auto SoundManager = UGameSoundManager::Get(GetWorld());
	if (!SoundManager)
	{
		PRINTLOG(TEXT("[VoiceConversation] TTS playback failed: could not get sound manager"));
		return false;
	}

	CurVoiceAudio = SoundManager->PlayConversationVoice(SoundWave);
	if (!CurVoiceAudio)
	{
		PRINTLOG(TEXT("[VoiceConversation] TTS playback failed: could not create audio component"));
		return false;
	}

	// Duration 기반 타이머로 재생 완료 감지
	const float Duration = SoundWave->Duration;
	PRINTLOG(TEXT("[VoiceConversation] TTS audio playing (duration: %.2f seconds)"), Duration);

	// 기존 타이머 정리
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(VoiceFinishTimerHandle);

		// Duration + 여유 시간(0.1초) 후에 OnVoiceAudioFinished 호출
		GetWorld()->GetTimerManager().SetTimer(
			VoiceFinishTimerHandle,
			this,
			&UVoiceConversationSystem::OnVoiceAudioFinished,
			Duration + 0.1f,
			false
		);
	}

	return true;
}

void UVoiceConversationSystem::OnVoiceAudioFinished()
{
	PRINTLOG(TEXT("[VoiceConversation] TTS audio playback finished"));

	// 타이머 정리
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(VoiceFinishTimerHandle);
	}

	PRINTLOG(TEXT("[VoiceConversation] TTS 재생 완료 이벤트 발생"));

	// AudioComponent 참조 초기화 (실제 파괴는 UGameSoundManager가 관리)
	CurVoiceAudio = nullptr;
}