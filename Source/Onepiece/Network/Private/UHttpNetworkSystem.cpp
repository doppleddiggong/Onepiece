// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file UHttpNetworkSystem.cpp
 * @brief UHttpNetworkSystem의 동작을 구현합니다.
 */

#include "UHttpNetworkSystem.h"
#include "NetworkLog.h"
#include "ENetworkLogType.h"
#include "HttpModule.h"
#include "FHttpMultipartFormData.h"
#include "GameLogging.h"
#include "JsonObjectConverter.h"
#include "UBroadcastManager.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

#define NETWORK_GET     TEXT("GET")
#define NETWORK_POST    TEXT("POST")

void UHttpNetworkSystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
}

void UHttpNetworkSystem::Deinitialize()
{
    Super::Deinitialize();
}

void UHttpNetworkSystem::LogNetwork( ENetworkLogType InLogType, const FString& Message, const FString& Body)
{
    if (!Body.IsEmpty())
    {
        NETWORK_LOG(TEXT("%s %s | Body: %s"), GetLogPrefix(InLogType), *Message, *Body);
    }
    else
    {
        NETWORK_LOG(TEXT("%s %s"), GetLogPrefix(InLogType), *Message);
    }
}

const TCHAR* UHttpNetworkSystem::GetLogPrefix(ENetworkLogType InLogType)
{
    switch (InLogType)
    {
    case ENetworkLogType::Get:       return TEXT("[GET]");
    case ENetworkLogType::Post:      return TEXT("[POST]");
    case ENetworkLogType::WS:        return TEXT("[WS]");
    default:                         return TEXT("[UNKNOWN]");
    }
}

void UHttpNetworkSystem::AddNetworkWaitCount(int InValue)
{
    NetworkWaitCount += InValue;

    UWorld* World = GetWorld();
    if (!IsValid(World))
    {
        PRINTLOG( TEXT("[Network] Invalid World in AddNetworkWaitCount."));
        return;
    }

    if ( auto BroadcastManager = UBroadcastManager::Get(World) )
        BroadcastManager->SendNetworkWaitCount(NetworkWaitCount);
}

void UHttpNetworkSystem::RequestHealth( FResponseHealthDelegate InDelegate )
{
    auto HttpRequest = FHttpModule::Get().CreateRequest();

    HttpRequest->SetVerb(NETWORK_GET);
    HttpRequest->SetURL(NetworkConfig::GetFullUrl(RequestAPI::Health));
    HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

    LogNetwork(ENetworkLogType::Get, *HttpRequest->GetURL());

    HttpRequest->OnProcessRequestComplete().BindLambda(
        [WeakThis = TWeakObjectPtr<UHttpNetworkSystem>(this), InDelegate](FHttpRequestPtr Req, FHttpResponsePtr ResPtr, bool bWasSuccessful)
        {
            if (!WeakThis.IsValid() || IsEngineExitRequested())
                return;
            
            WeakThis->AddNetworkWaitCount(-1);
            FResponseHealth ResponseData;
            ResponseData.SetFromHttpResponse(ResPtr);

            InDelegate.ExecuteIfBound(ResponseData, bWasSuccessful);
        });

    AddNetworkWaitCount(1);
    HttpRequest->ProcessRequest();
}


void UHttpNetworkSystem::RequestASK(const FString& FilePath, const FGPTContext& Context, FResponseAskDelegate InDelegate)
{
    auto HttpRequest = FHttpModule::Get().CreateRequest();
    HttpRequest->SetVerb(NETWORK_POST);
    HttpRequest->SetURL(NetworkConfig::GetFullUrl(RequestAPI::ASK));
    HttpRequest->SetHeader(TEXT("Accept"), TEXT("application/json"));

    FHttpMultipartFormData Form;
    if (!Form.AddFile(TEXT("file"), FilePath))
    {
        NETWORK_LOG(TEXT("Ask: file load failed: %s"), *FilePath);
        return;
    }
    Form.SetupHttpRequest(HttpRequest);

    FRequestASK RequestData;
    RequestData.context = Context;

    FString RequestBody;
    if (!RequestData.ToJsonString(RequestBody))
    {
        NETWORK_LOG(TEXT("Failed to serialize FRequestASK to JSON"));
        return;
    }

    Form.AddStringField(TEXT("context"), RequestBody);
    Form.SetupHttpRequest(HttpRequest);

    const FString LogBody = FString::Printf(TEXT("file: %s, context: %s"), *FilePath, *RequestBody);
    LogNetwork(ENetworkLogType::Post, *HttpRequest->GetURL(), LogBody);
    
    HttpRequest->OnProcessRequestComplete().BindLambda(
        [WeakThis = TWeakObjectPtr<UHttpNetworkSystem>(this), InDelegate](FHttpRequestPtr Req, FHttpResponsePtr ResPtr, bool bWasSuccessful)
        {
            if (!WeakThis.IsValid() || IsEngineExitRequested())
                return;
            
            WeakThis->AddNetworkWaitCount(-1);
            FResponseAsk ResponseData;
            if (bWasSuccessful && ResPtr.IsValid())
            {
                NETWORK_LOG(TEXT("[RES] Ask completed: transcribed_text, gpt_response_text, audio_content"));
                ResponseData.SetFromHttpResponse(ResPtr);
            }
            InDelegate.ExecuteIfBound(ResponseData, bWasSuccessful);
        });

    AddNetworkWaitCount(1);
    HttpRequest->ProcessRequest();
}