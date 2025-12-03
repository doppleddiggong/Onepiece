// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "Popup_QuestionnaireItem.h"

#include "GameLogging.h"
#include "NetworkData.h"
#include "Popup_WriteBoard.h"
#include "UPopupManager.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"


void UPopup_QuestionnaireItem::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	if (Button_Answer)
	{
		Button_Answer->OnClicked.AddDynamic(this, &UPopup_QuestionnaireItem::OnClickButton);
	}
}

void UPopup_QuestionnaireItem::InitItem(const FWriteQuestionData& Data)
{
	this->QuestionData = Data;
	
	// 질문 인덱스 설정 (예: "Question.01")
	if (Txt_Index)
	{
		FString IndexText = FString::Printf(TEXT("Question.%02d"), Data.Id);
		Txt_Index->SetText(FText::FromString(IndexText));
	}

	// 질문 내용 설정
	if (Text_Question_Kr)
	{
		FString QuestionText = Data.WordData.QuestionKr;
		Text_Question_Kr->SetText(FText::FromString(QuestionText));
	}
	if (Text_Question_En)
	{
		FString QuestionText = Data.WordData.QuestionEn;
		Text_Question_En->SetText(FText::FromString(QuestionText));
	}
}

void UPopup_QuestionnaireItem::OnClickButton()
{
	if (const auto PopupMgr = UPopupManager::Get(GetWorld()))
	{
		if (const auto Popup = Cast<UPopup_WriteBoard>(PopupMgr->ShowPopup(EPopupType::WriteBoard)))
		{
			Popup->InitPopup(QuestionData.Id);
		}
	}
}
