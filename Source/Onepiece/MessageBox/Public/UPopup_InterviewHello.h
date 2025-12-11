// Copyright (c) 2025 Doppleddiggong. All rights reserved.
// Unauthorized copying, modification, or distribution of this file,
// via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "UBasePopup.h"
#include "NetworkData.h"
#include "UPopup_InterviewHello.generated.h"

/**
 * Interview Popup - Duolingo Style (Single Question Mode)
 *
 * 기존 ScrollBox 방식과 달리 한 번에 하나의 질문만 표시하고
 * Prev/Next 버튼으로 네비게이션하는 방식입니다.
 */
UCLASS()
class ONEPIECE_API UPopup_InterviewHello : public UBasePopup
{
	GENERATED_BODY()

public:
	/**
	 * @brief 팝업 초기화
	 * @param InterviewData API 응답으로 받은 질문 배열
	 */
	UFUNCTION(BlueprintCallable, Category = "Popup")
	void InitPopup(const FResponseInterviewHello& InterviewData);

	/**
	 * @brief 오늘 Interview 팝업을 건너뛸지 확인
	 * @param WorldContextObject World 컨텍스트 객체
	 * @return true: 오늘은 건너뜀, false: 보여줌
	 */
	static bool ShouldSkipInterviewToday(const UObject* WorldContextObject);

protected:
	virtual void NativeConstruct() override;

private:
	// ===================================================================
	// UI Update Methods
	// ===================================================================

	/** 현재 질문과 답변을 UI에 반영 */
	void RefreshUI();

	/** Prev/Next 화살표 버튼 표시/숨김 처리 */
	void UpdateNavigationButtons();

	/** Submit 버튼 활성화 상태 업데이트 */
	void UpdateSubmitButtonState();

	/** Progress Bar 업데이트 */
	void UpdateProgressBar();

	// ===================================================================
	// Answer Management
	// ===================================================================

	/** 현재 질문의 답변을 TempAnswers에 저장 */
	void SaveCurrentAnswer();

	/** TempAnswers에서 현재 질문의 답변을 불러와 Edit_Answer에 표시 */
	void LoadCurrentAnswer();

	// ===================================================================
	// Button Events
	// ===================================================================

	/** 이전 질문으로 이동 (좌측 화살표) */
	UFUNCTION()
	void OnClickPrevArrow();

	/** 다음 질문으로 이동 (우측 화살표) */
	UFUNCTION()
	void OnClickNextArrow();

	/** Next 버튼 클릭 (중앙 버튼) */
	UFUNCTION()
	void OnClickNext();

	/** Submit 버튼 클릭 */
	UFUNCTION()
	void OnClickSubmit();

	/** 닫기 버튼 클릭 */
	UFUNCTION()
	void OnClickClose();

	/** "Today do not show" 체크박스 변경 */
	UFUNCTION()
	void OnCheckToday(bool bIsChecked);

	/** 답변 입력란 텍스트 변경 시 호출 (실시간 Submit 버튼 상태 업데이트) */
	UFUNCTION()
	void OnAnswerTextChanged(const FText& Text);

	// ===================================================================
	// API Response Callback
	// ===================================================================

	/** Interview Answer API 응답 처리 */
	UFUNCTION()
	void OnResponseInterviewAnswer(FResponseInterviewAnswer& ResponseData, bool bWasSuccessful);

protected:
	// ===================================================================
	// UI Widgets (BindWidget)
	// ===================================================================
	// 참고: 위젯 이름은 UMG 블루프린트의 실제 이름과 정확히 일치해야 합니다.

	/** 팝업 타이틀 텍스트 ("Interview") */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_Title;

	/** 현재 질문 텍스트 (영어) */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> TXt_Question;

	/** 플레이어 답변 입력란 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UEditableText> Edit_Answer;

	/** 질문 진행률 표시 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UProgressBar> ProgressBar_Question;

	/** 이전 질문 버튼 (◀) */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextureButton> Button_PrevArrow;

	/** 다음 질문 버튼 (▶) */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextureButton> Button_NextArrow;

	/** Next 버튼 (중앙, 초록색) - Submit과 전환 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImageButton> Btn_Next;

	/** Submit 버튼 (중앙) - Next와 전환 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImageButton> Btn_Submit;

	/** "Today do not show" 체크박스 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCheckBox> Button_CheckToday;

	/** 팝업 닫기 버튼 (우측 상단 ❌) */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextureButton> Btn_Close;

private:
	// ===================================================================
	// Member Variables
	// ===================================================================

	/** 저장된 인터뷰 질문 데이터 */
	TArray<FInterviewQuestionData> SavedQuestions;

	/** 사용자가 입력한 답변 임시 저장 배열 (Questions.Num()과 크기 동일) */
	TArray<FString> TempAnswers;

	/** 현재 표시 중인 질문 인덱스 (0-based) */
	int32 CurrentQuestionIndex = 0;

	/** "Today do not show" 체크박스 상태 (Submit 시 사용) */
	bool bCheckTodayDoNotShow = false;
};
