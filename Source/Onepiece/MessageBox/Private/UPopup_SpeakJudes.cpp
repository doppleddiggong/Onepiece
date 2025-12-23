// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "UPopup_SpeakJudes.h"

#include "NetworkData.h"
#include "UImageButton.h"
#include "NetworkData.h"
#include "UPopupManager.h"
#include "UResultStatWidget.h"
#include "UTextureButton.h"
#include "Components/TextBlock.h"

void UPopup_SpeakJudes::InitPopup(const FResponseSpeakingJudes& Response)
{
	if (Btn_Close)
	{
		Btn_Close->OnButtonClickedEvent.RemoveDynamic(this, &UPopup_SpeakJudes::OnClickClose);
		Btn_Close->OnButtonClickedEvent.AddDynamic(this, &UPopup_SpeakJudes::OnClickClose);
	}

	if (Btn_Confirm)
	{
		Btn_Confirm->OnButtonClickedEvent.RemoveDynamic(this, &UPopup_SpeakJudes::OnClickClose);
		Btn_Confirm->OnButtonClickedEvent.AddDynamic(this, &UPopup_SpeakJudes::OnClickClose);
	}

	
	// GetResultStatData로 Grammar, Context 데이터 가져오기
	auto ResultList = Response.GetResultStatData();

	// Grammar 위젯 설정
	Result_Grammer->InitData(ResultList[0]);
	Result_Context->InitData(ResultList[1]);
	Result_Overall->InitData(ResultList[2]);

	// 피드백 텍스트 설정
	Txt_Feedback->SetText(FText::FromString(Response.final_feedback));
}

void UPopup_SpeakJudes::OnClickClose()
{
	// PopupManager를 통해 팝업 닫기 (마우스 커서 처리 포함)
	if (UPopupManager* PopupMgr = UPopupManager::Get(GetWorld()))
	{
		PopupMgr->HideCurrentPopup();
	}
}
