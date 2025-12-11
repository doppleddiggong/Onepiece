// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "UPopup_ReadQuest.h"
#include "ALingoGameState.h"
#include "ALingoPlayerState.h"
#include "APlayerActor.h"
#include "UBroadcastManager.h"
#include "UImageButton.h"
#include "UKLingoNetworkSystem.h"
#include "ULingoGameHelper.h"
#include "UPopupManager.h"
#include "UPopup_Word.h"
#include "UWordWidget.h"
#include "Components/RichTextBlock.h"
#include "HyperLinkPluginBPLibrary.h"
#include "UWordItem.h"
#include "Components/HorizontalBox.h"
#include "Components/Spacer.h"
#include "Decoders/VorbisAudioInfo.h"

#include "GameFramework/PlayerController.h"


class USpacer;

void UPopup_ReadQuest::NativeConstruct()
{
	Super::NativeConstruct();

	// URichText 기능 이식: HyperLink 클릭 핸들러 등록
	if (Rich_Text)
	{
		if (UHyperLinkPluginBPLibrary* LinkDecorator = Cast<UHyperLinkPluginBPLibrary>(
			Rich_Text->GetDecoratorByClass(UHyperLinkPluginBPLibrary::StaticClass())))
		{
			LinkDecorator->SetNativeClickHandler(FOnClickLink::CreateUObject(this, &UPopup_ReadQuest::OnClickHyperlink));
		}
	}
}

void UPopup_ReadQuest::InitRead(const FResponseReadScenario& InScenarioData)
{
	if (Btn_Confirm)
	{
		Btn_Confirm->OnButtonClickedEvent.RemoveDynamic(this, &UPopup_ReadQuest::OnClickClose);
		Btn_Confirm->OnButtonClickedEvent.AddDynamic(this, &UPopup_ReadQuest::OnClickClose);
	}

	if (auto BM = UBroadcastManager::Get(GetWorld()))
	{
		BM->OnUpdateQuestRole.RemoveDynamic(this, &UPopup_ReadQuest::InitQuestInfo);
		BM->OnUpdateQuestRole.AddDynamic(this, &UPopup_ReadQuest::InitQuestInfo);
	}

	this->QuestType = EQuestType::Read;
	this->ReadData = InScenarioData;

	if ( const auto PS= ULingoGameHelper::GetLingoPlayerState(GetWorld()) )
		InitQuestInfo(PS->QuestRole);
}

FString UPopup_ReadQuest::ConvertWordDataToRichText(const FWordData& WordData)
{
	CachedPhonemeData = WordData.GetPhonemeData();

	FString TextString;
	for (int32 i = 0; i < CachedPhonemeData.Num(); ++i)
	{
		const FPhonemeData& Phoneme = CachedPhonemeData[i];
		TextString += Phoneme.ToRichTextString(i);
	}
	return TextString;
};

void UPopup_ReadQuest::InitQuestInfo(EQuestRole QuestRole)
{
	if ( QuestType == EQuestType::Read )
	{
		if ( QuestRole == EQuestRole::Both )
		{
			const FString RichTextString = ConvertWordDataToRichText(ReadData.full_data);
			
			Rich_Text->SetText(FText::FromString(RichTextString));
			Txt_SubTitle->SetText(FText::FromString(ReadData.full_data.Eng));
		}
		else if ( QuestRole == EQuestRole::OnlyQuestion1 )
		{
			const FString RichTextString = ConvertWordDataToRichText(ReadData.word_data1);
			
			Rich_Text->SetText(FText::FromString(RichTextString));
			Txt_SubTitle->SetText(FText::FromString(ReadData.word_data1.Eng));
		}
		else if ( QuestRole == EQuestRole::OnlyQuestion2 )
		{
			const FString RichTextString = ConvertWordDataToRichText(ReadData.word_data2);
			
			Rich_Text->SetText(FText::FromString(RichTextString));
			Txt_SubTitle->SetText(FText::FromString(ReadData.word_data2.Eng));
		}
	}

	InitWordList(QuestRole);
}

void UPopup_ReadQuest::InitWordList(EQuestRole QuestRole)
{
	WordBox->ClearChildren();

	if (WordItemClass == nullptr)
		return;

	auto AnswerData = ReadData.GetCorrectAnswerData();
	
	if ( QuestRole == EQuestRole::Both )
	{
		if ( auto WordItem = CreateWidget<UWordItem>(this, WordItemClass) )
		{
			EWordType WordType = EWordType::Animal;
			const int32 WordCode = FCString::Atoi(*AnswerData.word1.code);
			WordItem->InitInfo(WordType, WordCode);

			WordBox->AddChild(WordItem);
		}

		{
			USpacer* Spacer = NewObject<USpacer>(WordBox);
			Spacer->SetSize(FVector2D(10.f, 1.f));
			WordBox->AddChild(Spacer);
		}
		
		if ( auto WordItem = CreateWidget<UWordItem>(this, WordItemClass) )
		{
			EWordType WordType = EWordType::Color;
			const int32 WordCode = FCString::Atoi(*AnswerData.word2.code);
			WordItem->InitInfo(WordType, WordCode);

			WordBox->AddChild(WordItem);
		}
	}
	else if ( QuestRole == EQuestRole::OnlyQuestion1  )
	{
		if ( auto WordItem = CreateWidget<UWordItem>(this, WordItemClass) )
		{
			EWordType WordType = EWordType::Animal;
			const int32 WordCode = FCString::Atoi(*AnswerData.word1.code);
			WordItem->InitInfo(WordType, WordCode);

			WordBox->AddChild(WordItem);
		}
	}
	else if ( QuestRole == EQuestRole::OnlyQuestion2 )
	{
		if ( auto WordItem = CreateWidget<UWordItem>(this, WordItemClass) )
		{
			EWordType WordType = EWordType::Color;
			const int32 WordCode = FCString::Atoi(*AnswerData.word2.code);
			WordItem->InitInfo(WordType, WordCode);

			WordBox->AddChild(WordItem);
		}
	}
}


void UPopup_ReadQuest::OnClickClose()
{
	if (UPopupManager* PopupMgr = UPopupManager::Get(GetWorld()))
	{
		PopupMgr->HideCurrentPopup();
	}
}

void UPopup_ReadQuest::RequestListenAudio(const FString& AudioText)
{
	if (bIsRequest)
		return;
	
	if (auto KLingoNetwork = UKLingoNetworkSystem::Get(GetWorld()))
	{
		bIsRequest = true;
		
		KLingoNetwork->RequestListenAudio(
			AudioText,
			FResponseListenAudioDelegate::CreateUObject(this, &UPopup_ReadQuest::OnResponseListenAudio)
		);
	}
}

void UPopup_ReadQuest::OnResponseListenAudio(FResponseListenAudio& ResponseData, bool bWasSuccessful)
{
	bIsRequest = false;
	
	if (bWasSuccessful)
	{
		if (auto PlayerActor = ULingoGameHelper::GetPlayerActor(this))
			PlayerActor->PlayTTSAudio(ResponseData.audio_base64);
	}
}

void UPopup_ReadQuest::OnClickHyperlink(const FString& LinkID, const FString& Content)
{
	// URichText 기능 이식: LinkID를 인덱스로 변환하여 캐싱된 음소 데이터 가져오기
	const int32 Index = FCString::Atoi(*LinkID);

	if (CachedPhonemeData.IsValidIndex(Index))
	{
		if (auto Popup = UPopupManager::ShowPopupAs<UPopup_Word>(GetWorld(), EPopupType::Word))
		{
			Popup->InitPopup(CachedPhonemeData[Index]);
		}
	}
}