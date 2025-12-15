// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "UPopup_SpeakResult.h"

#include "FResultStatData.h"
#include "UAnswerItem.h"
#include "UImageButton.h"
#include "ULingoGameHelper.h"
#include "UPopupManager.h"
#include "UResultStatWidget.h"


void UPopup_SpeakResult::InitPopup(const FResponseSpeakResult& ResponseData)
{
	if (Btn_Confirm)
	{
		Btn_Confirm->OnButtonClickedEvent.RemoveDynamic(this, &UPopup_SpeakResult::OnClickClose);
		Btn_Confirm->OnButtonClickedEvent.AddDynamic(this, &UPopup_SpeakResult::OnClickClose);
	}

	this->InitSpeakResult(ResponseData);
}

void UPopup_SpeakResult::OnClickClose()
{
	// PopupManager를 통해 팝업 닫기 (마우스 커서 처리 포함)
	if (UPopupManager* PopupMgr = UPopupManager::Get(GetWorld()))
	{
		PopupMgr->HideCurrentPopup();
	}
}

void UPopup_SpeakResult::InitSpeakResult(const FResponseSpeakResult& ResponseData)
{
	FResultStatData GradeResultData;
	GradeResultData.WidgetType = EResultItemWidgetType::Grade;
	GradeResultData.ColorType = EColorStyleType::Green;
	GradeResultData.TitleText = FText::FromString(TEXT("GRADE"));
	GradeResultData.GradeTextureType = ULingoGameHelper::ConvertGradeString(ResponseData.grade);
	Result_Grade->InitData(GradeResultData);

	FResultStatData TopRateResultData;
	TopRateResultData.WidgetType = EResultItemWidgetType::Rate;
	TopRateResultData.ColorType = EColorStyleType::Red;
	TopRateResultData.TitleText = FText::FromString(TEXT("TOP"));
	TopRateResultData.RatePercent = ResponseData.top_percent;
	Result_TopRate->InitData(TopRateResultData);
		
	FResultStatData AverageScoreResultData;
	AverageScoreResultData.WidgetType = EResultItemWidgetType::Symbol;
	AverageScoreResultData.ColorType = EColorStyleType::Purple;
	AverageScoreResultData.TitleText = FText::FromString(TEXT("SCORE"));
	AverageScoreResultData.SymbolTextureType = EResourceTextureType::Score;
	AverageScoreResultData.SymbolValue = FString::Printf(TEXT("%d"), ResponseData.average_score);
	Result_AverageScore->InitData(AverageScoreResultData);
}