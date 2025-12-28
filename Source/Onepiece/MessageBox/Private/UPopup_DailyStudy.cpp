// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "UPopup_DailyStudy.h"

#include "APlayerActor.h"
#include "UPopupManager.h"
#include "UPopup_DailyResult.h"
#include "ULingoGameHelper.h"
#include "UGameDataManager.h"
#include "UConfigLibrary.h"
#include "GameLogging.h"
#include "UBroadcastManager.h"
#include "UCircularProgressBar.h"
#include "UKLingoNetworkSystem.h"
#include "UDialogManager.h"
#include "UGameSoundManager.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "UTextureButton.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
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

	// 일일 최고 득점 로드 및 표시
	InitBestScore();

	// 10개 랜덤 단어 생성
	GenerateQuestions();

	// 카운트다운 시작
	CountDown_Widget->StartCountDown(3);
}

void UPopup_DailyStudy::GenerateQuestions()
{
	QuestionList.Empty();

	UGameDataManager* DataManager = UGameDataManager::Get(GetWorld());
	if (!DataManager)
	{
		PRINTLOG(TEXT("[DailyStudy] Error: GameDataManager not found"));
		return;
	}

	// 각 타입별 사용 가능한 key 리스트 가져오기
	TArray<int32> ReadDataKeys = DataManager->GetAllReadDataKeys();    // Animal/Food용
	TArray<int32> ListenDataKeys = DataManager->GetAllListenDataKeys(); // Region용

	// 데이터가 충분한지 확인
	if (ReadDataKeys.Num() == 0 && ListenDataKeys.Num() == 0)
	{
		PRINTLOG(TEXT("[DailyStudy] Error: No data available for questions"));
		return;
	}

	// 중복 방지를 위해 이미 사용한 항목 추적
	TSet<TPair<EWordType, int32>> UsedItems;

	// 필요한 문제 개수만큼 생성
	for (int32 i = 0; i < DailyStudyConfig::QUESTIONS_PER_SESSION; ++i)
	{
		// 사용 가능한 타입 리스트 구성
		TArray<EWordType> AvailableTypes;
		if (ReadDataKeys.Num() > 0) AvailableTypes.Add(EWordType::Animal);
		if (ListenDataKeys.Num() > 0) AvailableTypes.Add(EWordType::Region);

		if (AvailableTypes.Num() == 0)
		{
			PRINTLOG(TEXT("[DailyStudy] Warning: Not enough data to generate %d questions, only %d generated"),
				DailyStudyConfig::QUESTIONS_PER_SESSION, i);
			break;
		}

		// 랜덤 타입 선택
		EWordType RandomType = AvailableTypes[FMath::RandRange(0, AvailableTypes.Num() - 1)];

		// 해당 타입에서 랜덤 key 선택
		int32 RandomKey = -1;
		if (RandomType == EWordType::Animal || RandomType == EWordType::Food)
		{
			if (ReadDataKeys.Num() > 0)
			{
				int32 RandomIndex = FMath::RandRange(0, ReadDataKeys.Num() - 1);
				RandomKey = ReadDataKeys[RandomIndex];
				ReadDataKeys.RemoveAt(RandomIndex); // 중복 방지
			}
		}
		else if (RandomType == EWordType::Region)
		{
			if (ListenDataKeys.Num() > 0)
			{
				int32 RandomIndex = FMath::RandRange(0, ListenDataKeys.Num() - 1);
				RandomKey = ListenDataKeys[RandomIndex];
				ListenDataKeys.RemoveAt(RandomIndex); // 중복 방지
			}
		}

		// 유효한 key를 찾지 못한 경우
		if (RandomKey == -1)
		{
			PRINTLOG(TEXT("[DailyStudy] Warning: Failed to find valid key for type %d"), static_cast<int32>(RandomType));
			continue;
		}

		// 데이터 로드
		FDailyStudyWordItem WordItem;
		LoadWordData(RandomType, RandomKey, WordItem);

		QuestionList.Add(WordItem);
	}

	PRINTLOG(TEXT("[DailyStudy] Generated %d questions"), QuestionList.Num());
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

	if (Type == EWordType::Animal)
	{
		FReadData ReadData;
		if (DataManager->GetReadData(Code, ReadData))
		{
			OutItem.Eng = ReadData.Eng;
			OutItem.Kor = ReadData.Word;
			OutItem.Texture = ReadData.Texture;
		}
	}
	else if (Type == EWordType::Food)
	{
		FReadData ReadData;
		if (DataManager->GetReadData(Code, ReadData))
		{
			OutItem.Eng = ReadData.Eng;
			OutItem.Kor = ReadData.Word;
			OutItem.Texture = ReadData.Texture;
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

	BestScore = 0;
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

	CorrectData = QuestionList[CurIndex];

	Txt_Question->SetText(FText::FromString(CorrectData.Kor));
	
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
		NetworkSystem->RequestListenAudio(CorrectData.Kor, FResponseListenAudioDelegate::CreateUObject(this, &UPopup_DailyStudy::OnResponseListenAudio));
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

	ProgressBar_RemainTime->SetPercent(RemainingThinkTime / DailyStudyConfig::THINK_TIME);
	Txt_RemainTime->SetText(FText::FromString(FString::Printf(TEXT("%.1f"), RemainingThinkTime)));
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
	
	if (UKLingoNetworkSystem* NetworkSystem = UKLingoNetworkSystem::Get(GetWorld()))
		NetworkSystem->RequestListenAudio(CorrectData.Kor, FResponseListenAudioDelegate::CreateUObject(this, &UPopup_DailyStudy::OnResponseListenAudio));
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
		ShowCorrectData();

		if ( auto DM = UDialogManager::Get(GetWorld()))
			DM->HideToastImmediately();
		
		if (auto PlayerActor = ULingoGameHelper::GetPlayerActor(this))
		{
			PlayerActor->PlayTTSAudio(ResponseData.audio_base64);

			GetWorld()->GetTimerManager().SetTimer(NextTimerHandle, this, &UPopup_DailyStudy::MoveToNextQuestion, DailyStudyConfig::NEXT_QUESTION, false);
		}
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(NextTimerHandle, this, &UPopup_DailyStudy::MoveToNextQuestion, DailyStudyConfig::NEXT_QUESTION, false);
	}
}

void UPopup_DailyStudy::ShowCorrectData()
{
	Canvas_Correct->SetVisibility(ESlateVisibility::Visible);
		
	Txt_Correct->SetText(FText::FromString(CorrectData.Eng));

	FSlateBrush Brush;
	Brush.SetResourceObject(CorrectData.LoadedTexture());
	Brush.ImageSize = FVector2D::ZeroVector;
	Img_Correct->SetBrush(Brush);
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
		OnClickClose();

		this->SaveProgress(CurrentScore);

		// 최고 점수 읽기 (SaveProgress 후 업데이트된 값)
		int32 UserId = ULingoGameHelper::GetUserId(GetWorld());
		FString Today = FDateTime::Now().ToString(TEXT("%Y-%m-%d"));
		FString BestScoreDate = UConfigLibrary::GetUserString(UserId, TEXT("DailyStudyBestScoreDate"), TEXT(""));

		BestScore = 0;
		if (BestScoreDate == Today)
		{
			BestScore = UConfigLibrary::GetUserInt(UserId, TEXT("DailyStudyBestScore"), 0);
		}

		// 결과 데이터 구성
		FDailyStudyResult ResultData;
		ResultData.CurrentScore = CurrentScore;
		ResultData.BestScore = BestScore;
		ResultData.QuestionList = QuestionList;
		ResultData.AnswerList = AnswerList;
		ResultData.TotalCount = QuestionList.Num();
		
		// 완료/건너뛰기 카운트 계산
		ResultData.CompletedCount = 0;
		ResultData.SkippedCount = 0;
		for (const FDailyStudyAnswer& Answer : AnswerList)
		{
			if (Answer.bSkipped)
				ResultData.SkippedCount++;
			else if (Answer.bCompleted)
				ResultData.CompletedCount++;
		}

		// 평균 점수 계산
		int32 TotalGrammar = 0;
		int32 TotalContext = 0;
		int32 TotalFinal = 0;
		int32 CompletedAnswers = 0;
		
		for (const FDailyStudyAnswer& Answer : AnswerList)
		{
			if (Answer.bCompleted)
			{
				TotalGrammar += Answer.JudgeResult.grammar_score;
				TotalContext += Answer.JudgeResult.context_score;
				TotalFinal += Answer.JudgeResult.final_overall_score;
				CompletedAnswers++;
			}
		}
		
		if (CompletedAnswers > 0)
		{
			ResultData.AvgGrammarScore = TotalGrammar / CompletedAnswers;
			ResultData.AvgContextScore = TotalContext / CompletedAnswers;
			ResultData.AvgFinalScore = TotalFinal / CompletedAnswers;
		}

		// 결과 팝업 표시
		if (UPopup_DailyResult* ResultPopup = UPopupManager::Get(GetWorld())->ShowPopupAs<UPopup_DailyResult>(EPopupType::DailyResult))
		{
			ResultPopup->InitPopup(ResultData);
		}
	}
}