// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "Popup_QuestionnaireResult.h"

#include "GameLogging.h"
#include "Components/Spacer.h"
#include "Components/VerticalBox.h"
#include "Popup_QuestionnaireResultItem.h"

UPopup_QuestionnaireResult::UPopup_QuestionnaireResult(const FObjectInitializer& ObjectInitializer)
{
}

void UPopup_QuestionnaireResult::InitPopup(const FResponseWriteSubmit& InResponseData)
{
	// 피드백 데이터 저장
	ResponseData = InResponseData;
	
	// VerticalBox 초기화
	VerticalBox_Result->ClearChildren();
	
	// TODO: 피드백 팝업 창 생성해야 함.
	// for (const FResponseOcrData& data : ResponseData.ResponseOcrDataArray)
	for (int32 i = 1; i <= ResponseData.ResponseWriteDataArray.Num(); ++i)
	{
		const FResponseWriteData& data = ResponseData.ResponseWriteDataArray[i - 1];
		PRINTLOG(TEXT("%d Success: %s"), i, data.display.is_pass ? TEXT("true") : TEXT("false"));
		PRINTLOG(TEXT("%d Extracted Text: %s"), i, *(data.display.message));
		PRINTLOG(TEXT("%d Extracted Text: %s"), i, *(data.display.correction));
		
		// 인터뷰 항목 위젯 생성
		UPopup_QuestionnaireResultItem* ItemWidget = CreateWidget<UPopup_QuestionnaireResultItem>(
			GetWorld(), QuestionnaireResultItemClass);
		ItemWidget->InitItem(data);
		VerticalBox_Result->AddChildToVerticalBox(ItemWidget);
	
		// 마지막 항목이 아니면 Spacer 추가
		if (i < ResponseData.ResponseWriteDataArray.Num() - 1)
		{
			USpacer* Spacer = NewObject<USpacer>(this);
			if (Spacer)
			{
				Spacer->SetSize(FVector2D(1.0f, ItemSpacing));
				VerticalBox_Result->AddChildToVerticalBox(Spacer);
			}
		}
	}	
}

void UPopup_QuestionnaireResult::NativeConstruct()
{
	Super::NativeConstruct();
}

void UPopup_QuestionnaireResult::OnClickClose()
{
	
}
