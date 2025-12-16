// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file UVoiceFunctionLibrary.cpp
 * @brief UVoiceFunctionLibrary의 동작을 구현합니다.
 */
#include "UVoiceFunctionLibrary.h"
#include "GameLogging.h"
#include "Sound/SoundWaveProcedural.h"

#define VOICE_LOG TEXT("VoiceLogs")

static uint32 ReadUInt32(const uint8* Data, int32 Offset)
{
	return Data[Offset] |
		   (Data[Offset + 1] << 8) |
		   (Data[Offset + 2] << 16) |
		   (Data[Offset + 3] << 24);
}

static uint16 ReadUInt16(const uint8* Data, int32 Offset)
{
	return Data[Offset] | (Data[Offset + 1] << 8);
}

TArray<uint8> UVoiceFunctionLibrary::ConvertPCM2WAV(
	const TArray<uint8>& InPCMData,
	const int32 InSampleRate,
	const int32 InChannel,
	const int32 InBitsPerSample)
{
	TArray<uint8> WavData;

	const int32 ByteRate   = InSampleRate * InChannel * InBitsPerSample / 8;
	const int32 BlockAlign = InChannel * InBitsPerSample / 8;
	const int32 DataSize   = InPCMData.Num();
	const int32 ChunkSize  = 36 + DataSize;

	// RIFF 헤더
	WavData.Append(reinterpret_cast<const uint8*>("RIFF"), 4);
	WavData.Append(reinterpret_cast<const uint8*>(&ChunkSize), 4);
	WavData.Append(reinterpret_cast<const uint8*>("WAVE"), 4);

	// fmt chunk
	WavData.Append(reinterpret_cast<const uint8*>("fmt "), 4);
	int32 SubChunk1Size = 16;
	int16 AudioFormat   = 1;

	WavData.Append(reinterpret_cast<const uint8*>(&SubChunk1Size), 4);
	WavData.Append(reinterpret_cast<const uint8*>(&AudioFormat), 2);
	WavData.Append(reinterpret_cast<const uint8*>(&InChannel), 2);
	WavData.Append(reinterpret_cast<const uint8*>(&InSampleRate), 4);
	WavData.Append(reinterpret_cast<const uint8*>(&ByteRate), 4);
	WavData.Append(reinterpret_cast<const uint8*>(&BlockAlign), 2);
	WavData.Append(reinterpret_cast<const uint8*>(&InBitsPerSample), 2);
	WavData.Append(reinterpret_cast<const uint8*>("data"), 4);
	WavData.Append(reinterpret_cast<const uint8*>(&DataSize), 4);

	WavData.Append(InPCMData);
	return WavData;
}


FString UVoiceFunctionLibrary::SaveWavToFile(TArray<uint8>& InWavData, const FString& InFileName )
{
	if (InWavData.Num() == 0)
	{
		PRINTLOG( TEXT("WavData is empty, nothing to save."));
		return FString();
	}

	FString FileName = InFileName;
	if (FileName.IsEmpty())
	{
		// 날짜 기반 파일명 생성
		const FDateTime Now = FDateTime::Now();
		FileName = FString::Printf(TEXT("Voice_%04d%02d%02d_%02d%02d%02d.wav"),
			Now.GetYear(), Now.GetMonth(), Now.GetDay(),
			Now.GetHour(), Now.GetMinute(), Now.GetSecond()
		);
	}

	FString FolderPath = FPaths::ProjectSavedDir() / VOICE_LOG;
	// 폴더 없으면 생성
	IFileManager::Get().MakeDirectory(*FolderPath, true);

	FString FullPath = FolderPath / FileName;
	if (FFileHelper::SaveArrayToFile(InWavData, *FullPath))
	{
		// 절대 경로로 변환하여 반환
		FString AbsolutePath = FPaths::ConvertRelativePathToFull(FullPath);
		PRINTLOG( TEXT("Saved WAV file: %s"), *AbsolutePath);
		return AbsolutePath;
	}
	else
	{
		PRINTLOG( TEXT("Failed to save WAV file: %s"), *FullPath);
		return FString();
	}
}

USoundWave* UVoiceFunctionLibrary::CreateSoundWaveFromWavData(const TArray<uint8>& WavData)
{
    if (WavData.Num() < 44)
    {
        PRINTLOG( TEXT("Invalid WAV data (too small)"));
        return nullptr;
    }

    const uint8* RawData = WavData.GetData();

    // WAV Header Parsing
    // ChunkID "RIFF" (0~3)
    // Format "WAVE" (8~11)
    // Subchunk1ID "fmt " (12~15)
    // AudioFormat, NumChannels, SampleRate, ByteRate, BlockAlign, BitsPerSample
    uint16 AudioFormat  = ReadUInt16(RawData, 20);
    uint16 NumChannels  = ReadUInt16(RawData, 22);
    uint32 SampleRate   = ReadUInt32(RawData, 24);
    uint16 BitsPerSample = ReadUInt16(RawData, 34);

    // Subchunk2ID "data"는 고정 위치가 아니므로 탐색
    int32 DataChunkOffset = 36;
    while (DataChunkOffset + 8 < WavData.Num())
    {
        uint32 ChunkID =
            RawData[DataChunkOffset] |
            (RawData[DataChunkOffset + 1] << 8) |
            (RawData[DataChunkOffset + 2] << 16) |
            (RawData[DataChunkOffset + 3] << 24);

        uint32 ChunkSize = ReadUInt32(RawData, DataChunkOffset + 4);

        // 'data' 체크
        if (ChunkID == 'atad') // 'data'를 리틀엔디언 'atad'로 읽음
        {
            break;
        }

        // 다음 Chunk로 이동
        DataChunkOffset += (8 + ChunkSize);
    }

    if (DataChunkOffset + 8 >= WavData.Num())
    {
        PRINTLOG( TEXT("WAV data chunk not found"));
        return nullptr;
    }

    const int32 DataStart = DataChunkOffset + 8;
    const int32 DataSize = WavData.Num() - DataStart;

    // SoundWave 생성
    USoundWave* SoundWave = NewObject<USoundWave>();
    if (!SoundWave)
    {
        PRINTLOG( TEXT("Failed to create USoundWave"));
        return nullptr;
    }

    SoundWave->SoundGroup = ESoundGroup::SOUNDGROUP_Voice;
    SoundWave->DecompressionType = EDecompressionType::DTYPE_Procedural;
    SoundWave->bLooping = false;

    SoundWave->NumChannels   = NumChannels;
    SoundWave->Duration      = (float)DataSize / (SampleRate * NumChannels * (BitsPerSample / 8));
    SoundWave->SetSampleRate(SampleRate);
    SoundWave->RawPCMDataSize = DataSize;

    // RawPCMData에 복사
    uint8* PCMData = (uint8*)FMemory::Malloc(DataSize);
    FMemory::Memcpy(PCMData, RawData + DataStart, DataSize);
    SoundWave->RawPCMData = PCMData;

    return SoundWave;
}

USoundWaveProcedural* UVoiceFunctionLibrary::CreateProceduralSoundWaveFromWavData(const TArray<uint8>& AudioData)
{
	// VoiceLogs 폴더에 타임스탬프 형식으로 저장
	const FDateTime Now = FDateTime::Now();
	const FString FileName = FString::Printf(TEXT("TTS_Output_%04d%02d%02d_%02d%02d%02d.wav"),
		Now.GetYear(), Now.GetMonth(), Now.GetDay(),
		Now.GetHour(), Now.GetMinute(), Now.GetSecond()
	);
	const FString FolderPath = FPaths::ProjectSavedDir() / VOICE_LOG;
	IFileManager::Get().MakeDirectory(*FolderPath, true);
	const FString SavePath = FolderPath / FileName;

	if (!FFileHelper::SaveArrayToFile(AudioData, *SavePath))
	{
		PRINTLOG(TEXT("TTS WAV 저장 실패"));
		return nullptr;
	}

	PRINTLOG(TEXT("TTS WAV 저장 완료: %s"), *SavePath);

	if (AudioData.Num() < 44)
	{
		PRINTLOG(TEXT("Invalid WAV data (too small)"));
		return nullptr;
	}

	const uint8* RawData = AudioData.GetData();

	// --- WAV Header Parsing ---
	uint16 NumChannels = ReadUInt16(RawData, 22);
	uint32 SampleRate = ReadUInt32(RawData, 24);
	uint16 BitsPerSample = ReadUInt16(RawData, 34);

	// Find the 'data' chunk, as it's not always at a fixed position.
	int32 DataChunkOffset = 36; // Typically after the 'fmt ' chunk.
	while (DataChunkOffset + 8 < AudioData.Num())
	{
		// Read ChunkID as a 4-character string
		const char* ChunkIDStr = (const char*)(RawData + DataChunkOffset);

		if (strncmp(ChunkIDStr, "data", 4) == 0)
		{
			break; // Found it
		}

		// If not 'data', skip to the next chunk
		uint32 ChunkSize = ReadUInt32(RawData, DataChunkOffset + 4);
		DataChunkOffset += (8 + ChunkSize);
	}

	if (DataChunkOffset + 8 >= AudioData.Num())
	{
		PRINTLOG(TEXT("WAV 'data' chunk not found"));
		return nullptr;
	}

	const uint32 DataSize = ReadUInt32(RawData, DataChunkOffset + 4);
	const int32 DataStart = DataChunkOffset + 8;

	if (DataStart + (int32)DataSize > AudioData.Num())
	{
		PRINTLOG(TEXT("Invalid WAV data chunk size"));
		return nullptr;
	}
	// --- End of WAV Header Parsing ---

	USoundWaveProcedural* SoundWave = NewObject<USoundWaveProcedural>();
	if (!SoundWave)
	{
		PRINTLOG(TEXT("Failed to create USoundWaveProcedural"));
		return nullptr;
	}

	// Calculate actual duration for one-shot playback
	const float BytesPerSample = BitsPerSample / 8.0f;
	const float ActualDuration = (float)DataSize / (SampleRate * NumChannels * BytesPerSample);

	SoundWave->SetSampleRate(SampleRate);
	SoundWave->NumChannels = NumChannels;
	SoundWave->Duration = ActualDuration;
	SoundWave->SoundGroup = ESoundGroup::SOUNDGROUP_Voice;
	SoundWave->bLooping = false;

	// Queue the raw PCM data for playback
	SoundWave->QueueAudio(RawData + DataStart, DataSize);
	return SoundWave;
}

TArray<uint8> UVoiceFunctionLibrary::ResampleAudio(const TArray<uint8>& InPCMData, int32 InSampleRate, int32 OutSampleRate, int32 InNumChannels)
{
	if (InSampleRate == OutSampleRate)
		return InPCMData;

	// PCM 데이터는 int16 형식
	const int32 NumSamples = InPCMData.Num() / sizeof(int16);
	const int16* InSamples = reinterpret_cast<const int16*>(InPCMData.GetData());

	// 리샘플링 비율 계산
	const double ResampleRatio = static_cast<double>(OutSampleRate) / static_cast<double>(InSampleRate);
	const int32 OutNumSamples = FMath::CeilToInt(NumSamples * ResampleRatio);

	TArray<uint8> OutPCMData;
	OutPCMData.Reserve(OutNumSamples * sizeof(int16));

	// Linear interpolation 리샘플링
	for (int32 i = 0; i < OutNumSamples; ++i)
	{
		const double SourceIndex = i / ResampleRatio;
		const int32 Index0 = FMath::FloorToInt(SourceIndex);
		const int32 Index1 = FMath::Min(Index0 + 1, NumSamples - 1);
		const double Fraction = SourceIndex - Index0;

		// Linear interpolation
		const int16 Sample0 = InSamples[Index0];
		const int16 Sample1 = InSamples[Index1];
		const int16 InterpolatedSample = FMath::RoundToInt(Sample0 + (Sample1 - Sample0) * Fraction);

		// 결과 버퍼에 추가
		const uint8* SampleBytes = reinterpret_cast<const uint8*>(&InterpolatedSample);
		OutPCMData.Append(SampleBytes, sizeof(int16));
	}

	return OutPCMData;
}

TArray<uint8> UVoiceFunctionLibrary::ConvertStereoToMono(const TArray<uint8>& InStereoPCMData)
{
	// PCM 데이터는 int16 형식 (16bit), 스테레오는 [L0, R0, L1, R1, ...] 형태
	const int32 NumSamples = InStereoPCMData.Num() / sizeof(int16);

	if (NumSamples % 2 != 0)
	{
		PRINTLOG(TEXT("[ConvertStereoToMono] Invalid stereo PCM data size (not even number of samples)"));
		return InStereoPCMData;
	}

	const int16* InSamples = reinterpret_cast<const int16*>(InStereoPCMData.GetData());
	const int32 NumMonoSamples = NumSamples / 2;

	TArray<uint8> MonoPCMData;
	MonoPCMData.Reserve(NumMonoSamples * sizeof(int16));

	// 좌우 채널 평균으로 모노 생성
	for (int32 i = 0; i < NumMonoSamples; ++i)
	{
		const int16 LeftSample = InSamples[i * 2];      // 왼쪽 채널
		const int16 RightSample = InSamples[i * 2 + 1]; // 오른쪽 채널

		// 평균값 계산 (오버플로우 방지)
		const int32 AverageSample = (static_cast<int32>(LeftSample) + static_cast<int32>(RightSample)) / 2;
		const int16 MonoSample = static_cast<int16>(FMath::Clamp(AverageSample, -32768, 32767));

		// 결과 버퍼에 추가
		const uint8* SampleBytes = reinterpret_cast<const uint8*>(&MonoSample);
		MonoPCMData.Append(SampleBytes, sizeof(int16));
	}

	PRINTLOG(TEXT("[ConvertStereoToMono] Converted %d stereo samples to %d mono samples"), NumSamples, NumMonoSamples);
	return MonoPCMData;
}
