// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "UQuestInfoWidget.h"
#include "ALingoGameState.h"
#include "ALingoPlayerState.h"
#include "UBroadcastManager.h"
#include "NetworkData.h"
#include "ULingoGameHelper.h"
#include "Components/TextBlock.h"



void UQuestInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (auto BM = UBroadcastManager::Get(GetWorld()))
		BM->OnUpdateQuestRole.AddDynamic(this, &UQuestInfoWidget::InitQuestInfo);

	if (auto GS = ULingoGameHelper::GetLingoGameState(GetWorld()))
	{
		QuestScenarioDataDelegateHandle = GS->OnQuestScenarioDataUpdated.AddUObject(this, &UQuestInfoWidget::OnQuestScenarioDataReceived);
		ReadResultDelegateHandle = GS->OnReadResultUpdated.AddUObject(this, &UQuestInfoWidget::OnReadResultReceived);
		ListenResultDelegateHandle = GS->OnListenResultUpdated.AddUObject(this, &UQuestInfoWidget::OnListenResultReceived);
	}
}

void UQuestInfoWidget::NativeDestruct()
{
	if (auto BM = UBroadcastManager::Get(GetWorld()))
		BM->OnUpdateQuestRole.RemoveDynamic(this, &UQuestInfoWidget::InitQuestInfo);

	if (auto GS = ULingoGameHelper::GetLingoGameState(GetWorld()))
	{
		if (QuestScenarioDataDelegateHandle.IsValid())
		{
			GS->OnQuestScenarioDataUpdated.Remove(QuestScenarioDataDelegateHandle);
			QuestScenarioDataDelegateHandle.Reset();
		}
		
		if (ReadResultDelegateHandle.IsValid())
		{
			GS->OnReadResultUpdated.Remove(ReadResultDelegateHandle);
			ReadResultDelegateHandle.Reset();
		}

		if (ListenResultDelegateHandle.IsValid())
		{
			GS->OnListenResultUpdated.Remove(ListenResultDelegateHandle);
			ListenResultDelegateHandle.Reset();
		}
	}

	Super::NativeDestruct();
}

void UQuestInfoWidget::InitQuestInfo(EQuestRole InQuestRole)
{
	const auto GS = ULingoGameHelper::GetLingoGameState(GetWorld());
	if (!GS || !GS->IsQuestIng())
	{
		SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	const EQuestType QuestType = GS->GetCurrentQuestType();
	this->QuestRole = InQuestRole;

	SetVisibility(ESlateVisibility::Visible);

	if (QuestType == EQuestType::Read)
	{
		const FResponseReadScenario& ReadScenario = GS->GetReadScenarioData();

		if (QuestRole == EQuestRole::Both)
			SetQuestText(ReadScenario.full_data);
		else if (QuestRole == EQuestRole::OnlyQuestion1)
			SetQuestText(ReadScenario.word_data1);
		else if (QuestRole == EQuestRole::OnlyQuestion2)
			SetQuestText(ReadScenario.word_data2);
	}
	else if (QuestType == EQuestType::Listen)
	{
		const FResponseListenScenario& ListenScenario = GS->GetListenScenarioData();

		if (QuestRole == EQuestRole::Both)
			SetQuestText(ListenScenario.full_data);
		else if (QuestRole == EQuestRole::OnlyQuestion1)
			SetQuestText(ListenScenario.word_data1);
		else if (QuestRole == EQuestRole::OnlyQuestion2)
			SetQuestText(ListenScenario.word_data2);
	}
}

void UQuestInfoWidget::SetQuestText(const FWordData& WordData) const
{
	if (Txt_Message)
		Txt_Message->SetText(FText::FromString(WordData.Kor));
}

void UQuestInfoWidget::OnQuestScenarioDataReceived()
{
	this->InitQuestInfo(QuestRole);
}

void UQuestInfoWidget::OnReadResultReceived(const FResponseReadResult& /*ResponseData*/)
{
	SetVisibility(ESlateVisibility::Collapsed);
}

void UQuestInfoWidget::OnListenResultReceived(const FResponseListenResult& /*ResponseData*/)
{
	SetVisibility(ESlateVisibility::Collapsed);
}