// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "Popup_Result.h"

#include "ALingoGameState.h"
#include "ScoreManager.h"
#include "UImageButton.h"
#include "UPopupManager.h"
#include "UPopup_ReadQuest.h"
#include "UTextureButton.h"
#include "Components/TextBlock.h"

void UPopup_Result::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Exit)
	{
		Btn_Exit->OnButtonClickedEvent.RemoveDynamic(this, &UPopup_Result::OnClickClose);
		Btn_Exit->OnButtonClickedEvent.AddDynamic(this, &UPopup_Result::OnClickClose);
	}

	if (Btn_OK)
	{
		Btn_OK->OnButtonClickedEvent.RemoveDynamic(this, &UPopup_Result::OnClickClose);
		Btn_OK->OnButtonClickedEvent.AddDynamic(this, &UPopup_Result::OnClickClose);
	}

	SetTimeTaken();
}

void UPopup_Result::OnClickClose()
{
	// PopupManager를 통해 팝업 닫기 (마우스 커서 처리 포함)
	if (UPopupManager* PopupMgr = UPopupManager::Get(GetWorld()))
	{
		PopupMgr->HideCurrentPopup();
	}
}

void UPopup_Result::SetTimeRank()
{
	FString Rank = "";
	if (UScoreManager* ScoreMgr = UScoreManager::Get(GetWorld()))
	{
		ScoreMgr->GetTimeRank(TimeTaken, Rank);

		Txt_TimeRank->SetText(FText::FromString(Rank));
	}
}

void UPopup_Result::SetTimeTaken()
{
	ALingoGameState* GS = Cast<ALingoGameState>(GetWorld()->GetGameState());
	if (GS)
	{
		float TimeRemain = GS->GetRemainMissionTime();
		TimeTaken = 300 - TimeRemain;

		int32 Minutes = FMath::FloorToInt(TimeRemain / 60.f);
		int32 Seconds = FMath::FloorToInt(TimeRemain) % 60;

		FString Format = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);

		Txt_TimeTaken->SetText(FText::FromString(Format));
	}
}
