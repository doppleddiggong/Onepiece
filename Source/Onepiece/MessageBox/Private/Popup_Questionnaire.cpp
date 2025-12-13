// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "Popup_Questionnaire.h"

#include "GameLogging.h"
#include "Popup_QuestionnaireItem.h"
#include "UImageButton.h"
#include "UKLingoNetworkSystem.h"
#include "UPopupManager.h"
#include "UTextureButton.h"
#include "Components/Spacer.h"
#include "Components/VerticalBox.h"


UPopup_Questionnaire::UPopup_Questionnaire(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UPopup_QuestionnaireItem> questionnaireItemRef(TEXT("/Game/CustomContents/UI/Widgets/Write/WBP_PopupQuestionnaireItem.WBP_PopupQuestionnaireItem_C"));
	if (questionnaireItemRef.Succeeded())
	{
		QuestionnaireItemClass = questionnaireItemRef.Class;
	}
}

void UPopup_Questionnaire::NativeConstruct()
{
	Super::NativeConstruct();

	// 버튼 이벤트 바인딩
	if (Btn_Close)
	{
		Btn_Close->OnButtonClickedEvent.RemoveDynamic(this, &UPopup_Questionnaire::OnClickClose);
		Btn_Close->OnButtonClickedEvent.AddDynamic(this, &UPopup_Questionnaire::OnClickClose);
	}

	if (Btn_Submit)
	{
		Btn_Submit->OnButtonClickedEvent.RemoveDynamic(this, &UPopup_Questionnaire::OnClickSubmit);
		Btn_Submit->OnButtonClickedEvent.AddDynamic(this, &UPopup_Questionnaire::OnClickSubmit);
	}
}

void UPopup_Questionnaire::InitPopup(const FQuestWriteInfo& QuestionData)
{
	// 질문 데이터 저장
	SavedQuestions = QuestionData.question;

	// 기존 항목 제거
	VerticalBox->ClearChildren();

	// 각 질문에 대해 항목 위젯 생성
	for (int32 i = 0; i < SavedQuestions.Num(); ++i)
	{
		const FWriteQuestionData& Data = SavedQuestions[i];

		// 인터뷰 항목 위젯 생성
		UPopup_QuestionnaireItem* ItemWidget = CreateWidget<UPopup_QuestionnaireItem>(
			GetWorld(), QuestionnaireItemClass);
		ItemWidget->InitItem(Data);
		VerticalBox->AddChildToVerticalBox(ItemWidget);

		// 마지막 항목이 아니면 Spacer 추가
		if (i < SavedQuestions.Num() - 1)
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

void UPopup_Questionnaire::OnClickClose()
{
	// PopupManager를 통해 팝업 닫기 (마우스 커서 처리 포함)
	if (UPopupManager* PopupMgr = UPopupManager::Get(GetWorld()))
	{
		PopupMgr->HideCurrentPopup();
	}
}

void UPopup_Questionnaire::OnClickSubmit()
{	
	// 사진 파일 모으기
	// 네트워크 전송
	if (auto KLingoNetwork = UKLingoNetworkSystem::Get(GetWorld()))
	{
		// TODO: Question마다 png파일 1개로 보내자
		TArray<FString> pngFiles;
		TArray<FString> targetTexts;
		for (const auto& question : SavedQuestions)
		{
			FString imagePath = WriteImagePath + FString::Printf(TEXT("Answer%d.PNG"), question.Id);
			PRINTLOG(TEXT("[TEST] RequestOcrExtract - ImagePath: %s"), *imagePath);
			
			pngFiles.Add(imagePath);
			targetTexts.Add(question.answer_kor);
		}
			
		KLingoNetwork->RequestWriteSubmit(
			pngFiles,
			targetTexts,
			FResponseWriteSubmitDelegate::CreateUObject(this, &UPopup_Questionnaire::OnResponseOcrExtract)
		);
	}
	else
	{
		PRINTLOG(TEXT("UKLingoNetworkSystem not found!"));
	}
}

void UPopup_Questionnaire::OnResponseOcrExtract(FResponseWriteSubmit& ResponseData, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		PRINTLOG(TEXT("--- OCR Extract SUCCESS ---"));
		ResponseData.PrintData();
		
		// TODO: 피드백 팝업 창 생성해야 함.
		// for (const FResponseOcrData& data : ResponseData.ResponseOcrDataArray)
		for (int32 i = 1; i <= ResponseData.ResponseWriteDataArray.Num(); ++i)
		{
			const FResponseWriteData& data = ResponseData.ResponseWriteDataArray[i - 1];
			PRINTLOG(TEXT("%d Success: %s"), i, data.display.is_pass ? TEXT("true") : TEXT("false"));
			PRINTLOG(TEXT("%d Extracted Text: %s"), i, *(data.display.message));
			PRINTLOG(TEXT("%d Extracted Text: %s"), i, *(data.display.correction));
		}
	}
	else
	{
		PRINTLOG(TEXT("--- OCR Extract FAILED ---"));
	}
}