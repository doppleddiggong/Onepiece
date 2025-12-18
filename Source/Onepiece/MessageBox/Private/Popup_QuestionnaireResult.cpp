// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "Popup_QuestionnaireResult.h"

#include "ALingoPlayerState.h"
#include "ANetworkBroadcastActor.h"
#include "FResultStatData.h"
#include "GameLogging.h"
#include "Components/Spacer.h"
#include "Popup_QuestionnaireResultItem.h"
#include "UBroadcastManager.h"
#include "UPopupManager.h"
#include "Components/ScrollBox.h"
#include "UImageButton.h"
#include "ULingoGameHelper.h"
#include "UResultStatWidget.h"
#include "Components/VerticalBox.h"
#include "Onepiece/Onepiece.h"

UPopup_QuestionnaireResult::UPopup_QuestionnaireResult(const FObjectInitializer& ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UPopup_QuestionnaireResultItem> QuestionnaireResultItemRef(TEXT("/Game/CustomContents/UI/Widgets/Write/WBP_WriteResultItem.WBP_WriteResultItem_C"));
	if (QuestionnaireResultItemRef.Succeeded())
	{
		QuestionnaireResultItemClass = QuestionnaireResultItemRef.Class;
	}
}

void UPopup_QuestionnaireResult::InitPopup(const FResponseWriteSubmit& InResponseData, const FResponseWriteResult& InResponseResultData)
{
	// 피드백 데이터 저장
	ResponseData = InResponseData;
	// 질문 데이터 저장
	TArray<FWriteQuestionData> QuestionArray = ULingoGameHelper::GetLingoPlayerState(GetWorld())->WriteQuestionData.question;
	for (const auto& question : QuestionArray)
	{
		QuestionsKor.Add(question.word_data.kor);
		QuestionsEng.Add(question.word_data.eng);
	}
	
	// TODO: 최종 Result 불러오기
	// GameState에서 결과 확인
	if (auto PS = ULingoGameHelper::GetLingoPlayerState(GetWorld()))
	{
		bool bHasResult = false;

		if (!PS->WriteWholeResultData.grade.IsEmpty())
		{
			// 이미 결과가 있으면 바로 표시
			PRINTLOG(TEXT("[Popup_Result] Read result already exists in GameState"));
			InitWholeResult(PS->WriteWholeResultData);
			bHasResult = true;
		}

		// 결과가 없으면 Host만 요청
		if (!bHasResult && GetOwningPlayer()->HasAuthority())
		{
			PRINTLOG(TEXT("[Popup_Result] 없어요"));
		}
	}
	
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

void UPopup_QuestionnaireResult::InitWholeResult(const FResponseWriteResult& InResponseData)
{
	if (auto GS = Cast<ALingoGameState>(GetWorld()->GetGameState()))
	{
		// Symbol로 타임 처리
		auto TimeTaken = GS->GetTimeTaken();
		const int32 Minutes = FMath::FloorToInt(TimeTaken / 60.f);
		const int32 Seconds = FMath::FloorToInt(TimeTaken) % 60;

		FResultStatData TimeResultData;
		TimeResultData.WidgetType = EResultItemWidgetType::Symbol;
		TimeResultData.ColorType = EColorStyleType::Gray;
		TimeResultData.TitleText = FText::FromString(TEXT("Time"));
		TimeResultData.SymbolTextureType = EResourceTextureType::Time;
		TimeResultData.SymbolValue = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		Result_Time->InitData(TimeResultData);
		PRINTLOG(TEXT("[Popup_Result] Time: %d:%d"), Minutes, Seconds);
	}

	FResultStatData GradeResultData;
	GradeResultData.WidgetType = EResultItemWidgetType::Grade;
	GradeResultData.ColorType = EColorStyleType::Gray;
	GradeResultData.TitleText = FText::FromString(TEXT("Grade"));
	GradeResultData.GradeTextureType = ULingoGameHelper::ConvertGradeString(InResponseData.grade);
	Result_Grade->InitData(GradeResultData);
	PRINTLOG(TEXT("[Popup_Result] Grade: %s"), *(InResponseData.grade));

	FResultStatData TopRateResultData;
	TopRateResultData.WidgetType = EResultItemWidgetType::Rate;
	TopRateResultData.ColorType = EColorStyleType::Gray;
	TopRateResultData.TitleText = FText::FromString(TEXT("Rate"));
	TopRateResultData.RatePercent = InResponseData.top_percent;
	Result_TopRate->InitData(TopRateResultData);
	PRINTLOG(TEXT("[Popup_Result] Rate: %d"), InResponseData.top_percent);
		
	FResultStatData AverageScoreResultData;
	AverageScoreResultData.WidgetType = EResultItemWidgetType::Symbol;
	AverageScoreResultData.ColorType = EColorStyleType::Gray;
	AverageScoreResultData.TitleText = FText::FromString(TEXT("Score"));
	AverageScoreResultData.SymbolTextureType = EResourceTextureType::Score;
	AverageScoreResultData.SymbolValue = FString::Printf(TEXT("%d"), InResponseData.average_score);
	Result_AverageScore->InitData(AverageScoreResultData);
	PRINTLOG(TEXT("[Popup_Result] Average Score: %d"), InResponseData.average_score);
}

void UPopup_QuestionnaireResult::OnClickClose()
{
	// PopupManager를 통해 팝업 닫기 (마우스 커서 처리 포함)
	if (UPopupManager* PopupMgr = UPopupManager::Get(GetWorld()))
	{
		PopupMgr->HideCurrentPopup();
	}

	UBroadcastManager::Get(GetWorld())->SendDoorMessage(DoorGroup::Step4_End, true);
}