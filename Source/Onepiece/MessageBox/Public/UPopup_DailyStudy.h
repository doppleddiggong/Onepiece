// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "UBasePopup.h"
#include "FDailyStudy.h"
#include "EWordType.h"
#include "NetworkData.h"
#include "UCountDown.h"
#include "UPopup_DailyStudy.generated.h"

/**
 * @brief Daily Study 설정 상수
 *
 * 나중에 조정 가능하도록 const/static으로 분리된 설정값들입니다.
 */
namespace DailyStudyConfig
{
	/** 한 세션당 문제 개수 (조정 가능) */
	static constexpr int32 QUESTIONS_PER_SESSION = 5;

	/** 생각할 시간 (초) */
	static constexpr float THINK_TIME = 5.0f;
	
	static constexpr int32 MAX_CATEGORY = 24;

	static constexpr float NEXT_QUESTION = 1.5f;
}

// ========================================
// UPopup_DailyStudy
// ========================================

/**
 * @brief Daily Study 팝업
 *
 * 데일리 단어 반복 학습 팝업입니다.
 * Listen → Speak → Judge 루프를 순차적으로 진행합니다.
 *
 * @details
 * - 한 세션당 10개의 랜덤 단어 학습
 * - 순차적 진행 (FSM 불필요)
 * - 10개 완료 후 종합 점수 Summary 표시
 * - PlayerActor의 VoiceConversationSystem과 연동하여 녹음 처리
 */
UCLASS()
class ONEPIECE_API UPopup_DailyStudy : public UBasePopup
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
public:
	UFUNCTION(BlueprintCallable, Category = "Popup")
	void InitPopup();

	FString GetCurrentQuestionText() const;
	
	void OnResponseSpeakingsJudges(const FResponseSpeakingJudes& JudgeResult);

private:
	/** 10개 랜덤 단어 생성 */
	void GenerateQuestions();

	/** WordType에 맞는 데이터 로드 */
	void LoadWordData(EWordType Type, int32 Code, FDailyStudyWordItem& OutItem);

	/** 일일 최고 득점 로드 및 표시 */
	void InitBestScore();
	
	void SaveProgress(int32 FinalScore);

	/** 현재 질문 UI 업데이트 */
	void LoadCurQuestion();

	// FDailyStudyResult CalculateResults();

	/** 다음 질문으로 이동 */
	void MoveToNextQuestion();

	void ShowCorrectData();
	
	/** 닫기 버튼 클릭 */
	UFUNCTION()
	void OnClickClose();

	UFUNCTION()
	void OnAudioCapture(bool bIsRecording);

	UFUNCTION()
	void OnResponseListenAudio(FResponseListenAudio& Response, bool bWasSuccessful);

	void StartThinkTimer();
	void UpdateThinkTimer();
	void OnThinkTimeFinished();

	UFUNCTION()
	void OnCountDownFinished();
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_CurScore;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_BestScore;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextureButton> Btn_Close;


	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCanvasPanel> Canvas_Question;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_Question;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_QuestionProgress;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_RemainTime;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCircularProgressBar> ProgressBar_RemainTime;



	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCanvasPanel> Canvas_Correct;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Img_Correct;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_Correct;

	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCountDown> CountDown_Widget;
	
private:
	// ===================================================================
	// Member Variables
	// ===================================================================

	/** 10개 랜덤 단어 목록 */
	TArray<FDailyStudyWordItem> QuestionList;
	TArray<FDailyStudyAnswer> AnswerList;
	
	/** 현재 질문 인덱스 (0-based) */
	int32 CurIndex = 0;
	
	int32 CurrentScore = 0;
	int32 BestScore = 0;

	/** 다음 문제 이동 타이머 */
	FTimerHandle NextTimerHandle;
	FTimerHandle ThinkingTimerHandle;

	float RemainingThinkTime = 0.f;

	FDailyStudyWordItem CorrectData;
};