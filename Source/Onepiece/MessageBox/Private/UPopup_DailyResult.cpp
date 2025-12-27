// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "UPopup_DailyResult.h"

#include "UPopup_DailyStudy.h"
#include "UPopupManager.h"
#include "UTextureButton.h"

#include "Components/TextBlock.h"

void UPopup_DailyResult::NativeConstruct()
{
	Super::NativeConstruct();

	// 버튼 바인딩
	if (Btn_Confirm)
	{
		Btn_Confirm->OnButtonClickedEvent.RemoveDynamic(this, &UPopup_DailyResult::OnClickConfirm);
		Btn_Confirm->OnButtonClickedEvent.AddDynamic(this, &UPopup_DailyResult::OnClickConfirm);
	}
}

// ========================================
// Public Methods
// ========================================

void UPopup_DailyResult::InitPopup(const FDailyStudyResult& InResult)
{
	// 결과 집계
	DailyResult = InResult;

	Txt_FinalScore->SetText(FText::FromString(
		FString::Printf(TEXT("Your Score: %d/100"), DailyResult.AvgFinalScore)));
	
	Txt_CompletedCount->SetText(FText::FromString(
		FString::Printf(TEXT("Completed: %d/%d"), DailyResult.CompletedCount, DailyResult.TotalCount)));
}

// ========================================
// Button Events
// ========================================

void UPopup_DailyResult::OnClickConfirm()
{
	if ( auto PopupMgr = UPopupManager::Get(GetWorld()) )
	{
		PopupMgr->HideCurrentPopup();
	}
}