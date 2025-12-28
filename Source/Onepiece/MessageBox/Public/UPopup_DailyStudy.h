// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "UBasePopup.h"
#include "FDailyStudyResult.h"
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
	static constexpr float THINK_TIME = 3.0f;

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
	/**
	 * @brief AI 생성 단어로 Daily Study 초기화
	 * @param Words AI가 생성한 한국어 단어 배열
	 */
	void InitPopup(const TArray<FString>& Words);

	FString GetCurrentQuestionText() const;
	
	void OnResponseSpeakingsJudges(const FResponseSpeakingJudes& JudgeResult);

private:
	/** 현재 질문 UI 업데이트 */
	void LoadCurQuestion();

	/** 다음 질문으로 이동 */
	void MoveToNextQuestion();

	/** 정답 데이터 표시 (정답/오답 판별) */
	void ShowCorrectData(bool bIsCorrect);
	
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
	TObjectPtr<class UTextureButton> Btn_Close;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCanvasPanel> Canvas_Question;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UBorder> Border_Question;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_Question;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_Infomation;
	
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
	TArray<FString> QuestionList;
	
	/** 답변 리스트 (FResponseSpeakingJudes) */
	TArray<FResponseSpeakingJudes> AnswerList;
	
	/** 현재 질문 인덱스 (0-based) */
	int32 CurIndex = 0;
	
	int32 CurrentScore = 0;
	int32 CorrectAnswerCount = 0;

	/** 다음 문제 이동 타이머 */
	FTimerHandle NextTimerHandle;
	FTimerHandle ThinkingTimerHandle;

	float RemainingThinkTime = 0.f;

	/** 마지막 답변이 정답인지 여부 (타임업 또는 50점 미만 = false) */
	bool bLastAnswerCorrect = false;
	
	// ===================================================================
	// Media Player - 정답/오답 영상 재생
	// ===================================================================

	/** 미디어 플레이어 */
	UPROPERTY()
	TObjectPtr<class UMediaPlayer> MediaPlayer;

	/** 미디어 텍스처 */
	UPROPERTY()
	TObjectPtr<class UMediaTexture> MediaTexture;

	/** 정답 영상 소스 */
	UPROPERTY(EditDefaultsOnly, Category = "Media")
	TObjectPtr<class UMediaSource> CorrectVideoSource;

	/** 오답 영상 소스 */
	UPROPERTY(EditDefaultsOnly, Category = "Media")
	TObjectPtr<class UMediaSource> WrongVideoSource;

	/** 영상 재생 시작 시간 (초) */
	UPROPERTY(EditDefaultsOnly, Category = "Media", meta = (ClampMin = "0.0"))
	float VideoStartTime = 0.0f;

	/** 영상 재생 종료 시간 (초) */
	UPROPERTY(EditDefaultsOnly, Category = "Media", meta = (ClampMin = "0.0"))
	float VideoEndTime = 3.0f;

	/** 영상 재생 시간 체크 타이머 */
	FTimerHandle VideoCheckTimerHandle;

private:
	/** 영상 재생 (정답/오답) */
	void PlayVideo(bool bIsCorrect);

	/** 영상 재생 시간 체크 */
	void CheckVideoPlayback();

	/** 영상 재생 완료 콜백 */
	UFUNCTION()
	void OnVideoFinished();
};