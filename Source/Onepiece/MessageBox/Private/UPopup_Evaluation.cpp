// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "UPopup_Evaluation.h"

#include "GameLogging.h"
#include "OnlineSubsystemUtils.h"
#include "UTabButtonGroup.h"
#include "UEvaluationScenario.h"
#include "UEvaluationTotal.h"
#include "UGameSoundManager.h"
#include "UImageButton.h"
#include "UPopupManager.h"
#include "Kismet/GameplayStatics.h"

void UPopup_Evaluation::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Confirm)
	{
		Btn_Confirm->OnButtonClickedEvent.RemoveDynamic(this, &UPopup_Evaluation::OnClickClose);
		Btn_Confirm->OnButtonClickedEvent.AddDynamic(this, &UPopup_Evaluation::OnClickClose);
	}
}

void UPopup_Evaluation::InitPopup(const FResponseEvaluationResult& InEvaluationResult)
{
	Panel_TotalResult->InitPanel(InEvaluationResult.total_result);

	for (const FScenarioResult& ScenarioResult : InEvaluationResult.scenario_results)
	{
		switch (ScenarioResult.scenario_type)
		{
		case EScenarioType::READING:
			Panel_Reading->InitPanel(ScenarioResult);
			break;

		case EScenarioType::LISTENING:
			Panel_Listening->InitPanel(ScenarioResult);
			break;

		case EScenarioType::WRITING:
			Panel_Writing->InitPanel(ScenarioResult);
			break;

		case EScenarioType::SPEAKING:
			Panel_Speaking->InitPanel(ScenarioResult);
			break;

		default:
			break;
		}
	}
}

void UPopup_Evaluation::OnClickClose()
{
	if (UPopupManager* PopupMgr = UPopupManager::Get(GetWorld()))
	{
		PopupMgr->HidePopup(EPopupType::Evaluation);

		// MsgBox OK 버튼 클릭 시 Lobby로 이동하는 콜백 바인딩
		FOnMsgBoxOkDelegate OnOkDelegate;
		OnOkDelegate.BindUObject(this, &UPopup_Evaluation::OnMsgBoxOk_MoveToLobby);

		UPopupManager::Get(GetWorld())->ShowMsgBox(TEXT("Notice"), TEXT("Thank you For Playing"),
			EMsgBoxType::OK,
			OnOkDelegate);
	}
}

void UPopup_Evaluation::OnMsgBoxOk_MoveToLobby()
{
	// Lobby 맵으로 이동
	if (UWorld* World = GetWorld())
	{		
		if ( UGameSoundManager* SoundMgr = GetGameInstance()->GetSubsystem<UGameSoundManager>())
		{
			SoundMgr->PlayBGM(EGameSoundType::BGM_Start);
		}
		
		UGameplayStatics::OpenLevel(World, FName("/Game/CustomContents/Maps/LobbyMap"));
	}
}