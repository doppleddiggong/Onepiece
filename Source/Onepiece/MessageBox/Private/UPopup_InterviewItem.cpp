// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "UPopup_InterviewItem.h"
#include "NetworkData.h"
#include "Components/TextBlock.h"
#include "Components/MultiLineEditableText.h"

void UPopup_InterviewItem::NativeConstruct()
{
	Super::NativeConstruct();
}

void UPopup_InterviewItem::InitItem(const FInterviewQuestionData& Data)
{
	this->QuestionData = Data;
	
	// 질문 인덱스 설정 (예: "Question.01")
	if (Txt_Index)
	{
		FString IndexText = FString::Printf(TEXT("Question.%02d"), Data.Id);
		Txt_Index->SetText(FText::FromString(IndexText));
	}

	// 질문 내용 설정
	if (Text_Question)
	{
		FString QuestionText = Data.Eng;
		Text_Question->SetText(FText::FromString(QuestionText));
	}

	// 답변 입력란 초기화
	if (Edit_Answer)
		Edit_Answer->SetText(FText::GetEmpty());
}

FString UPopup_InterviewItem::GetAnswer() const
{
	if (Edit_Answer)
		return Edit_Answer->GetText().ToString();
	return FString();
}

FInterviewAnswerData UPopup_InterviewItem::GetAnswerData() const
{
	FInterviewAnswerData AnswerData;
	AnswerData.interview_id = QuestionData.Id;
	AnswerData.answer = GetAnswer();
	AnswerData.user_id = 0;

	return AnswerData;
}
