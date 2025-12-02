// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "Popup_Result.h"

#include "ALingoGameState.h"
#include "ScoreManager.h"
#include "UImageButton.h"
#include "UPopupManager.h"
#include "UPopup_ReadQuest.h"
#include "UTextureButton.h"
#include "UWordWidget.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"

void UPopup_Result::NativeConstruct()
{
	Super::NativeConstruct();
}

void UPopup_Result::InitPopup()
{
	// 중복 바인딩 방지: 기존 바인딩 제거 후 재바인딩
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

	SetWordWidget();
	SetWrongList();
	SetTimeTaken();
}

void UPopup_Result::OnClickClose()
{
	// PopupManager를 통해 팝업 닫기 (마우스 커서 처리 포함)
	if (UPopupManager* PopupMgr = UPopupManager::Get(GetWorld()))
	{
		PopupMgr->HideCurrentPopup();
	}
}

void UPopup_Result::SetWordWidget()
{
	if (WordWidget)
	{
		ALingoGameState* GS = Cast<ALingoGameState>(GetWorld()->GetGameState());
		if (!GS) return;
		
		WordWidget->InitWordData(GS->CurScenarioData.full_data);
	}
}

void UPopup_Result::SetWrongList()
{
	if (!Scrl_WrongList) return;

	ALingoGameState* GS = Cast<ALingoGameState>(GetWorld()->GetGameState());
	if (!GS) return;
	// 틀린 인덱스 리스트
	TArray<int32> WrongList = GS->WrongLuggageList;
	// 전체 캐리어 정보
	const TArray<FScenarioTargetData>& ScenarioData = GS->GetScenarioData().target_data;
	
	// 기존 항목 제거
	Scrl_WrongList->ClearChildren();

	// WrongList의 각 항목을 텍스트로 추가
	for (int32 i=0; i<WrongList.Num(); i++)
	{
		int32 WrongIndex = WrongList[i];
		auto SD = ScenarioData[WrongIndex];

		if (UTextBlock* TextBlock = NewObject<UTextBlock>(this))
		{
			// 맨 마지막 인덱스는 정답
			if (i == WrongList.Num()-1)
			{
				TextBlock->SetText(FText::FromString(FString::Printf(TEXT("[정답] %s, %s"),
								*SD.word1.name, *SD.word2.name)));
				TextBlock->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
				
				Scrl_WrongList->AddChild(TextBlock);
				
				break;
			}
			
			TextBlock->SetText(FText::FromString(FString::Printf(TEXT("Try %d - [오답] %s, %s"),
				i+1, *SD.word1.name, *SD.word2.name)));
			TextBlock->SetColorAndOpacity(FSlateColor(FLinearColor::Black));
			
			Scrl_WrongList->AddChild(TextBlock);
		}
	}
}

void UPopup_Result::SetTimeRank()
{
	FString Rank = "";
	if (UScoreManager* ScoreMgr = UScoreManager::Get(GetWorld()))
	{
		ScoreMgr->GetTimeRank(TimeTaken, Rank);

		Txt_TimeRank->SetText(FText::FromString(Rank));

		SetAccuracy();
	}
}

void UPopup_Result::SetTimeTaken()
{
	ALingoGameState* GS = Cast<ALingoGameState>(GetWorld()->GetGameState());
	if (GS)
	{
		float TimeRemain = GS->GetRemainMissionTime();
		TimeTaken = 300 - TimeRemain;

		int32 Minutes = FMath::FloorToInt(TimeTaken / 60.f);
		int32 Seconds = FMath::FloorToInt(TimeTaken) % 60;

		FString Format = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);

		Txt_TimeTaken->SetText(FText::FromString(Format));

		SetTimeRank();
	}
}

void UPopup_Result::SetAccuracy()
{
	FString Accuracy = "";
	if (UScoreManager* ScoreMgr = UScoreManager::Get(GetWorld()))
	{
		ScoreMgr->GetAccuracyPercentage(Accuracy);

		Txt_Accuracy->SetText(FText::FromString(Accuracy));
	}
}
