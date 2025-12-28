// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "UBasePopup.h"
#include "FDailyStudy.h"
#include "UPopup_DailyResult.generated.h"

UCLASS()
class ONEPIECE_API UPopup_DailyResult : public UBasePopup
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Popup")
	void InitPopup(const FDailyStudyResult& InResult);

protected:
	virtual void NativeConstruct() override;

private:
	/** 확인 버튼 클릭 */
	UFUNCTION()
	void OnClickConfirm();

protected:
	/** 최종 점수 ("Your Score: 85/100") */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_FinalScore;

	/** 최고 점수 ("Best Score: 90") */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_BestScore;

	/** 완료/총 개수 ("Completed: 9/10") */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_CompletedCount;

	/** 안내 메시지 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_GuideMessage;

	/** 단어 목록 스크롤박스 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UScrollBox> ScrollBox_WordList;

	/** 확인 버튼 (팝업 닫기) */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextureButton> Btn_Confirm;

private:
	/** 결과 요약 데이터 */
	FDailyStudyResult DailyResult;

	/** 단어 항목 위젯 생성 */
	void CreateWordItemWidget(const FDailyStudyWordItem& WordItem, const FDailyStudyAnswer& Answer, int32 Index);
};
