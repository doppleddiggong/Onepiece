// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "Popup_QuestionnaireItem.h"

#include "GameLogging.h"
#include "NetworkData.h"
#include "Popup_WriteBoard.h"
#include "UImageButton.h"
#include "UPopupManager.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"


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

	// 초기 상태는 답변 안됨
	SetAnswerState(false);
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

		// WriteBoard 팝업이 닫힐 때 답변 저장 완료를 감지하기 위해 델리게이트 바인딩
		// 중복 바인딩 방지를 위해 먼저 제거 후 추가
		Popup->OnCanvasSaved.RemoveDynamic(this, &UPopup_QuestionnaireItem::OnWriteBoardSaved);
		Popup->OnCanvasSaved.AddDynamic(this, &UPopup_QuestionnaireItem::OnWriteBoardSaved);
	}
}

void UPopup_QuestionnaireItem::OnWriteBoardSaved()
{
	// 답변이 저장되었으므로 상태 업데이트
	SetAnswerState(true);
}

void UPopup_QuestionnaireItem::SetAnswerState(bool bAnswered)
{
	if (bIsAnswered != bAnswered)
	{
		bIsAnswered = bAnswered;
		UpdateWidgetSwitcher();

		// 델리게이트 브로드캐스트
		OnAnswerStateChanged.Broadcast(bIsAnswered);
	}
}

void UPopup_QuestionnaireItem::UpdateWidgetSwitcher()
{
	if (WidgetSwitcher)
	{
		// bIsAnswered가 true이면 Image_Checker(인덱스 1) 표시, false이면 Text_Index(인덱스 0) 표시
		WidgetSwitcher->SetActiveWidgetIndex(bIsAnswered ? 1 : 0);
	}
}
