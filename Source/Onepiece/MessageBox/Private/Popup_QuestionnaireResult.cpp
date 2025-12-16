// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "Popup_QuestionnaireResult.h"

#include "GameLogging.h"
#include "Components/Spacer.h"
#include "Popup_QuestionnaireResultItem.h"
#include "UPopupManager.h"
#include "Components/ScrollBox.h"
#include "UImageButton.h"

UPopup_QuestionnaireResult::UPopup_QuestionnaireResult(const FObjectInitializer& ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UPopup_QuestionnaireResultItem> QuestionnaireResultItemRef(TEXT("/Game/CustomContents/UI/Widgets/Write/WBP_WriteResultItem.WBP_WriteResultItem_C"));
	if (QuestionnaireResultItemRef.Succeeded())
	{
		QuestionnaireResultItemClass = QuestionnaireResultItemRef.Class;
	}
}

void UPopup_QuestionnaireResult::InitPopup(const TArray<FWriteWordData> InQuestionArray, const FResponseWriteSubmit& InResponseData)
{
	// 피드백 데이터 저장
	ResponseData = InResponseData;
	// 질문 데이터 저장
	for (const auto& question : InQuestionArray)
	{
		QuestionsKor.Add(question.kor);
		QuestionsEng.Add(question.eng);
	}
	
	// TODO: 최종 Result 불러오기
	
	// 피드백 팝업 창 생성
	// for (const FResponseOcrData& data : ResponseData.ResponseOcrDataArray)
	for (int32 i = 1; i <= ResponseData.ResponseWriteDataArray.Num(); ++i)
	{
		const FResponseWriteData& data = ResponseData.ResponseWriteDataArray[i - 1];
		// PRINTLOG(TEXT("----Display----"));
		// PRINTLOG(TEXT("%d Success: %s"), i, data.display.is_pass ? TEXT("true") : TEXT("false"));
		// PRINTLOG(TEXT("%d Display Message: %s"), i, *(data.display.message));
		// PRINTLOG(TEXT("%d Display Correction: %s"), i, *(data.display.correction));
		// PRINTLOG(TEXT("----Record----"));
		// PRINTLOG(TEXT("%d Score: %d"), i, data.record.score);
		// PRINTLOG(TEXT("%d Record Target: %s"), i, *(data.record.target));
		// PRINTLOG(TEXT("%d Record Input: %s"), i, *(data.record.input));
		// PRINTLOG(TEXT("%d Record Stage: %s"), i, *(data.record.stage));
		
		// 인터뷰 항목 위젯 생성
		UPopup_QuestionnaireResultItem* ItemWidget = CreateWidget<UPopup_QuestionnaireResultItem>(
			GetWorld(), QuestionnaireResultItemClass);
		ItemWidget->InitItem(i, QuestionsKor[i - 1], data);
		ScrollBox_Result->AddChild(ItemWidget);
	
		// 마지막 항목이 아니면 Spacer 추가
		if (i < ResponseData.ResponseWriteDataArray.Num() - 1)
		{
			USpacer* Spacer = NewObject<USpacer>(this);
			if (Spacer)
			{
				Spacer->SetSize(FVector2D(1.0f, ItemSpacing));
				ScrollBox_Result->AddChild(Spacer);
			}
		}
	}	
}

void UPopup_QuestionnaireResult::NativeConstruct()
{
	Super::NativeConstruct();
	
	Btn_Confirm->OnButtonClickedEvent.AddDynamic(this, &UPopup_QuestionnaireResult::OnClickClose);
}

void UPopup_QuestionnaireResult::OnClickClose()
{
	// PopupManager를 통해 팝업 닫기 (마우스 커서 처리 포함)
	if (UPopupManager* PopupMgr = UPopupManager::Get(GetWorld()))
	{
		PopupMgr->HideCurrentPopup();
	}
}
