// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.
// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "UPopup_DailyStudy.h"

#include "UPopupManager.h"
#include "ULingoGameHelper.h"
#include "UGameDataManager.h"
#include "UConfigLibrary.h"
#include "GameLogging.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "UImageButton.h"
#include "Components/ProgressBar.h"
#include "UTextureButton.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

void UPopup_DailyStudy::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Close)
	{
		Btn_Close->OnButtonClickedEvent.RemoveDynamic(this, &UPopup_DailyStudy::OnClickClose);
		Btn_Close->OnButtonClickedEvent.AddDynamic(this, &UPopup_DailyStudy::OnClickClose);
	}
}

void UPopup_DailyStudy::NativeDestruct()
{
	Super::NativeDestruct();

	// 타이머 정리
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(NextTimerHandle);
	}
}

void UPopup_DailyStudy::InitPopup()
{
	// 초기화
	CurIndex = 0;

	// 일일 최고 득점 로드 및 표시
	InitBestScore();

	// 10개 랜덤 단어 생성
	GenerateQuestions();

	// 첫 번째 질문 표시
	LoadCurQuestion();
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

	Txt_BestScore->SetText( FText::FromString( FString::Printf(TEXT("Best Today: %d"), BestScore)));
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

	// 이미지 로드
	UTexture2D* LoadedTexture = nullptr;
	if (WordData.Texture.IsValid())
		LoadedTexture = WordData.Texture.Get();
	else if (!WordData.Texture.IsNull())
		LoadedTexture = WordData.Texture.LoadSynchronous();

	Img_Display->SetBrushFromTexture(LoadedTexture, true);

	// 진행 상황 업데이트
	Txt_Progress->SetText(FText::FromString(
		FString::Printf(TEXT("%d/%d"),
			CurIndex + 1,
			QuestionList.Num())));

	// Progress Bar 업데이트
	RefreshProgressBar();
}


FDailyStudyResult UPopup_DailyStudy::CalculateResults()
{
	FDailyStudyResult Summary;

	int32 TotalGrammarScore = 0;
	int32 TotalContextScore = 0;
	int32 TotalFinalScore = 0;

	for (const FDailyStudyAnswer& Answer : AnswerList)
	{
		if (Answer.bCompleted)
		{
			TotalGrammarScore += Answer.JudgeResult.grammar_score;
			TotalContextScore += Answer.JudgeResult.context_score;
			TotalFinalScore += Answer.JudgeResult.final_overall_score;
			Summary.CompletedCount++;
		}
		else if (Answer.bSkipped)
		{
			Summary.SkippedCount++;
		}
	}

	Summary.TotalCount = AnswerList.Num();

	if (Summary.CompletedCount > 0)
	{
		Summary.AvgGrammarScore = TotalGrammarScore / Summary.CompletedCount;
		Summary.AvgContextScore = TotalContextScore / Summary.CompletedCount;
		Summary.AvgFinalScore = TotalFinalScore / Summary.CompletedCount;
	}

	return Summary;
}


void UPopup_DailyStudy::RefreshProgressBar()
{
	float Value = 0.0f;
	if (QuestionList.Num() > 0)
		Value = static_cast<float>(CurIndex) / static_cast<float>(QuestionList.Num());

	ProgressBar->SetPercent(Value);
}

void UPopup_DailyStudy::MoveToNextQuestion()
{
	CurIndex++;

	if (CurIndex < QuestionList.Num())
	{
		LoadCurQuestion();
	}
	else
	{
		OnClickClose();

		auto Result = CalculateResults();
		
		this->SaveProgress(Result.AvgFinalScore);
		
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