// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "UBasePopup.h"
#include "FDailyStudyResult.h"
#include "UPopup_DailyResult.generated.h"

/**
 * @brief Daily Study 결과 팝업
 * 
 * Daily Study에서 학습한 단어들의 결과를 스크롤 가능한 리스트로 표시합니다.
 * UPopup_SpeakResult를 참조하여 구현되었습니다.
 */
UCLASS()
class ONEPIECE_API UPopup_DailyResult : public UBasePopup
{
	GENERATED_BODY()

public:
	/**
	 * @brief Daily Study 결과로 팝업 초기화
	 * @param Result Daily Study 결과 데이터
	 */
	UFUNCTION(BlueprintCallable, Category = "Popup")
	void InitPopup(const FDailyStudyResult& Result);

protected:
	virtual void NativeConstruct() override;

private:
	/** 확인 버튼 클릭 */
	UFUNCTION()
	void OnClickConfirm();

	/** 질문 리스트 초기화 (스크롤 가능한 아이템 생성) */
	void InitQuestionList();

protected:
	/** 결과 리스트를 담는 VerticalBox (스크롤 가능) */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UVerticalBox> VerticalBox;

	/** 확인 버튼 (팝업 닫기) */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImageButton> Btn_Confirm;

private:
	/** 아이템 위젯 클래스 */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UPopup_DailyResultItem> AnswerItemClass;

	/** Daily Study 결과 데이터 */
	FDailyStudyResult StudyResult;
};
