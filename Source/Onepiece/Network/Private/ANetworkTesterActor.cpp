// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file ANetworkTesterActor.cpp
 * @brief ANetworkTesterActor의 동작을 구현합니다.
 */
#include "ANetworkTesterActor.h"
#include "UKLingoNetworkSystem.h"
#include "GameLogging.h"
#include "NetworkData.h"
#include "UDialogManager.h"
#include "UPopupManager.h"
#include "UPopup_Interview.h"
#include "UPopup_MsgBox.h"
#include "UVoiceConversationSystem.h"
#include "Engine/Engine.h"

ANetworkTesterActor::ANetworkTesterActor()
{
    PrimaryActorTick.bCanEverTick = false;

    VoiceConversationSystem = CreateDefaultSubobject<UVoiceConversationSystem>(TEXT("VoiceConversationSystem"));
}

// =============================================================================
// User API Tests
// =============================================================================

void ANetworkTesterActor::RequestUserRegister()
{
    if (auto KLingoNetwork = UKLingoNetworkSystem::Get(GetWorld()))
    {
        PRINTLOG(TEXT("[TEST] RequestUserRegister - UserName: %s"), *UserName);
        KLingoNetwork->RequestUserRegister(
            UserName,
            FResponseUserRegisterDelegate::CreateUObject(this, &ANetworkTesterActor::OnResponseUserRegister)
        );
    }
    else
    {
        PRINTLOG(TEXT("UKLingoNetworkSystem not found!"));
    }
}

void ANetworkTesterActor::RequestUserToken()
{
    if (auto KLingoNetwork = UKLingoNetworkSystem::Get(GetWorld()))
    {
        PRINTLOG(TEXT("[TEST] RequestUserToken - UserName: %s"), *UserName);
        KLingoNetwork->RequestUserToken(
            UserName,
            FResponseUserTokenDelegate::CreateUObject(this, &ANetworkTesterActor::OnResponseUserToken)
        );
    }
    else
    {
        PRINTLOG(TEXT("UKLingoNetworkSystem not found!"));
    }
}

void ANetworkTesterActor::RequestUserMe()
{
    if (auto KLingoNetwork = UKLingoNetworkSystem::Get(GetWorld()))
    {
        PRINTLOG(TEXT("[TEST] RequestUserMe"));
        KLingoNetwork->RequestUserMe(
            FResponseUserMeDelegate::CreateUObject(this, &ANetworkTesterActor::OnResponseUserMe)
        );
    }
    else
    {
        PRINTLOG(TEXT("UKLingoNetworkSystem not found!"));
    }
}

void ANetworkTesterActor::RequestUserHost()
{
    if (auto KLingoNetwork = UKLingoNetworkSystem::Get(GetWorld()))
    {
        KLingoNetwork->RequestUserHost(
            FResponseUserHostDelegate::CreateUObject(this, &ANetworkTesterActor::OnResponseUserHost)
        );
    }
    else
    {
        PRINTLOG(TEXT("UKLingoNetworkSystem not found!"));
    }
}

void ANetworkTesterActor::OnResponseUserRegister(FResponseUserRegister& ResponseData, bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        PRINTLOG(TEXT("--- User Register SUCCESS ---"));
        ResponseData.PrintData();
    }
    else
    {
        PRINTLOG(TEXT("--- User Register FAILED ---"));
    }
}

void ANetworkTesterActor::OnResponseUserToken(FResponseUserToken& ResponseData, bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        PRINTLOG(TEXT("--- User Token SUCCESS ---"));
        ResponseData.PrintData();
        PRINTLOG(TEXT("Token: %s"), *ResponseData.access_token);
    }
    else
    {
        PRINTLOG(TEXT("--- User Token FAILED ---"));
    }
}

void ANetworkTesterActor::OnResponseUserMe(FResponseUserMe& ResponseData, bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        PRINTLOG(TEXT("--- User Me SUCCESS ---"));
        ResponseData.PrintData();
    }
    else
    {
        PRINTLOG(TEXT("--- User Me FAILED ---"));
    }
}

void ANetworkTesterActor::OnResponseUserHost(FResponseUserHost& ResponseData, bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        PRINTLOG(TEXT("--- User Me SUCCESS ---"));
        ResponseData.PrintData();
    }
    else
    {
        PRINTLOG(TEXT("--- User Me FAILED ---"));
    }
}



// =============================================================================
// New API Tests
// =============================================================================

void ANetworkTesterActor::RequestScenario()
{
    if (auto KLingoNetwork = UKLingoNetworkSystem::Get(GetWorld()))
    {
        PRINTLOG(TEXT("[TEST] RequestScenario - Index: %d, Difficulty: %d, Lang: %d"), 
                 ScenarioIndex, ScenarioDifficulty, ScenarioLevel);
        KLingoNetwork->RequestScenario(
            ScenarioIndex,
            ScenarioDifficulty,
            ScenarioLevel,
            FResponseScenarioDelegate::CreateUObject(this, &ANetworkTesterActor::OnResponseScenario)
        );
    }
    else
    {
        PRINTLOG(TEXT("UKLingoNetworkSystem not found!"));
    }
}

void ANetworkTesterActor::RequestOcrExtract()
{
    if (auto KLingoNetwork = UKLingoNetworkSystem::Get(GetWorld()))
    {
        PRINTLOG(TEXT("[TEST] RequestOcrExtract - ImagePath: %s"), *OcrImagePath);
        KLingoNetwork->RequestOcrExtract(
            OcrImagePath,
            FResponseOcrExtractDelegate::CreateUObject(this, &ANetworkTesterActor::OnResponseOcrExtract)
        );
    }
    else
    {
        PRINTLOG(TEXT("UKLingoNetworkSystem not found!"));
    }
}

void ANetworkTesterActor::RequestSpeakingQuestions()
{
    if (auto KLingoNetwork = UKLingoNetworkSystem::Get(GetWorld()))
    {
        PRINTLOG(TEXT("[TEST] RequestSpeakingQuestions - Question :%s, AudioPath: %s"), *SpeakingQuestion, *SpeakingAudioPath);
        KLingoNetwork->RequestSpeakingJudges(
            SpeakingQuestion, SpeakingAudioPath,
            FResponseSpeakingJudesDelegate::CreateUObject(this, &ANetworkTesterActor::OnResponseSpeakingJudes)
        );
    }
    else
    {
        PRINTLOG(TEXT("UKLingoNetworkSystem not found!"));
    }
}

void ANetworkTesterActor::RequestListenAudio()
{
    if (auto KLingoNetwork = UKLingoNetworkSystem::Get(GetWorld()))
    {
        PRINTLOG(TEXT("[TEST] RequestListenAudio - AudioText: %s"), *AudioText);
        KLingoNetwork->RequestListenAudio(
            AudioText,
            FResponseListenAudioDelegate::CreateUObject(this, &ANetworkTesterActor::OnResponseListenAudio)
        );
    }
    else
    {
        PRINTLOG(TEXT("UKLingoNetworkSystem not found!"));
    }
}

void ANetworkTesterActor::RequestInterviewHello()
{
    if (auto KLingoNetwork = UKLingoNetworkSystem::Get(GetWorld()))
    {
        PRINTLOG(TEXT("[TEST] RequestInterviewHello"));
        KLingoNetwork->RequestInterviewHello(
            FResponseInterviewHelloDelegate::CreateUObject(this, &ANetworkTesterActor::OnResponseInterviewHello)
        );
    }
    else
    {
        PRINTLOG(TEXT("UKLingoNetworkSystem not found!"));
    }
}

void ANetworkTesterActor::OnResponseScenario(FResponseScenario& ResponseData, bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        PRINTLOG(TEXT("--- Scenario SUCCESS ---"));
        ResponseData.PrintData();
        PRINTLOG(TEXT("Index: %d, Difficulty: %d"), ResponseData.index, ResponseData.dificulity);
        PRINTLOG(TEXT("Correct Answer Index: %d"), ResponseData.correct_answer_index);
        PRINTLOG(TEXT("Target Data Count: %d"), ResponseData.target_data.Num());
        
        for (int32 i = 0; i < ResponseData.target_data.Num(); i++)
        {
            PRINTLOG(TEXT("  [%d] Word1: %s (Code: %s), Word2: %s (Code: %s)"),
                     i,
                     *ResponseData.target_data[i].word1.name,
                     *ResponseData.target_data[i].word1.code,
                     *ResponseData.target_data[i].word2.name,
                     *ResponseData.target_data[i].word2.code);
        }
        
        PRINTLOG(TEXT("Word Data 1 - Kor: %s, Eng: %s, Pronunciation: %s"), 
                 *ResponseData.word_data1.Kor, *ResponseData.word_data1.Eng, *ResponseData.word_data1.Pronunciation);
        PRINTLOG(TEXT("Word Data 2 - Kor: %s, Eng: %s, Pronunciation: %s"), 
                 *ResponseData.word_data2.Kor, *ResponseData.word_data2.Eng, *ResponseData.word_data2.Pronunciation);
        PRINTLOG(TEXT("Full Data - Kor: %s, Eng: %s, Pronunciation: %s"),
                 *ResponseData.full_data.Kor, *ResponseData.full_data.Eng, *ResponseData.full_data.Pronunciation);
    }
    else
    {
        PRINTLOG(TEXT("--- Scenario FAILED ---"));
    }
}

void ANetworkTesterActor::OnResponseOcrExtract(FResponseOcrExtract& ResponseData, bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        PRINTLOG(TEXT("--- OCR Extract SUCCESS ---"));
        ResponseData.PrintData();
        PRINTLOG(TEXT("Success: %s"), ResponseData.success ? TEXT("true") : TEXT("false"));
        PRINTLOG(TEXT("Extracted Text: %s"), *ResponseData.extracted_text);
    }
    else
    {
        PRINTLOG(TEXT("--- OCR Extract FAILED ---"));
    }
}

void ANetworkTesterActor::OnResponseSpeakingJudes(FResponseSpeakingJudes& ResponseData, bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        PRINTLOG(TEXT("--- Speaking Questions SUCCESS ---"));
        ResponseData.PrintData();
        UDialogManager::Get(GetWorld())->ShowToast(*ResponseData.final_feedback);
    }
    else
    {
        PRINTLOG(TEXT("--- Speaking Questions FAILED ---"));
    }
}

void ANetworkTesterActor::OnResponseListenAudio(FResponseListenAudio& ResponseData, bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        PRINTLOG(TEXT("--- Listen Audio SUCCESS ---"));
        ResponseData.PrintData();

        UDialogManager::Get(GetWorld())->ShowToast(*ResponseData.audio_text);
        VoiceConversationSystem->PlayVoiceAudio(ResponseData.audio_base64);
    }
    else
    {
        PRINTLOG(TEXT("--- Listen Audio FAILED ---"));
    }
}


void ANetworkTesterActor::OnResponseInterviewHello(FResponseInterviewHello& ResponseData, bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        PRINTLOG(TEXT("--- InterViewHello Questions SUCCESS ---"));
        ResponseData.PrintData();

        if (const auto PopupMgr = UPopupManager::Get(GetWorld()))
        {
            if (const auto Popup = Cast<UPopup_Interview>(PopupMgr->ShowPopup(EPopupType::Interview)))
            {
                Popup->InitPopup(ResponseData);
            }
        }
    }
    else
    {
        PRINTLOG(TEXT("--- InterViewHello Questions FAILED ---"));
    }
}
