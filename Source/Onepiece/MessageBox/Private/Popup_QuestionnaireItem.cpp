// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "Popup_QuestionnaireItem.h"

#include "GameLogging.h"
#include "NetworkData.h"
#include "Popup_WriteBoard.h"
#include "UImageButton.h"
#include "UPopupManager.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"


void UPopup_QuestionnaireItem::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (Button_Answer)
	{
		Button_Answer->OnButtonClickedEvent.RemoveDynamic(this, &UPopup_QuestionnaireItem::OnClickButton);
		Button_Answer->OnButtonClickedEvent.AddDynamic(this, &UPopup_QuestionnaireItem::OnClickButton);
	}
}

void UPopup_QuestionnaireItem::InitItem(const FWriteQuestionData& Data)
{
	this->QuestionData = Data;
	
	// 질문 인덱스 설정 (예: "Question.01")
	if (Text_Index)
	{
		FString IndexText = FString::Printf(TEXT("Q%d"), Data.Id);
		Text_Index->SetText(FText::FromString(IndexText));
	}

	// FString Description = FString::Printf(TEXT("%s\n[%s]"),
	// 	*Data.word_data.kor,
	// 	*Data.word_data.eng);
	//
	// // 질문 내용 설정
	// Text_Question->SetText(FText::FromString(Description));

	Text_Question->SetText(FText::FromString(Data.word_data.eng));
}

void UPopup_QuestionnaireItem::OnClickButton()
{
	if (auto Popup = UPopupManager::ShowPopupAs<UPopup_WriteBoard>(GetWorld(), EPopupType::WriteBoard))
	{
		// TODO: 단어 수 및 글자 수 구하기
		TArray<FString> Tokens;
		QuestionData.answer_kor.ParseIntoArrayWS(Tokens);
		// for (const auto& token : Tokens)
		// {
		// 	PRINT_STRING(TEXT("Question 글 : %s"), *token);
		// }
		// PRINT_STRING(TEXT("Question 단어 수 : %d"), Tokens.Num());
		// PRINT_STRING(TEXT("Question 글자 수 : %d"), QuestionData.answer_kor.Len());
		// PRINT_STRING(TEXT("Question 첫 단어 글자 수 : %d"), Tokens[0].Len());
		
		Popup->InitPopup(QuestionData.Id, QuestionData);
	}
}
