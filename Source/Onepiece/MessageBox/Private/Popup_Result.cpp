// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "Popup_Result.h"

#include "ALingoGameState.h"
#include "APlayerControl.h"
#include "FResultStatData.h"
#include "GameLogging.h"
#include "UImageButton.h"
#include "UKLingoNetworkSystem.h"
#include "ULingoGameHelper.h"
#include "UPopupManager.h"
#include "UResultStatWidget.h"
#include "UTextureButton.h"
#include "UAnswerItem.h"
#include "Components/HorizontalBox.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "Components/VerticalBox.h"

void UPopup_Result::InitPopup(const EQuestType InQuestType)
{
	if (Btn_Exit)
	{
		Btn_Exit->OnButtonClickedEvent.RemoveDynamic(this, &UPopup_Result::OnClickClose);
		Btn_Exit->OnButtonClickedEvent.AddDynamic(this, &UPopup_Result::OnClickClose);
	}

	if (Btn_OK)
	{
		Btn_OK->OnButtonClickedEvent.RemoveDynamic(this, &UPopup_Result::OnClickClose);
		Btn_OK->OnButtonClickedEvent.AddDynamic(this, &UPopup_Result::OnClickClose);
	}

	this->QuestType =InQuestType;

	InitWordWidget();
	InitWrongList();

	// GameState에서 결과 확인
	if (auto GS = Cast<ALingoGameState>(GetWorld()->GetGameState()))
	{
		bool bHasResult = false;

		if (QuestType == EQuestType::Read && !GS->ReadResult.grade.IsEmpty())
		{
			// 이미 결과가 있으면 바로 표시
			PRINTLOG(TEXT("[Popup_Result] Read result already exists in GameState"));
			InitReadResult(GS->ReadResult);
			bHasResult = true;
		}
		else if (QuestType == EQuestType::Listen && !GS->ListenResult.grade.IsEmpty())
		{
			// 이미 결과가 있으면 바로 표시
			PRINTLOG(TEXT("[Popup_Result] Listen result already exists in GameState"));
			InitListenResult(GS->ListenResult);
			bHasResult = true;
		}

		// 결과가 없으면 Host만 요청
		if (!bHasResult)
		{
			RequestResult();
		}
	}
}

void UPopup_Result::OnClickClose()
{
	// PopupManager를 통해 팝업 닫기 (마우스 커서 처리 포함)
	if (UPopupManager* PopupMgr = UPopupManager::Get(GetWorld()))
	{
		PopupMgr->HideCurrentPopup();
	}
}

void UPopup_Result::InitWordWidget()
{
	if (auto GS = Cast<ALingoGameState>(GetWorld()->GetGameState()))
	{
		if ( QuestType == EQuestType::Read)
		{
			Txt_Kor->SetText(FText::FromString(GS->ReadScenarioData.full_data.Kor));
			Txt_Eng->SetText(FText::FromString(GS->ReadScenarioData.full_data.Eng));
		}
		else if ( QuestType == EQuestType::Listen )
		{
			Txt_Kor->SetText(FText::FromString(GS->ListenScenarioData.full_data.Kor));
			Txt_Eng->SetText(FText::FromString(GS->ListenScenarioData.full_data.Eng));
		}
	}
}

void UPopup_Result::InitWrongList()
{
	ALingoGameState* GS = Cast<ALingoGameState>(GetWorld()->GetGameState());
	if (!GS)
		return;
	
	// 틀린 인덱스 리스트
	TArray<int32> WrongList;
	// 전체 캐리어 정보
	TArray<FScenarioTargetData> ScenarioData;
	
	if ( QuestType == EQuestType::Read)
	{
		WrongList = GS->WrongReadAnswerList;
		ScenarioData = GS->GetReadScenarioData().target_data;
	}
	else if ( QuestType == EQuestType::Listen )
	{
		WrongList = GS->WrongListenAnswerList;
		ScenarioData = GS->GetListenScenarioData().target_data;
	}
	
	// 기존 항목 제거
	VerticalBox->ClearChildren();
	// WrongList의 각 항목을 UAnswerItem으로 추가
	for (int32 i = 0; i < WrongList.Num(); i++)
	{
		const FScenarioTargetData& SD = ScenarioData[WrongList[i]];

		// UAnswerItem 위젯 생성
		if (AnswerItemClass)
		{
			UAnswerItem* AnswerItem = CreateWidget<UAnswerItem>(this, AnswerItemClass);
			if (AnswerItem)
			{
				// 마지막 인덱스는 정답
				const bool bCorrect = (i == WrongList.Num() - 1);
				const int32 Order = i + 1;
				const int32 Word1Code = FCString::Atoi(*SD.word1.code);
				const int32 Word2Code = FCString::Atoi(*SD.word2.code);

				// AnswerItem 초기화
				AnswerItem->InitInfo(QuestType, bCorrect, Order, Word1Code, Word2Code);

				// HorizontalBox에 추가
				VerticalBox->AddChild(AnswerItem);
			}
		}
	}
}

void UPopup_Result::InitReadResult(const FResponseReadResult& ResponseData)
{
	if (auto GS = Cast<ALingoGameState>(GetWorld()->GetGameState()))
	{
		// Symbol로 타임 처리
		auto TimeTaken = GS->GetTimeTaken();
		const int32 Minutes = FMath::FloorToInt(TimeTaken / 60.f);
		const int32 Seconds = FMath::FloorToInt(TimeTaken) % 60;

		FResultStatData TimeResultData;
		TimeResultData.WidgetType = EResultItemWidgetType::Symbol;
		TimeResultData.ColorType = EColorStyleType::Blue;
		TimeResultData.TitleText = FText::FromString(TEXT("TIME"));
		TimeResultData.SymbolTextureType = EResourceTextureType::Time;
		TimeResultData.SymbolValue = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		Result_Time->InitData(TimeResultData);
	}

	FResultStatData GradeResultData;
	GradeResultData.WidgetType = EResultItemWidgetType::Grade;
	GradeResultData.ColorType = EColorStyleType::Green;
	GradeResultData.TitleText = FText::FromString(TEXT("GRADE"));
	GradeResultData.GradeTextureType = ULingoGameHelper::ConvertGradeString(ResponseData.grade);
	Result_Grade->InitData(GradeResultData);

	FResultStatData TopRateResultData;
	TopRateResultData.WidgetType = EResultItemWidgetType::Rate;
	TopRateResultData.ColorType = EColorStyleType::Red;
	TopRateResultData.TitleText = FText::FromString(TEXT("TOP"));
	TopRateResultData.RatePercent = ResponseData.top_percent;
	Result_TopRate->InitData(TopRateResultData);
		
	FResultStatData AverageScoreResultData;
	AverageScoreResultData.WidgetType = EResultItemWidgetType::Symbol;
	AverageScoreResultData.ColorType = EColorStyleType::Purple;
	AverageScoreResultData.TitleText = FText::FromString(TEXT("SCORE"));
	AverageScoreResultData.SymbolTextureType = EResourceTextureType::Score;
	AverageScoreResultData.SymbolValue = FString::Printf(TEXT("%d"), ResponseData.average_score);
	Result_AverageScore->InitData(AverageScoreResultData);
}

void UPopup_Result::InitListenResult(const FResponseListenResult& ResponseData)
{
	if (auto GS = Cast<ALingoGameState>(GetWorld()->GetGameState()))
	{
		// Symbol로 타임 처리
		auto TimeTaken = GS->GetTimeTaken();
		const int32 Minutes = FMath::FloorToInt(TimeTaken / 60.f);
		const int32 Seconds = FMath::FloorToInt(TimeTaken) % 60;

		FResultStatData TimeResultData;
		TimeResultData.WidgetType = EResultItemWidgetType::Symbol;
		TimeResultData.ColorType = EColorStyleType::Blue;
		TimeResultData.TitleText = FText::FromString(TEXT("TIME"));
		TimeResultData.SymbolTextureType = EResourceTextureType::Time;
		TimeResultData.SymbolValue = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		Result_Time->InitData(TimeResultData);
	}

	FResultStatData GradeResultData;
	GradeResultData.WidgetType = EResultItemWidgetType::Grade;
	GradeResultData.ColorType = EColorStyleType::Green;
	GradeResultData.TitleText = FText::FromString(TEXT("GRADE"));
	GradeResultData.GradeTextureType = ULingoGameHelper::ConvertGradeString(ResponseData.grade);
	Result_Grade->InitData(GradeResultData);

	FResultStatData TopRateResultData;
	TopRateResultData.WidgetType = EResultItemWidgetType::Rate;
	TopRateResultData.ColorType = EColorStyleType::Red;
	TopRateResultData.TitleText = FText::FromString(TEXT("TOP"));
	TopRateResultData.RatePercent = ResponseData.top_percent;
	Result_TopRate->InitData(TopRateResultData);
		
	FResultStatData AverageScoreResultData;
	AverageScoreResultData.WidgetType = EResultItemWidgetType::Symbol;
	AverageScoreResultData.ColorType = EColorStyleType::Purple;
	AverageScoreResultData.TitleText = FText::FromString(TEXT("SCORE"));
	AverageScoreResultData.SymbolTextureType = EResourceTextureType::Score;
	AverageScoreResultData.SymbolValue = FString::Printf(TEXT("%d"), ResponseData.average_score);
	Result_AverageScore->InitData(AverageScoreResultData);
}

void UPopup_Result::RequestResult()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC)
		return;

	// Host 플레이어(첫 번째 플레이어)인지 확인
	bool bIsHost = false;
	if (APlayerState* PS = PC->GetPlayerState<APlayerState>())
	{
		if (AGameStateBase* GameStateBase = GetWorld()->GetGameState())
		{
			int32 PlayerIndex = GameStateBase->PlayerArray.IndexOfByKey(PS);
			bIsHost = (PlayerIndex == 0);
		}
	}

	// Host가 아니면 요청하지 않음
	if (!bIsHost)
	{
		PRINTLOG(TEXT("[Popup_Result] Guest player - skipping request, waiting for Host"));
		return;
	}

	PRINTLOG(TEXT("[Popup_Result] Host player - sending result request"));

	if (auto GS = Cast<ALingoGameState>(GetWorld()->GetGameState()))
	{
		// 네트워크 송신 (Host만)
		if (UKLingoNetworkSystem* Network = UKLingoNetworkSystem::Get(GetWorld()))
		{
			if ( QuestType == EQuestType::Read )
			{
				FRequestReadResult ReadRequest;
				ReadRequest.room_id	= GS->GetRoomId();
				ReadRequest.user_id = Cast<APlayerControl>(GetOwningPlayer())->GetUserId();
				ReadRequest.scenario_id = 1;
				ReadRequest.stage_type = ULingoGameHelper::GetStageTypeIndex(QuestType);
				ReadRequest.state_type = 0;
				ReadRequest.result_time = GS->GetTimeTaken();
				ReadRequest.wrong_idx = GS->WrongReadAnswerList;
				Network->RequestReadResult(ReadRequest,
						FResponseReadResultDelegate::CreateUObject(this, &UPopup_Result::OnResponseReadResult));
			}
			else if ( QuestType == EQuestType::Listen )
			{
				FRequestListenResult ListenRequest;
				ListenRequest.room_id = GS->GetRoomId();
				ListenRequest.user_id = Cast<APlayerControl>(GetOwningPlayer())->GetUserId();
				ListenRequest.scenario_id = 1;
				ListenRequest.stage_type = ULingoGameHelper::GetStageTypeIndex(QuestType);
				ListenRequest.state_type = 0;
				ListenRequest.result_time = GS->GetTimeTaken();
				ListenRequest.wrong_idx = GS->WrongListenAnswerList;
				Network->RequestListenResult(ListenRequest,
						FResponseListenResultDelegate::CreateUObject(this, &UPopup_Result::OnResponseListenResult));
			}
		}
	}
}

void UPopup_Result::OnResponseReadResult(FResponseReadResult& ResponseData, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		PRINTLOG(TEXT("[Result] Grade: %s, average_score: %d, Top Percent: %.2f%%"),
			*ResponseData.grade, ResponseData.average_score, ResponseData.top_percent);

		// GameState에 결과 저장 (자동으로 복제됨)
		if (auto GS = Cast<ALingoGameState>(GetWorld()->GetGameState()))
		{
			GS->ReadResult = ResponseData;
		}

		this->InitReadResult(ResponseData);
	}
	else
	{
		PRINTLOG(TEXT("[Result] Quest result Failed"));
	}
}

void UPopup_Result::OnResponseListenResult(FResponseListenResult& ResponseData, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		// GameState에 결과 저장 (자동으로 복제됨)
		if (auto GS = Cast<ALingoGameState>(GetWorld()->GetGameState()))
		{
			GS->ListenResult = ResponseData;
		}

		this->InitListenResult(ResponseData);
	}
	else
	{
		PRINTLOG(TEXT("[Result] Quest result Failed"));
	}
}