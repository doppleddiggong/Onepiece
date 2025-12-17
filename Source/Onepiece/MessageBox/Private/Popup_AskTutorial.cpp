// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "Popup_AskTutorial.h"

#include "APlayerControl.h"
#include "UImageButton.h"
#include "ULingoGameHelper.h"
#include "UPopupManager.h"

void UPopup_AskTutorial::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Yes)
	{
		Btn_Yes->OnButtonClickedEvent.RemoveDynamic(this, &UPopup_AskTutorial::OnClickYes);
		Btn_Yes->OnButtonClickedEvent.AddDynamic(this, &UPopup_AskTutorial::OnClickYes);
	}

	if (Btn_Skip)
	{
		Btn_Skip->OnButtonClickedEvent.RemoveDynamic(this, &UPopup_AskTutorial::OnClickSkip);
		Btn_Skip->OnButtonClickedEvent.AddDynamic(this, &UPopup_AskTutorial::OnClickSkip);
	}
}

void UPopup_AskTutorial::OnClickYes()
{
	if (UPopupManager* PopupMgr = UPopupManager::Get(GetWorld()))
	{
		PopupMgr->HideCurrentPopup();
	}

	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (APlayerControl* PlayerControl = Cast<APlayerControl>(PC))
		{
			PlayerControl->StartTutorialManually();
		}
	}
}

void UPopup_AskTutorial::OnClickSkip()
{
	// 세이브 시스템에 여부 저장
	const int32 UserId = ULingoGameHelper::GetUserId(GetWorld());
	const FString ConfigSection = TEXT("/Script/Onepiece.TutorialSystem");
	const FString ConfigKey = FString::Printf(TEXT("TutorialCompleted_%d"), UserId);

	GConfig->SetString(
				*ConfigSection,
				*ConfigKey,
				TEXT("true"),
				GGameUserSettingsIni
			);
	GConfig->Flush(false, GGameUserSettingsIni);
	
	// 일단 그냥 창 닫히게
	if (UPopupManager* PopupMgr = UPopupManager::Get(GetWorld()))
	{
		PopupMgr->HideCurrentPopup();
	}
}
