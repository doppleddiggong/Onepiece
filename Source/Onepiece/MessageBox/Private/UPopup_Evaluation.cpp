// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "UPopup_Evaluation.h"
#include "Components/WidgetSwitcher.h"
#include "UTabButtonGroup.h"
#include "UPopup_EvaluationScenarioResultItem.h"
#include "UPopup_EvaluationTotalResultItem.h"
#include "UImageButton.h"
#include "UPopupManager.h"

void UPopup_Evaluation::NativeConstruct()
{
	Super::NativeConstruct();

	// 탭 선택 이벤트 바인딩
	if (TabButtonGroup)
	{
		TabButtonGroup->OnTabSelected.RemoveDynamic(this, &UPopup_Evaluation::OnTabSelected);
		TabButtonGroup->OnTabSelected.AddDynamic(this, &UPopup_Evaluation::OnTabSelected);
	}

	// 닫기 버튼 이벤트 바인딩
	if (Btn_Close)
	{
		Btn_Close->OnButtonClickedEvent.RemoveDynamic(this, &UPopup_Evaluation::OnClickClose);
		Btn_Close->OnButtonClickedEvent.AddDynamic(this, &UPopup_Evaluation::OnClickClose);
	}
}

void UPopup_Evaluation::InitPopup(const FResponseEvaluationResult& InEvaluationResult)
{
	// 1. Total Result 패널 초기화
	if (Panel_TotalResult)
	{
		Panel_TotalResult->InitPanel(InEvaluationResult.total_result);
	}

	// 2. Scenario Result 패널들 초기화
	// scenario_results 배열에서 각 타입별 데이터를 찾아서 초기화
	for (const FScenarioResult& ScenarioResult : InEvaluationResult.scenario_results)
	{
		switch (ScenarioResult.scenario_type)
		{
		case EScenarioType::READING:
			if (Panel_Reading)
			{
				Panel_Reading->InitPanel(ScenarioResult);
			}
			break;

		case EScenarioType::LISTENING:
			if (Panel_Listening)
			{
				Panel_Listening->InitPanel(ScenarioResult);
			}
			break;

		case EScenarioType::WRITING:
			if (Panel_Writing)
			{
				Panel_Writing->InitPanel(ScenarioResult);
			}
			break;

		case EScenarioType::SPEAKING:
			if (Panel_Speaking)
			{
				Panel_Speaking->InitPanel(ScenarioResult);
			}
			break;

		default:
			break;
		}
	}

	// 3. 기본적으로 첫 번째 탭(Total Result) 활성화
	if (WidgetSwitcher)
	{
		WidgetSwitcher->SetActiveWidgetIndex(0);
	}

	if (TabButtonGroup)
	{
		TabButtonGroup->OnSelectTab(0, false);
	}
}

void UPopup_Evaluation::OnTabSelected(int32 TabIndex)
{
	// 탭 인덱스에 따라 WidgetSwitcher 활성화
	if (WidgetSwitcher)
	{
		WidgetSwitcher->SetActiveWidgetIndex(TabIndex);
	}
}

void UPopup_Evaluation::OnClickClose()
{
	// 팝업 닫기
	if (UPopupManager* PopupMgr = UPopupManager::Get(GetWorld()))
	{
		PopupMgr->HidePopup(EPopupType::Evaluation);
	}
}
