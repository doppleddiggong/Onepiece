// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "UPopup_Interview.h"
#include "UPopup_InterviewItem.h"
#include "NetworkData.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/VerticalBox.h"
#include "Components/Spacer.h"
#include "Components/TextBlock.h"
#include "UPopupManager.h"
#include "GameLogging.h"
#include "UImageButton.h"
#include "UTextureButton.h"

void UPopup_Interview::NativeConstruct()
{
	Super::NativeConstruct();

	// 버튼 이벤트 바인딩
	if (Btn_Close)
	{
		Btn_Close->OnButtonClickedEvent.RemoveDynamic(this, &UPopup_Interview::OnClickClose);
		Btn_Close->OnButtonClickedEvent.AddDynamic(this, &UPopup_Interview::OnClickClose);
	}

	if (Btn_Submit)
	{
		Btn_Submit->OnButtonClickedEvent.RemoveDynamic(this, &UPopup_Interview::OnClickSubmit);
		Btn_Submit->OnButtonClickedEvent.AddDynamic(this, &UPopup_Interview::OnClickSubmit);
	}
}

void UPopup_Interview::InitPopup(const FInterviewData& InterviewData)
{
	// 기존 항목 제거
	VerticalBox->ClearChildren();

	// 각 질문에 대해 항목 위젯 생성
	for (int32 i = 0; i < InterviewData.Question.Num(); ++i)
	{
		const FInterviewQuestionData& QuestionData = InterviewData.Question[i];

		// 인터뷰 항목 위젯 생성
		UPopup_InterviewItem* ItemWidget = CreateWidget<UPopup_InterviewItem>(
			GetWorld(), InterviewItemClass);
		ItemWidget->InitItem(QuestionData);
		VerticalBox->AddChildToVerticalBox(ItemWidget);

		// 마지막 항목이 아니면 Spacer 추가
		if (i < InterviewData.Question.Num() - 1)
		{
			USpacer* Spacer = NewObject<USpacer>(this);
			if (Spacer)
			{
				Spacer->SetSize(FVector2D(1.0f, ItemSpacing));
				VerticalBox->AddChildToVerticalBox(Spacer);
			}
		}
	}
}

void UPopup_Interview::OnClickClose()
{
	// PopupManager를 통해 팝업 닫기 (마우스 커서 처리 포함)
	if (UPopupManager* PopupMgr = UPopupManager::Get(GetWorld()))
	{
		PopupMgr->HideCurrentPopup();
	}
}

void UPopup_Interview::OnClickSubmit()
{
	// 모든 답변 수집
	TArray<FString> Answers;
	const TArray<UWidget*>& Children = VerticalBox->GetAllChildren();

	int32 QuestionIndex = 1; // 실제 질문 번호 카운터
	for (int32 i = 0; i < Children.Num(); i++)
	{
		if (UPopup_InterviewItem* Item = Cast<UPopup_InterviewItem>(Children[i]))
		{
			FString Answer = Item->GetAnswer();
			Answers.Add(Answer);
			QuestionIndex++;
		}
	}

	// TODO: 여기에 네트워크 전송 로직 추가 가능
	// 예: UKLingoNetworkSystem::Get(GetWorld())->SubmitInterviewAnswers(Answers);

	// PopupManager를 통해 팝업 닫기
	if (UPopupManager* PopupMgr = UPopupManager::Get(GetWorld()))
	{
		PopupMgr->HideCurrentPopup();
	}
}
