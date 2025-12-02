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
#include "UKLingoNetworkSystem.h"
#include "UDialogManager.h"

void UPopup_Interview::NativeConstruct()
{
	Super::NativeConstruct();
}

void UPopup_Interview::InitPopup(const FResponseInterviewHello& InterviewData)
{
	// 중복 바인딩 방지: 기존 바인딩 제거 후 재바인딩
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

	// 질문 데이터 저장
	SavedQuestions = InterviewData.Questions;

	// 기존 항목 제거
	VerticalBox->ClearChildren();

	// 각 질문에 대해 항목 위젯 생성
	for (int32 i = 0; i < InterviewData.Questions.Num(); ++i)
	{
		const FInterviewQuestionData& QuestionData = InterviewData.Questions[i];

		// 인터뷰 항목 위젯 생성
		UPopup_InterviewItem* ItemWidget = CreateWidget<UPopup_InterviewItem>(
			GetWorld(), InterviewItemClass);
		ItemWidget->InitItem(QuestionData);
		VerticalBox->AddChildToVerticalBox(ItemWidget);

		// 마지막 항목이 아니면 Spacer 추가
		if (i < InterviewData.Questions.Num() - 1)
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
	// 모든 답변 수집 및 검증
	TArray<FInterviewAnswerData> AnswerDataList;
	const TArray<UWidget*>& Children = VerticalBox->GetAllChildren();

	for (int32 i = 0; i < Children.Num(); i++)
	{
		if (UPopup_InterviewItem* Item = Cast<UPopup_InterviewItem>(Children[i]))
		{
			FString Answer = Item->GetAnswer().TrimStartAndEnd();

			// 빈 답변 체크
			if (Answer.IsEmpty())
			{
				// Toast 메시지로 알림
				if (UDialogManager* DialogMgr = UDialogManager::Get(GetWorld()))
				{
					FString Message = FString::Printf(TEXT("Question is not answered. Please fill in all answers."));
					DialogMgr->ShowToast(Message);
				}
				return;
			}

			// 답변 데이터 생성
			AnswerDataList.Add( Item->GetAnswerData() );
		}
	}

	// 네트워크 전송
	if (UKLingoNetworkSystem* LingoNetworkSystem = UKLingoNetworkSystem::Get(GetWorld()))
	{
		FRequestInterviewAnswer Request;
		Request.answer = AnswerDataList;

		LingoNetworkSystem->RequestInterviewAnswer( Request,
			FResponseInterviewAnswerDelegate::CreateUObject(this, &UPopup_Interview::OnResponseInterviewAnswer));
	}
}

void UPopup_Interview::OnResponseInterviewAnswer(FResponseInterviewAnswer& ResponseData, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		PRINTLOG(TEXT("--- Interview Answer SUCCESS ---"));
		ResponseData.PrintData();

		// 성공 시 토스트 메시지 표시
		if (UDialogManager* DialogMgr = UDialogManager::Get(GetWorld()))
		{
			DialogMgr->ShowToast(TEXT("Interview answers submitted successfully!"));
		}

		// 팝업 닫기
		if (UPopupManager* PopupMgr = UPopupManager::Get(GetWorld()))
		{
			PopupMgr->HideCurrentPopup();
		}
	}
	else
	{
		PRINTLOG(TEXT("--- Interview Answer FAILED ---"));

		// 실패 시 토스트 메시지 표시
		if (UDialogManager* DialogMgr = UDialogManager::Get(GetWorld()))
		{
			DialogMgr->ShowToast(TEXT("Failed to submit interview answers. Please try again."));
		}
	}
}