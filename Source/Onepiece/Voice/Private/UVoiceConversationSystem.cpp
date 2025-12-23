// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file UVoiceConversationSystem.cpp
 * @brief UVoiceConversationSystem의 동작을 구현합니다.
 */
#include "UVoiceConversationSystem.h"

#include "ALingoPlayerState.h"
#include "GameLogging.h"
#include "ULingoGameHelper.h"
#include "UBroadcastManager.h"
#include "UDialogManager.h"
#include "UVoiceFunctionLibrary.h"
#include "UGameSoundManager.h"
#include "UKLingoNetworkSystem.h"
#include "ASpeakStageActor.h"
#include "APlayerActor.h"
#include "APlayerControl.h"
#include "UChatHistorySystem.h"
#include "UPopupManager.h"
#include "UPopup_SpeakJudes.h"
#include "Sound/SoundWaveProcedural.h"
#include "GameFramework/PlayerState.h"
#include "Onepiece/Onepiece.h"

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

	// STT 최적화: 스테레오 → 모노 변환
	TArray<uint8> ProcessedPCM = PCMData;
	int32 ProcessedChannels = LastNumChannels;

	if (LastNumChannels == 2)
	{
		ProcessedPCM = UVoiceFunctionLibrary::ConvertStereoToMono(PCMData);
		ProcessedChannels = 1;  // 모노로 변경
	}

	// STT 최적화: 16kHz로 리샘플링
	int32 TargetSampleRate = 16000;

	if (LastSampleRate != TargetSampleRate)
	{
		ProcessedPCM = UVoiceFunctionLibrary::ResampleAudio(ProcessedPCM, LastSampleRate, TargetSampleRate, ProcessedChannels);
	}

	WAVData = UVoiceFunctionLibrary::ConvertPCM2WAV(ProcessedPCM, TargetSampleRate, ProcessedChannels, 16);
	LastRecordedFilePath = UVoiceFunctionLibrary::SaveWavToFile(WAVData);

	PRINTLOG(TEXT("[VoiceConversation] Recording saved to: %s"), *LastRecordedFilePath);
	if (LastRecordedFilePath.IsEmpty())
	{
		PRINTLOG( TEXT("[VoiceConversation] FilePath is Empty") );
		return;
	}
	
	UKLingoNetworkSystem* KLingoNetwork = UKLingoNetworkSystem::Get(GetWorld());
	if (!KLingoNetwork)
	{
		PRINTLOG( TEXT("HttpSystem을 찾을 수 없습니다."));
		bIsProcessing = false;
		return;
	}

	// SpeakStageActor 존재 여부와 CurrentSpeaker 확인
	auto SpeakStageActor = ULingoGameHelper::GetSpeakStageActor(GetWorld());
	bool bUseSpeakJudges = false;

	if (SpeakStageActor)
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

		// CurrentSpeaker 확인
		ALingoPlayerState* CurrentSpeaker = SpeakStageActor->GetCurrentSpeaker();

		// SpeakStageActor가 있고 CurrentSpeaker가 나라면 SpeakJudges 사용
		if (CurrentSpeaker && LocalPlayerState && CurrentSpeaker == LocalPlayerState)
		{
			bUseSpeakJudges = true;
		}
	}

	if (bUseSpeakJudges)
	{
		// Toast 메시지 표시: 답변 분석 중
		if (UDialogManager* DM = UDialogManager::Get(GetWorld()))
		{
			DM->ShowToast(TEXT("The officer is reviewing your answer"));
		}

		KLingoNetwork->RequestSpeakingJudges(
			SpeakStageActor->GetCurrentQuestion(),
			LastRecordedFilePath,
			FResponseSpeakingJudesDelegate::CreateUObject(this, &UVoiceConversationSystem::OnResponseSpeakingsJudges));
	}
	else
	{
		// Toast 메시지 표시: 답변 분석 중
		if (UDialogManager* DM = UDialogManager::Get(GetWorld()))
		{
			DM->ShowToast(TEXT("Processing your voice message..."));
		}

		// PlayerState에서 Chat Context 가져오기
		FString ChatContext = TEXT("You are a helpful assistant."); // 기본값
		if (Owner)
		{
			APlayerController* PC = Cast<APlayerController>(Owner->GetController());
			if (PC)
			{
				if (auto PS = PC->GetPlayerState<ALingoPlayerState>())
				{
					ChatContext = PS->GetChatContext();
				}
			}
		}

		// 일반 대화 모드: RequestChatAnswers 사용
		KLingoNetwork->RequestChatAudio(
			ChatContext,
			LastRecordedFilePath,
			FResponseChatAnswersDelegate::CreateUObject(this, &UVoiceConversationSystem::OnResponseChatAnswers));
	}
}

void UVoiceConversationSystem::OnResponseSpeakingsJudges(FResponseSpeakingJudes& Response, bool bSuccess)
{
	bIsProcessing = false;

	if (bSuccess)
	{
		// 팝업 표시
		if (auto Popup = UPopupManager::ShowPopupAs<UPopup_SpeakJudes>(GetWorld(), EPopupType::SpeakJudes))
		{
			Popup->InitPopup(Response);
		}

		// PlayerActor의 Server RPC 호출 (PlayerActor는 Client 소유!)
		if (Owner)
			Owner->Server_NotifySpeakJudgeComplete(Response);
	}
	else
	{
		PRINTLOG( TEXT("--- Network Response Received (FAIL) ---"));
	}
}

void UVoiceConversationSystem::OnResponseChatAnswers(FResponseChatAnswers& Response, bool bSuccess)
{
	bIsProcessing = false;

	if (bSuccess)
	{
		// 메시지로 답변 표시
		if (auto* GS = GetWorld()->GetGameState<ALingoGameState>())
		{
			APlayerControl* PC = Cast<APlayerControl>(Owner->GetController());
			if ( PC != nullptr )
			{
				// Chat History 저장
				PC->GetChatHistorySystem()->SaveChatHistory(Response.question, Response.answer);
				
				FText PlayerQuestion  = FText::FromString(Response.question);
				// Bot은 PlayerIndex -1 사용
				GS->MulticastRPC_SendChat(PC->GetUserInfo(), PlayerQuestion, PC->GetUserId());

				FText AIAnswer = FText::FromString(Response.answer);
				// Bot은 PlayerIndex -1 사용
				GS->MulticastRPC_SendChat(GS->GetBotInfo(), AIAnswer, GameName::BotID);
			}

			PRINTLOG(TEXT("[AI Chat] AI Answer: %s"), *Response.answer);
		}
	}
	else
	{
		PRINTLOG( TEXT("--- Chat Answers Response Received (FAIL) ---"));
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
		return false;
	}

	// Duration 기반 타이머로 재생 완료 감지
	const float Duration = SoundWave->Duration;
	PRINTLOG(TEXT("[VoiceConversation] TTS audio playing (duration: %.2f seconds)"), Duration);

	// 기존 타이머 정리
	if (auto World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(VoiceFinishTimerHandle);

		// Duration + 여유 시간(0.1초) 후에 OnVoiceAudioFinished 호출
		World->GetTimerManager().SetTimer(
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
	// 타이머 정리
	if (auto World = GetWorld())
		World->GetTimerManager().ClearTimer(VoiceFinishTimerHandle);

	// AudioComponent 참조 초기화 (실제 파괴는 UGameSoundManager가 관리)
	CurVoiceAudio = nullptr;
}