// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "UBasePopup.h"
#include "UPopup_Interview.generated.h"

/**
 * 
 */
UCLASS()
class ONEPIECE_API UPopup_Interview : public UBasePopup
{
	GENERATED_BODY()

public:
// 	/** 팝업 초기화 (필요 시 API 확장) */
 	UFUNCTION(BlueprintCallable)
 	void InitPopup(const FInterviewData& InterviewData);
//
// protected:
// 	virtual void NativeConstruct() override;
//
// 	/** 내부 리스트 초기화 */
// 	void BuildInterviewList(const TArray<FString>& InterviewTexts);

protected:
	/* ------------------- Layout ------------------- */

	/** 팝업 전체 배경 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Popup_BaseBg;

	/** 타이틀 프레임 이미지 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Image_TitleFrame;

	/** 팝업 타이틀 텍스트 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_Title;

	/** 닫기 버튼 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Btn_Close;

	/** 제출 버튼 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Btn_Submit;

	/** 스크롤 가능한 인터뷰 패널 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UScrollBox> ScrollBox;

	/** 인터뷰 항목들이 추가될 VerticalBox */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UVerticalBox> VerticalBox;

	/* ------------------- Classes ------------------- */
	/** 팝업 내에서 표시될 인터뷰 항목 위젯 클래스 */
	UPROPERTY(EditDefaultsOnly, Category="Popup")
	TSubclassOf<class UPopup_InterviewItem> InterviewItemClass;
};
