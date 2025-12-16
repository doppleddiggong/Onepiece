// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file ANetworkTesterActor.cpp
 * @brief ANetworkTesterActor의 동작을 구현합니다.
 */
#include "ANetworkTesterActor.h"

#include "APlayerControl.h"
#include "UKLingoNetworkSystem.h"
#include "GameLogging.h"
#include "NetworkData.h"
#include "UDialogManager.h"
#include "ULingoGameHelper.h"
#include "ULingoGameInstanceSubsystem.h"
#include "UPopupManager.h"
#include "UPopup_Interview.h"
#include "UPopup_InterviewHello.h"
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
        KLingoNetwork->RequestUserMe( FResponseUserMeDelegate::CreateUObject(this, &ANetworkTesterActor::OnResponseUserMe) );
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
        RequestUserMe();
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

        if ( auto PC = ULingoGameHelper::GetPlayerControl(GetWorld()) )
            PC->Server_SetUserInfo(ResponseData);
    }
    else
    {
        PRINTLOG(TEXT("--- User Me FAILED ---"));
    }
}

// =============================================================================
// New API Tests
// =============================================================================
void ANetworkTesterActor::RequestOcrExtract()
{
    if (auto KLingoNetwork = UKLingoNetworkSystem::Get(GetWorld()))
    {
        PRINTLOG(TEXT("[TEST] RequestOcrExtract - ImagePath: %s"), *OcrImagePath);
        // KLingoNetwork->RequestOcrExtract(
        //     OcrImagePath,
        //     FResponseOcrExtractDelegate::CreateUObject(this, &ANetworkTesterActor::OnResponseOcrExtract)
        // );
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

void ANetworkTesterActor::OnResponseOcrExtract(FResponseWriteSubmit& ResponseData, bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        PRINTLOG(TEXT("--- OCR Extract SUCCESS ---"));
        ResponseData.PrintData();
        // PRINTLOG(TEXT("Success: %s"), ResponseData.success ? TEXT("true") : TEXT("false"));
        // PRINTLOG(TEXT("Extracted Text: %s"), *ResponseData.extracted_text);
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

        if (auto Popup = UPopupManager::ShowPopupAs<UPopup_InterviewHello>(GetWorld(), EPopupType::InterviewHello))
        {
            Popup->InitPopup(ResponseData);
        }
    }
    else
    {
        PRINTLOG(TEXT("--- InterViewHello Questions FAILED ---"));
    }
}
