// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file ANetworkTesterActor.cpp
 * @brief ANetworkTesterActor의 동작을 구현합니다.
 */
#include "ANetworkTesterActor.h"
#include "UHttpNetworkSystem.h"
#include "UKLingoNetworkSystem.h"
#include "GameLogging.h"
#include "NetworkData.h"
#include "UDialogManager.h"
#include "Engine/Engine.h"

ANetworkTesterActor::ANetworkTesterActor()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ANetworkTesterActor::SendToastMessage()
{
    UDialogManager::Get(GetWorld())->ShowToast(TEXT("토스트 메세지 샘플"));
}

void ANetworkTesterActor::RequestHealth()
{
    if ( auto ReqNetwork = UHttpNetworkSystem::Get(GetWorld()) )
    {
        ReqNetwork->RequestHealth( FResponseHealthDelegate::CreateUObject( this, &ANetworkTesterActor::OnResponseHealth));
    }
    else
    {
        PRINTLOG( TEXT("UNetworkSystem not found!"));
    }
}

void ANetworkTesterActor::OnResponseHealth(FResponseHealth& ResponseData, bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        ResponseData.PrintData();
    }
    else
    {
        PRINTLOG( TEXT("--- Network Response Received (FAIL) ---"));
    }
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