// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "UPopup_DailyStudy.h"

#include "APlayerActor.h"
#include "UPopupManager.h"
#include "UPopup_DailyResult.h"
#include "ULingoGameHelper.h"
#include "UGameDataManager.h"
#include "GameLogging.h"
#include "UBroadcastManager.h"
#include "UCircularProgressBar.h"
#include "UKLingoNetworkSystem.h"
#include "UDialogManager.h"
#include "UGameSoundManager.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "UTextureButton.h"
#include "Components/CanvasPanel.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

// Media Player
#include "MediaPlayer.h"
#include "MediaTexture.h"
#include "MediaSource.h"

void UPopup_DailyStudy::NativeConstruct()
{
	bAllowPlayerControl = true;
	
	Super::NativeConstruct();

	// 미디어 플레이어 초기화
	if (!MediaPlayer)
	{
		MediaPlayer = NewObject<UMediaPlayer>(this);
		MediaPlayer->SetLooping(false);
		MediaPlayer->OnEndReached.AddDynamic(this, &UPopup_DailyStudy::OnVideoFinished);
	}

	// 미디어 텍스처 생성
	if (!MediaTexture)
	{
		MediaTexture = NewObject<UMediaTexture>(this);
		MediaTexture->SetMediaPlayer(MediaPlayer);
		MediaTexture->UpdateResource();
	}

	// Img_Correct에 미디어 텍스처 바인딩
	FSlateBrush Brush;
	Brush.SetResourceObject(MediaTexture);
	Brush.ImageSize = FVector2D(1920, 1080); // 영상 해상도에 맞게 조정
	Img_Correct->SetBrush(Brush);
}

void UPopup_DailyStudy::NativeDestruct()
{
	Super::NativeDestruct();

	// 타이머 정리
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(NextTimerHandle);
		GetWorld()->GetTimerManager().ClearTimer(ThinkingTimerHandle);
		GetWorld()->GetTimerManager().ClearTimer(VideoCheckTimerHandle);
	}

	// 미디어 플레이어 정리
	MediaPlayer->Close();
}

void UPopup_DailyStudy::InitPopup()
{
	if (Btn_Close)
	{
		Btn_Close->OnButtonClickedEvent.RemoveDynamic(this, &UPopup_DailyStudy::OnClickClose);
		Btn_Close->OnButtonClickedEvent.AddDynamic(this, &UPopup_DailyStudy::OnClickClose);
	}

	if (auto DM = UBroadcastManager::Get(GetWorld()))
	{
		DM->OnAudioCapture.RemoveDynamic(this, &UPopup_DailyStudy::OnAudioCapture);
		DM->OnAudioCapture.AddDynamic(this, &UPopup_DailyStudy::OnAudioCapture);
	}

	if (CountDown_Widget)
	{
		CountDown_Widget->OnCountDownFinished.RemoveDynamic(this, &UPopup_DailyStudy::OnCountDownFinished);
		CountDown_Widget->OnCountDownFinished.AddDynamic(this, &UPopup_DailyStudy::OnCountDownFinished);
	}

	// 초기화
	CurIndex = 0;
	AnswerList.Empty();
	CurrentScore = 0;

	Canvas_Correct->SetVisibility(ESlateVisibility::Hidden);
	Canvas_Question->SetVisibility(ESlateVisibility::Hidden);
	
	Txt_CurScore->SetText(FText::FromString(FString::Printf(TEXT("Score : %d"), CurrentScore)));

	// 카운트다운 시작
	CountDown_Widget->StartCountDown(3);
}

void UPopup_DailyStudy::InitPopup(const TArray<FString>& Words)
{
	// 기본 초기화 (델리게이트 바인딩 등)
	if (Btn_Close)
	{
		Btn_Close->OnButtonClickedEvent.RemoveDynamic(this, &UPopup_DailyStudy::OnClickClose);
		Btn_Close->OnButtonClickedEvent.AddDynamic(this, &UPopup_DailyStudy::OnClickClose);
	}

	if (auto DM = UBroadcastManager::Get(GetWorld()))
	{
		DM->OnAudioCapture.RemoveDynamic(this, &UPopup_DailyStudy::OnAudioCapture);
		DM->OnAudioCapture.AddDynamic(this, &UPopup_DailyStudy::OnAudioCapture);
	}

	if (CountDown_Widget)
	{
		CountDown_Widget->OnCountDownFinished.RemoveDynamic(this, &UPopup_DailyStudy::OnCountDownFinished);
		CountDown_Widget->OnCountDownFinished.AddDynamic(this, &UPopup_DailyStudy::OnCountDownFinished);
	}

	
	// 초기화
	CurIndex = 0;
	AnswerList.Empty();
	CurrentScore = 0;
	QuestionList.Empty();
	QuestionList.Append(Words);

	Canvas_Correct->SetVisibility(ESlateVisibility::Hidden);
	Canvas_Question->SetVisibility(ESlateVisibility::Hidden);
	
	Txt_CurScore->SetText(FText::FromString(FString::Printf(TEXT("Score : %d"), CurrentScore)));

	// 카운트다운 시작
	CountDown_Widget->StartCountDown(3);
}

void UPopup_DailyStudy::LoadCurQuestion()
{
	if (!QuestionList.IsValidIndex(CurIndex))
	{
		PRINTLOG(TEXT("[DailyStudy] Error: Invalid question index %d"), CurIndex);
		return;
	}

	Txt_Question->SetText(FText::FromString(QuestionList[CurIndex]));
	
	// 진행 상황 업데이트
	Txt_QuestionProgress->SetText(FText::FromString(
		FString::Printf(TEXT("%02d/%02d"),
			CurIndex + 1,
			QuestionList.Num())));

	StartThinkTimer();
}

void UPopup_DailyStudy::OnClickClose()
{
	if ( auto PopupMgr = UPopupManager::Get(GetWorld()) )
	{
		PopupMgr->HideCurrentPopup();
	}
}

FString UPopup_DailyStudy::GetCurrentQuestionText() const
{
	if (QuestionList.IsValidIndex(CurIndex))
	{
		return QuestionList[CurIndex];
	}
	return FString();
}

void UPopup_DailyStudy::OnResponseSpeakingsJudges(const FResponseSpeakingJudes& JudgeResult)
{
	if (!QuestionList.IsValidIndex(CurIndex))
	{
		return;
	}
	
	FDailyStudyAnswer Answer;
	Answer.QuestionIndex = CurIndex;
	Answer.ExpectedAnswer = QuestionList[CurIndex];
	Answer.JudgeResult = JudgeResult;
	Answer.bCompleted = true;
	AnswerList.Add(Answer);

	CurrentScore += JudgeResult.final_overall_score;
	Txt_CurScore->SetText(FText::FromString(FString::Printf(TEXT("Score : %d"), CurrentScore)));

	// 정답/오답 판별 (50점 미만이면 오답)
	bLastAnswerCorrect = (JudgeResult.final_overall_score >= 50);
	
	PRINTLOG(TEXT("[DailyStudy] OnResponseSpeakingsJudges - Score: %d, IsCorrect: %s"), 
		JudgeResult.final_overall_score, 
		bLastAnswerCorrect ? TEXT("TRUE") : TEXT("FALSE"));

	// Request TTS for the Korean answer
	if (UKLingoNetworkSystem* NetworkSystem = UKLingoNetworkSystem::Get(GetWorld()))
	{
		NetworkSystem->RequestListenAudio(QuestionList[CurIndex], FResponseListenAudioDelegate::CreateUObject(this, &UPopup_DailyStudy::OnResponseListenAudio));
	}
}

void UPopup_DailyStudy::StartThinkTimer()
{
	RemainingThinkTime = DailyStudyConfig::THINK_TIME;
	
	ProgressBar_RemainTime->SetPercent(1.0f);

	Txt_RemainTime->SetText(FText::FromString(FString::Printf(TEXT("%.1f"), RemainingThinkTime)));

	GetWorld()->GetTimerManager().SetTimer(ThinkingTimerHandle, this, &UPopup_DailyStudy::UpdateThinkTimer, 0.1f, true);
}

void UPopup_DailyStudy::UpdateThinkTimer()
{
	RemainingThinkTime -= 0.1f;
	if (RemainingThinkTime <= 0.f)
	{
		RemainingThinkTime = 0.f;
		OnThinkTimeFinished();
	}

	ProgressBar_RemainTime->SetPercent(RemainingThinkTime / DailyStudyConfig::THINK_TIME);
	Txt_RemainTime->SetText(FText::FromString(FString::Printf(TEXT("%.1f"), RemainingThinkTime)));
}

void UPopup_DailyStudy::OnThinkTimeFinished()
{
	GetWorld()->GetTimerManager().ClearTimer(ThinkingTimerHandle);

	// 타임업 = 오답 처리
	bLastAnswerCorrect = false;

	// Add a skipped answer
	if (QuestionList.IsValidIndex(CurIndex))
	{
		FDailyStudyAnswer Answer;
		Answer.QuestionIndex = CurIndex;
		Answer.ExpectedAnswer = QuestionList[CurIndex];
		Answer.bSkipped = true;
		AnswerList.Add(Answer);
	}
	
	if (UKLingoNetworkSystem* NetworkSystem = UKLingoNetworkSystem::Get(GetWorld()))
		NetworkSystem->RequestListenAudio(QuestionList[CurIndex], FResponseListenAudioDelegate::CreateUObject(this, &UPopup_DailyStudy::OnResponseListenAudio));
}

void UPopup_DailyStudy::OnAudioCapture(bool bIsRecording)
{
	if (bIsRecording)
	{
		GetWorld()->GetTimerManager().ClearTimer(ThinkingTimerHandle);
	}
}

void UPopup_DailyStudy::OnResponseListenAudio(FResponseListenAudio& ResponseData, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		// 저장된 정답/오답 판별 결과 사용
		ShowCorrectData(bLastAnswerCorrect);

		if ( auto DM = UDialogManager::Get(GetWorld()))
			DM->HideToastImmediately();
		
		if (auto PlayerActor = ULingoGameHelper::GetPlayerActor(this))
			PlayerActor->PlayTTSAudio(ResponseData.audio_base64);
	}
	else
	{
		ShowCorrectData(bLastAnswerCorrect);
	}
}

void UPopup_DailyStudy::ShowCorrectData(bool bIsCorrect)
{
	Canvas_Correct->SetVisibility(ESlateVisibility::Visible);
		
	Txt_Correct->SetText(FText::FromString(""));
	Txt_Question->SetText(FText::FromString(""));
	
	PlayVideo(bIsCorrect);
}

void UPopup_DailyStudy::OnCountDownFinished()
{
	Canvas_Question->SetVisibility(ESlateVisibility::Visible);

	UGameSoundManager::Get(GetWorld())->PlaySound2D(EGameSoundType::Speak_the_Word_in_korean);
	
	LoadCurQuestion();
}

void UPopup_DailyStudy::MoveToNextQuestion()
{
	Canvas_Correct->SetVisibility(ESlateVisibility::Hidden);

	CurIndex++;

	if (CurIndex < QuestionList.Num())
	{
		LoadCurQuestion();
	}
	else
	{
		// 모든 문제 완료 - 점수 계산 및 표시
		int32 AverageScore = QuestionList.Num() > 0 ? (CurrentScore / QuestionList.Num()) : 0;

		FString ResultMessage = FString::Printf(
			TEXT("Score: %d\nAverage: %d\nSolved: %d"),
			CurrentScore,
			AverageScore,
			QuestionList.Num()
		);

		UPopupManager::Get(GetWorld())->ShowMsgBox(
			TEXT("Daily Result"),
			ResultMessage,
			EMsgBoxType::OK,
			FOnMsgBoxOkDelegate::CreateUObject(this, &UPopup_DailyStudy::OnClickClose));
	}
}

void UPopup_DailyStudy::PlayVideo(bool bIsCorrect)
{
	if (!MediaPlayer || !Img_Correct)
	{
		// 영상 재생 실패 시 바로 다음 문제로
		OnVideoFinished();
		return;
	}

	// 재생할 영상 소스 선택
	UMediaSource* SourceToPlay = bIsCorrect ? CorrectVideoSource : WrongVideoSource;

	if (!SourceToPlay)
	{
		// 영상이 없으면 바로 다음 문제로 이동
		OnVideoFinished();
		return;
	}

	Img_Correct->SetVisibility(ESlateVisibility::Visible);

	// 영상 열기 및 재생
	MediaPlayer->OpenSource(SourceToPlay);
	MediaPlayer->Play();

	// 시작 위치로 Seek
	MediaPlayer->Seek(FTimespan::FromSeconds(VideoStartTime));

	// 재생 시간 체크 타이머 시작 (0.1초마다 체크)
	GetWorld()->GetTimerManager().SetTimer(
		VideoCheckTimerHandle,
		this, &UPopup_DailyStudy::CheckVideoPlayback,
		0.1f,
		true  // 반복
	);
}

void UPopup_DailyStudy::CheckVideoPlayback()
{
	if (!MediaPlayer)
		return;

	// 현재 재생 시간 확인
	FTimespan CurrentTime = MediaPlayer->GetTime();
	float CurrentSeconds = CurrentTime.GetTotalSeconds();

	// 종료 시간 도달 체크
	if (CurrentSeconds >= VideoEndTime)
	{
		// 타이머 중지
		GetWorld()->GetTimerManager().ClearTimer(VideoCheckTimerHandle);

		// 영상 종료 처리
		OnVideoFinished();
	}
}

void UPopup_DailyStudy::OnVideoFinished()
{
	PRINTLOG(TEXT("[DailyStudy] Video playback finished"));

	// 영상 숨기기
	Img_Correct->SetVisibility(ESlateVisibility::Hidden);

	// 미디어 플레이어 정지
	MediaPlayer->Close();

	// 타이머 정리
	GetWorld()->GetTimerManager().ClearTimer(VideoCheckTimerHandle);

	// 다음 문제로 이동 (기존 타이머 로직 사용)
	GetWorld()->GetTimerManager().SetTimer(
		NextTimerHandle,
		this, &UPopup_DailyStudy::MoveToNextQuestion,
		DailyStudyConfig::NEXT_QUESTION,
		false
	);
}