// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "UPopup_DailyStudy.h"

#include "APlayerActor.h"
#include "UPopupManager.h"
#include "ULingoGameHelper.h"
#include "UGameDataManager.h"
#include "UConfigLibrary.h"
#include "GameLogging.h"
#include "UBroadcastManager.h"
#include "UCircularProgressBar.h"
#include "UKLingoNetworkSystem.h"
#include "UDialogManager.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "UTextureButton.h"
#include "Components/Border.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

void UPopup_DailyStudy::NativeConstruct()
{
	bAllowPlayerControl = true;
	
	Super::NativeConstruct();
}

void UPopup_DailyStudy::NativeDestruct()
{
	Super::NativeDestruct();

	// 타이머 정리
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(NextTimerHandle);
		GetWorld()->GetTimerManager().ClearTimer(ThinkingTimerHandle);
		GetWorld()->GetTimerManager().ClearTimer(CountdownTimerHandle);
	}
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
	
	// 초기화
	CurIndex = 0;
	AnswerList.Empty();
	CurrentScore = 0;

	Txt_CurScore->SetText(FText::FromString(FString::Printf(TEXT("Score : %d"), CurrentScore)));

	// 일일 최고 득점 로드 및 표시
	InitBestScore();

	// 10개 랜덤 단어 생성
	GenerateQuestions();

	StartCountDown(3);
}

void UPopup_DailyStudy::GenerateQuestions()
{
	QuestionList.Empty();

	for (int32 i = 0; i < DailyStudyConfig::QUESTIONS_PER_SESSION; ++i)
	{
		EWordType RandomType = static_cast<EWordType>( FMath::RandRange(0, static_cast<int32>(EWordType::Food)));
		int32 MaxCode = DailyStudyConfig::MAX_CATEGORY;
		int32 RandomCode = FMath::RandRange(0, MaxCode - 1);

		// 데이터 로드
		FDailyStudyWordItem WordItem;
		LoadWordData(RandomType, RandomCode, WordItem);

		QuestionList.Add(WordItem);
	}
}

void UPopup_DailyStudy::LoadWordData(EWordType Type, int32 Code, FDailyStudyWordItem& OutItem)
{
	OutItem.WordType = Type;
	OutItem.WordCode = Code;

	UGameDataManager* DataManager = UGameDataManager::Get(GetWorld());
	if (!DataManager)
	{
		PRINTLOG(TEXT("[DailyStudy] Error: GameDataManager not found"));
		return;
	}

	if (Type == EWordType::Animal || Type == EWordType::Food)
	{
		// Read 계열 데이터 사용
		FReadData ReadData;
		if (DataManager->GetReadData(Code, ReadData))
		{
			OutItem.Eng = ReadData.Eng;
			OutItem.Kor = ReadData.Word;
			OutItem.Texture = ReadData.Texture;
		}
	}
	else if (Type == EWordType::Color)
	{
		// Color 데이터 사용
		FColorData ColorData;
		if (DataManager->GetColorData(Code, ColorData))
		{
			OutItem.Eng = ColorData.Eng;
			OutItem.Kor = ColorData.Desc;
			// Color는 이미지 대신 색상 표시
		}
	}
	else if (Type == EWordType::Region)
	{
		// Listen 데이터 사용
		FListenData ListenData;
		if (DataManager->GetListenData(Code, ListenData))
		{
			OutItem.Eng = ListenData.Eng;
			OutItem.Kor = ListenData.Word;
			OutItem.Texture = ListenData.Texture;
		}
	}
}

void UPopup_DailyStudy::InitBestScore()
{
	int32 UserId = ULingoGameHelper::GetUserId(GetWorld());
	FString Today = FDateTime::Now().ToString(TEXT("%Y-%m-%d"));
	FString BestScoreDate = UConfigLibrary::GetUserString(UserId, TEXT("DailyStudyBestScoreDate"), TEXT(""));

	int32 BestScore = 0;
	if (BestScoreDate == Today)
	{
		// 오늘의 최고 점수 로드
		BestScore = UConfigLibrary::GetUserInt(UserId, TEXT("DailyStudyBestScore"), 0);
	}

	Txt_BestScore->SetText( FText::FromString( FString::Printf(TEXT("Best : %d"), BestScore)));
}

void UPopup_DailyStudy::SaveProgress(int32 FinalScore)
{
	int32 UserId = ULingoGameHelper::GetUserId(GetWorld());

	// 플레이 횟수 증가
	int32 CurrentCount = UConfigLibrary::GetUserInt(UserId, TEXT("DailyStudyCount"), 0);
	UConfigLibrary::SetUserInt(UserId, TEXT("DailyStudyCount"), CurrentCount + 1);

	// 마지막 점수 저장
	UConfigLibrary::SetUserInt(UserId, TEXT("DailyStudyLastScore"), FinalScore);

	// 일일 최고 득점 업데이트
	FString Today = FDateTime::Now().ToString(TEXT("%Y-%m-%d"));
	FString LastBestDate = UConfigLibrary::GetUserString(UserId, TEXT("DailyStudyBestScoreDate"), TEXT(""));

	if (LastBestDate != Today)
	{
		// 새로운 날, 오늘의 최고 점수로 설정
		UConfigLibrary::SetUserInt(UserId, TEXT("DailyStudyBestScore"), FinalScore);
		UConfigLibrary::SetUserString(UserId, TEXT("DailyStudyBestScoreDate"), Today);
		PRINTLOG(TEXT("[DailyResult] New best score for today: %d"), FinalScore);
	}
	else
	{
		// 같은 날, 기존 최고 점수와 비교
		int32 CurrentBest = UConfigLibrary::GetUserInt(UserId, TEXT("DailyStudyBestScore"), 0);
		if (FinalScore > CurrentBest)
		{
			UConfigLibrary::SetUserInt(UserId, TEXT("DailyStudyBestScore"), FinalScore);
			PRINTLOG(TEXT("[DailyResult] Updated best score: %d -> %d"), CurrentBest, FinalScore);
		}
	}

	// 마지막 플레이 날짜 저장
	UConfigLibrary::SetUserString(UserId, TEXT("DailyStudyLastDate"), Today);

	PRINTLOG(TEXT("[DailyResult] Progress saved for User %d, Score: %d"), UserId, FinalScore);
}


void UPopup_DailyStudy::LoadCurQuestion()
{
	if (!QuestionList.IsValidIndex(CurIndex))
	{
		PRINTLOG(TEXT("[DailyStudy] Error: Invalid question index %d"), CurIndex);
		return;
	}

	const FDailyStudyWordItem& WordData = QuestionList[CurIndex];

	CurrentKorAnswer = WordData.Kor;
	
	// 이미지 로드
	UTexture2D* LoadedTexture = nullptr;
	if (WordData.Texture.IsValid())
		LoadedTexture = WordData.Texture.Get();
	else if (!WordData.Texture.IsNull())
		LoadedTexture = WordData.Texture.LoadSynchronous();

	FSlateBrush Brush;
	Brush.SetResourceObject(LoadedTexture);
	Brush.ImageSize = FVector2D::ZeroVector; // 핵심
	Img_Display->SetBrush(Brush);

	// 진행 상황 업데이트
	Txt_QuestionProgress->SetText(FText::FromString(
		FString::Printf(TEXT("%02d/%02d"),
			CurIndex + 1,
			QuestionList.Num())));

	StartThinkTimer();
}


// FDailyStudyResult UPopup_DailyStudy::CalculateResults()
// {
// 	FDailyStudyResult Summary;
//
// 	int32 TotalGrammarScore = 0;
// 	int32 TotalContextScore = 0;
// 	int32 TotalFinalScore = 0;
//
// 	for (const FDailyStudyAnswer& Answer : AnswerList)
// 	{
// 		if (Answer.bCompleted)
// 		{
// 			TotalGrammarScore += Answer.JudgeResult.grammar_score;
// 			TotalContextScore += Answer.JudgeResult.context_score;
// 			TotalFinalScore += Answer.JudgeResult.final_overall_score;
// 			Summary.CompletedCount++;
// 		}
// 		else if (Answer.bSkipped)
// 		{
// 			Summary.SkippedCount++;
// 		}
// 	}
//
// 	Summary.TotalCount = AnswerList.Num();
//
// 	if (Summary.CompletedCount > 0)
// 	{
// 		Summary.AvgGrammarScore = TotalGrammarScore / Summary.CompletedCount;
// 		Summary.AvgContextScore = TotalContextScore / Summary.CompletedCount;
// 		Summary.AvgFinalScore = TotalFinalScore / Summary.CompletedCount;
// 	}
//
// 	return Summary;
// }

void UPopup_DailyStudy::MoveToNextQuestion()
{
	Border_CorrectAnswer->SetVisibility(ESlateVisibility::Hidden);

	CurIndex++;

	if (CurIndex < QuestionList.Num())
	{
		LoadCurQuestion();
	}
	else
	{
		OnClickClose();

		// auto Result = CalculateResults();
		
		this->SaveProgress(CurrentScore);
		
		// ShowResultPopup();
	}
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
		return QuestionList[CurIndex].Eng;
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
	Answer.ExpectedAnswer = QuestionList[CurIndex].Eng;
	Answer.JudgeResult = JudgeResult;
	Answer.bCompleted = true;
	AnswerList.Add(Answer);

	CurrentScore += JudgeResult.final_overall_score;
	Txt_CurScore->SetText(FText::FromString(FString::Printf(TEXT("Score : %d"), CurrentScore)));

	// Request TTS for the Korean answer
	if (UKLingoNetworkSystem* NetworkSystem = UKLingoNetworkSystem::Get(GetWorld()))
	{
		NetworkSystem->RequestListenAudio(CurrentKorAnswer, FResponseListenAudioDelegate::CreateUObject(this, &UPopup_DailyStudy::OnResponseListenAudio));
	}
}

void UPopup_DailyStudy::StartThinkTimer()
{
	RemainingThinkTime = DailyStudyConfig::THINK_TIME;
	
	if (ProgressBar_RemainTime)
		ProgressBar_RemainTime->SetPercent(1.0f);

	if (Txt_RemainTime)
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

	if (ProgressBar_RemainTime)
	{
		ProgressBar_RemainTime->SetPercent(RemainingThinkTime / DailyStudyConfig::THINK_TIME);
	}
	
	if (Txt_RemainTime)
	{
		Txt_RemainTime->SetText(FText::FromString(FString::Printf(TEXT("%.1f"), RemainingThinkTime)));
	}
}

void UPopup_DailyStudy::OnThinkTimeFinished()
{
	GetWorld()->GetTimerManager().ClearTimer(ThinkingTimerHandle);

	// Add a skipped answer
	if (QuestionList.IsValidIndex(CurIndex))
	{
		FDailyStudyAnswer Answer;
		Answer.QuestionIndex = CurIndex;
		Answer.ExpectedAnswer = QuestionList[CurIndex].Eng;
		Answer.bSkipped = true;
		AnswerList.Add(Answer);
	}
	
	MoveToNextQuestion();
}

void UPopup_DailyStudy::OnAudioCapture(bool bIsRecording)
{
	if (bIsRecording)
	{
		// Stop the thinking timer
		GetWorld()->GetTimerManager().ClearTimer(ThinkingTimerHandle);
		
		if (ProgressBar_RemainTime)
			ProgressBar_RemainTime->SetPercent(0.0f);
		
		if (Txt_RemainTime)
			Txt_RemainTime->SetText(FText::FromString(TEXT("")));
	}
}

void UPopup_DailyStudy::OnResponseListenAudio(FResponseListenAudio& ResponseData, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if (UDialogManager* DM = UDialogManager::Get(GetWorld()))
		{
			Border_CorrectAnswer->SetVisibility(ESlateVisibility::Visible);
			Txt_CorrectAnswer->SetText(FText::FromString(CurrentKorAnswer));
		}

		if (auto PlayerActor = ULingoGameHelper::GetPlayerActor(this))
		{
			PlayerActor->PlayTTSAudio(ResponseData.audio_base64);

			GetWorld()->GetTimerManager().SetTimer(NextTimerHandle, this, &UPopup_DailyStudy::MoveToNextQuestion, 1.0f, false);
		}
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(NextTimerHandle, this, &UPopup_DailyStudy::MoveToNextQuestion, 1.0f, false);
	}
}

void UPopup_DailyStudy::StartCountDown(const int InCountDownValue)
{
	this->CountDownValue = InCountDownValue;

	Border_CorrectAnswer->SetVisibility(ESlateVisibility::Visible);
	Txt_CorrectAnswer->SetText(FText::AsNumber(CountDownValue));

	GetWorld()->GetTimerManager().SetTimer(CountdownTimerHandle, this, &UPopup_DailyStudy::UpdateCountDown, 1.0f, true);
}

void UPopup_DailyStudy::UpdateCountDown()
{
	CountDownValue--;

	if (CountDownValue > 0)
	{
		Txt_CorrectAnswer->SetText(FText::AsNumber(CountDownValue));
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(CountdownTimerHandle);

		Border_CorrectAnswer->SetVisibility(ESlateVisibility::Hidden);

		LoadCurQuestion();
	}
}