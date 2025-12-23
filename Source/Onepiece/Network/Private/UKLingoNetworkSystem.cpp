// Copyright (c) 2025 Doppleddiggong. All rights reserved.
// Unauthorized copying, modification, or distribution of this file,
// via any medium is strictly prohibited. Proprietary and confidential.

/// @file UKLingoNetworkSystem.cpp
/// @brief KLingo API 요청 서브시스템의 구현을 제공합니다.

#include "UKLingoNetworkSystem.h"

#include <string>

#include "ALingoPlayerState.h"
#include "APlayerControl.h"
#include "HttpModule.h"
#include "NetworkLog.h"
#include "NetworkData.h"
#include "FHttpMultipartFormData.h"
#include "GameLogging.h"
#include "UBroadcastManager.h"
#include "ULingoGameHelper.h"
#include "UPopupManager.h"
#include "UPopup_MsgBox.h"
#include "UAudioCacheManager.h"
#include "ULingoGameInstanceSubsystem.h"
#include "Misc/Paths.h"
#include "Dom/JsonObject.h"
#include "GenericPlatform/GenericPlatformHttp.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

#define NETWORK_GET     TEXT("GET")
#define NETWORK_POST    TEXT("POST")

void UKLingoNetworkSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	NetworkWaitCount = 0;
}

void UKLingoNetworkSystem::Deinitialize()
{
	Super::Deinitialize();
}

void UKLingoNetworkSystem::LogNetwork( ENetworkLogType InLogType, const FString& Message, const FString& Body)
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

const TCHAR* UKLingoNetworkSystem::GetLogPrefix(ENetworkLogType InLogType)
{
	switch (InLogType)
	{
	case ENetworkLogType::Get:       return TEXT("[GET]");
	case ENetworkLogType::Post:      return TEXT("[POST]");
	case ENetworkLogType::WS:        return TEXT("[WS]");
	default:                         return TEXT("[UNKNOWN]");
	}
}

bool UKLingoNetworkSystem::IsResSuccess(const int InCode)
{
	if ( InCode == 200 || InCode == 201 )
		return true;
	return false;
}


void UKLingoNetworkSystem::AddNetworkWaitCount(int Value)
{
	NetworkWaitCount += Value;

	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		PRINTLOG( TEXT("[Network] Invalid World in AddNetworkWaitCount."));
		return;
	}

	if ( auto BroadcastManager = UBroadcastManager::Get(World) )
		BroadcastManager->SendNetworkWaitCount(NetworkWaitCount);
}

void UKLingoNetworkSystem::ShowNetworkErrorPopup(int32 ResponseCode, const FString& ResponseContent)
{
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		NETWORK_LOG(TEXT("[Network] Invalid World in ShowNetworkErrorPopup"));
		return;
	}

	// JSON 응답 파싱
	FString ErrorDetail = TEXT("Network request failed");
	
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);
	
	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		// "detail" 필드 추출
		if (JsonObject->HasField(TEXT("detail")))
		{
			ErrorDetail = JsonObject->GetStringField(TEXT("detail"));
		}
	}
	else
	{
		// JSON 파싱 실패 시 원본 응답 사용
		if (!ResponseContent.IsEmpty())
		{
			ErrorDetail = ResponseContent;
		}
	}

	// 팝업 표시
	if (UPopupManager* PopupMgr = UPopupManager::Get(World))
	{
		FString Title = FString::Printf(TEXT("Error %d"), ResponseCode);
		PopupMgr->ShowMsgBoxSimple(Title, ErrorDetail, EMsgBoxType::OK);
		
		NETWORK_LOG(TEXT("[Network] Error Popup - Code: %d, Detail: %s"), ResponseCode, *ErrorDetail);
	}
}

TSharedRef<IHttpRequest, ESPMode::ThreadSafe> UKLingoNetworkSystem::SetupHttpRequest(
	const FString& Url,	const FString& Verb )
{
	auto RetRequest = FHttpModule::Get().CreateRequest();
	RetRequest->SetURL(Url);
	RetRequest->SetVerb(Verb);

	// 기본 헤더
	RetRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	RetRequest->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *access_token));

	return RetRequest;
}


// =================================================================================
// RequestLogin
// =================================================================================

void UKLingoNetworkSystem::RequestUserRegister(const FString& UserName, FResponseUserRegisterDelegate InDelegate)
{
	FString Url = NetworkConfig::GetFullUrl(RequestAPI::users_register);
	auto Request = SetupHttpRequest( Url, NETWORK_POST );
	
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField(TEXT("username"), UserName);
	JsonObject->SetStringField(TEXT("fullname"), UserName + TEXT("@klingo.com"));
	JsonObject->SetStringField(TEXT("password"), UserName);

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
	Request->SetContentAsString(RequestBody);

	LogNetwork(ENetworkLogType::Post, *Request->GetURL(), *RequestBody);
	
	Request->OnProcessRequestComplete().BindLambda(
		[this, InDelegate](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSuccess)
		{
			AddNetworkWaitCount(-1);

			FResponseUserRegister ResponseData;

			if (bSuccess && HttpResponse.IsValid())
			{
				const int32 ResponseCode = HttpResponse->GetResponseCode();

				if (IsResSuccess(ResponseCode))
				{
					ResponseData.SetFromHttpResponse(HttpResponse);
					ResponseData.PrintData();
					InDelegate.ExecuteIfBound(ResponseData, true);
				}
				else
				{
					NETWORK_LOG(TEXT("[POST] RequestUserRegister failed - Code: %d, Response: %s"),
						ResponseCode, *HttpResponse->GetContentAsString());
					ShowNetworkErrorPopup(ResponseCode, HttpResponse->GetContentAsString());
					InDelegate.ExecuteIfBound(ResponseData, false);
				}
			}
			else
			{
				NETWORK_LOG(TEXT("[POST] RequestUserRegister failed - bSuccess: %s, Response valid: %s"),
					bSuccess ? TEXT("true") : TEXT("false"),
					HttpResponse.IsValid() ? TEXT("true") : TEXT("false"));
				
				// HTTP 요청 실패 시 팝업 표시
				int32 ErrorCode = HttpResponse.IsValid() ? HttpResponse->GetResponseCode() : 0;
				FString ErrorContent = HttpResponse.IsValid() ? HttpResponse->GetContentAsString() : TEXT("Network connection failed");
				ShowNetworkErrorPopup(ErrorCode, ErrorContent);
				
				InDelegate.ExecuteIfBound(ResponseData, false);
			}
		});

	AddNetworkWaitCount(1);
	Request->ProcessRequest();
}



void UKLingoNetworkSystem::RequestUserToken(const FString& UserName, FResponseUserTokenDelegate InDelegate)
{
	FString Url = NetworkConfig::GetFullUrl(RequestAPI::users_token);
	auto Request = SetupHttpRequest( Url, NETWORK_POST );

	// OAuth2 password flow requires application/x-www-form-urlencoded
	FHttpMultipartFormData FormData(EFormDataType::FormUrlEncoded);
	FormData.AddText(TEXT("grant_type"), TEXT("password"));
	FormData.AddText(TEXT("username"), UserName);
	FormData.AddText(TEXT("password"), UserName);
	FormData.SetupHttpRequest(Request);

	LogNetwork(ENetworkLogType::Post, *Request->GetURL(), *UserName);
	
	Request->OnProcessRequestComplete().BindLambda(
		[this, InDelegate, UserName](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSuccess)
		{
			AddNetworkWaitCount(-1);

			FResponseUserToken ResponseData;

			if (bSuccess && HttpResponse.IsValid())
			{
				const int32 ResponseCode = HttpResponse->GetResponseCode();

				if (IsResSuccess(ResponseCode))
				{
					NETWORK_LOG(TEXT("[RES] RequestInterviewHello - Code: %d, Response: %s"), ResponseCode, *HttpResponse->GetContentAsString());

					ResponseData.SetFromHttpResponse(HttpResponse);
					ResponseData.PrintData();

					access_token = ResponseData.access_token;

					if (auto PS = ULingoGameHelper::GetLingoPlayerState(this))
						PS->SetToken(ResponseData.access_token);

					InDelegate.ExecuteIfBound(ResponseData, true);
				}
				else
				{
					NETWORK_LOG(TEXT("[POST] RequestUserToken failed - Code: %d, Response: %s"),
						ResponseCode, *HttpResponse->GetContentAsString());
					ShowNetworkErrorPopup(ResponseCode, HttpResponse->GetContentAsString());
					InDelegate.ExecuteIfBound(ResponseData, false);
				}
			}
			else
			{
				NETWORK_LOG(TEXT("[POST] RequestUserToken failed - bSuccess: %s, Response valid: %s"),
					bSuccess ? TEXT("true") : TEXT("false"),
					HttpResponse.IsValid() ? TEXT("true") : TEXT("false"));
				
				int32 ErrorCode = HttpResponse.IsValid() ? HttpResponse->GetResponseCode() : 0;
				FString ErrorContent = HttpResponse.IsValid() ? HttpResponse->GetContentAsString() : TEXT("Network connection failed");
				ShowNetworkErrorPopup(ErrorCode, ErrorContent);
				
				InDelegate.ExecuteIfBound(ResponseData, false);
			}
		});

	AddNetworkWaitCount(1);
	Request->ProcessRequest();
}


void UKLingoNetworkSystem::RequestUserMe( FResponseUserMeDelegate InDelegate)
{
	FString Url = NetworkConfig::GetFullUrl(RequestAPI::users_me);
	auto Request = SetupHttpRequest( Url, NETWORK_GET );
	
	LogNetwork(ENetworkLogType::Get, *Request->GetURL());
	
	Request->OnProcessRequestComplete().BindLambda(
		[this, InDelegate](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSuccess)
		{
			AddNetworkWaitCount(-1);

			FResponseUserMe ResponseData;

			if (bSuccess && HttpResponse.IsValid())
			{
				const int32 ResponseCode = HttpResponse->GetResponseCode();

				NETWORK_LOG(TEXT("[RES] Code: %d, Response: %s"), ResponseCode, *HttpResponse->GetContentAsString());

				if (IsResSuccess(ResponseCode))
				{
					ResponseData.SetFromHttpResponse(HttpResponse);
					ResponseData.PrintData();

					ULingoGameInstanceSubsystem::Get(GetWorld())->SetUserInfo(ResponseData);

					InDelegate.ExecuteIfBound(ResponseData, true);
				}
				else
				{
					ShowNetworkErrorPopup(ResponseCode, HttpResponse->GetContentAsString());
					InDelegate.ExecuteIfBound(ResponseData, false);
				}
			}
			else
			{
				NETWORK_LOG(TEXT("[GET] RequestUserMe failed - bSuccess: %s, Response valid: %s"),
					bSuccess ? TEXT("true") : TEXT("false"),
					HttpResponse.IsValid() ? TEXT("true") : TEXT("false"));
				
				int32 ErrorCode = HttpResponse.IsValid() ? HttpResponse->GetResponseCode() : 0;
				FString ErrorContent = HttpResponse.IsValid() ? HttpResponse->GetContentAsString() : TEXT("Network connection failed");
				ShowNetworkErrorPopup(ErrorCode, ErrorContent);
				
				InDelegate.ExecuteIfBound(ResponseData, false);
			}
		});

	AddNetworkWaitCount(1);
	Request->ProcessRequest();
}

// =================================================================================
// RequestWriteQuestions
// =================================================================================

void UKLingoNetworkSystem::RequestWriteQuestions(FResponseWriteQuestionDelegate InDelegate)
{
	// URL 형식: /scenario/stages/redis/{room_id}/{scenario_id}/{stage_type}/{level}
	FString Endpoint = FString::Printf(TEXT("%s/%lld/%d/%d/%d"), *RequestAPI::scenario,
		ULingoGameHelper::GetLingoGameState( GetWorld())->GetRoomId(),
		1,
		ULingoGameHelper::GetStageTypeIndex(EQuestType::Write),
		ULingoGameHelper::GetLingoGameState( GetWorld())->GetRoomLevel());
	FString Url = NetworkConfig::GetFullUrl(Endpoint);
	auto Request = SetupHttpRequest(Url, NETWORK_GET);

	LogNetwork(ENetworkLogType::Get, *Request->GetURL());

	Request->OnProcessRequestComplete().BindLambda(
		[WeakThis = TWeakObjectPtr<UKLingoNetworkSystem>(this), InDelegate](FHttpRequestPtr Req, FHttpResponsePtr ResPtr, bool bWasSuccessful)
		{
			if (!WeakThis.IsValid() || IsEngineExitRequested())
				return;

			WeakThis->AddNetworkWaitCount(-1);
			FQuestWriteInfo ResponseData;

			if (bWasSuccessful && ResPtr.IsValid())
			{
				const int32 ResponseCode = ResPtr->GetResponseCode();

				NETWORK_LOG(TEXT("[RES] RequestWriteQuestions - Code: %d, Response: %s"), ResponseCode, *ResPtr->GetContentAsString());
				
				if (IsResSuccess(ResponseCode))
				{
					ResponseData.SetFromHttpResponse(ResPtr);
					InDelegate.ExecuteIfBound(ResponseData, true);
				}
				else
				{
					WeakThis->ShowNetworkErrorPopup(ResponseCode, ResPtr->GetContentAsString());
					InDelegate.ExecuteIfBound(ResponseData, false);
				}
			}
			else
			{
				NETWORK_LOG(TEXT("[POST] RequestWriteQuestions failed - bSuccess: %s, Response valid: %s"),
					bWasSuccessful ? TEXT("true") : TEXT("false"),
					ResPtr.IsValid() ? TEXT("true") : TEXT("false"));
				
				int32 ErrorCode = ResPtr.IsValid() ? ResPtr->GetResponseCode() : 0;
				FString ErrorContent = ResPtr.IsValid() ? ResPtr->GetContentAsString() : TEXT("Network connection failed");
				WeakThis->ShowNetworkErrorPopup(ErrorCode, ErrorContent);
				
				InDelegate.ExecuteIfBound(ResponseData, false);
			}
		});

	AddNetworkWaitCount(1);
	Request->ProcessRequest();
}

// =================================================================================
// RequestWriteSubmit
// =================================================================================

void UKLingoNetworkSystem::RequestWriteSubmit(const TArray<FString>& ImageNameArray, TArray<FString> InTargetText, FResponseWriteSubmitDelegate InDelegate)
{
	FString Url = NetworkConfig::GetFullUrl(RequestAPI::writes_submit);
	auto Request = SetupHttpRequest(Url, NETWORK_POST);
	
	FHttpMultipartFormData Form;
	for (FString ImageName : ImageNameArray)
	{
		if (!Form.AddFile(TEXT("files"), FString::Printf(TEXT("%s"), *ImageName)))
		{
			NETWORK_LOG(TEXT("[POST] OCR Extract: file load failed: %s"), *ImageName);
			FResponseWriteSubmit EmptyResponse;
			InDelegate.ExecuteIfBound(EmptyResponse, false);
			return;
		}
	}
	for (FString targetText : InTargetText)
	{
		Form.AddText(TEXT("target_texts"), targetText);
	}
	
	Form.SetupHttpRequest(Request);

	LogNetwork(ENetworkLogType::Post, *Request->GetURL());

	Request->OnProcessRequestComplete().BindLambda(
		[WeakThis = TWeakObjectPtr<UKLingoNetworkSystem>(this), InDelegate](FHttpRequestPtr Req, FHttpResponsePtr ResPtr, bool bWasSuccessful)
		{
			if (!WeakThis.IsValid() || IsEngineExitRequested())
				return;

			WeakThis->AddNetworkWaitCount(-1);
			FResponseWriteSubmit ResponseData;

			if (bWasSuccessful && ResPtr.IsValid())
			{
				const int32 ResponseCode = ResPtr->GetResponseCode();

				NETWORK_LOG(TEXT("[RES] Code: %d, Response: %s"), ResponseCode, *ResPtr->GetContentAsString());
				
				if (IsResSuccess(ResponseCode))
				{
					ResponseData.SetFromHttpResponse(ResPtr);
					InDelegate.ExecuteIfBound(ResponseData, true);
				}
				else
				{
					WeakThis->ShowNetworkErrorPopup(ResponseCode, ResPtr->GetContentAsString());
					InDelegate.ExecuteIfBound(ResponseData, false);
				}
			}
			else
			{
				NETWORK_LOG(TEXT("[POST] RequestOcrExtract failed - bSuccess: %s, Response valid: %s"),
					bWasSuccessful ? TEXT("true") : TEXT("false"),
					ResPtr.IsValid() ? TEXT("true") : TEXT("false"));
				
				int32 ErrorCode = ResPtr.IsValid() ? ResPtr->GetResponseCode() : 0;
				FString ErrorContent = ResPtr.IsValid() ? ResPtr->GetContentAsString() : TEXT("Network connection failed");
				WeakThis->ShowNetworkErrorPopup(ErrorCode, ErrorContent);
				
				InDelegate.ExecuteIfBound(ResponseData, false);
			}
		});

	AddNetworkWaitCount(1);
	Request->ProcessRequest();
}

void UKLingoNetworkSystem::RequestWriteResult(const FRequestWriteResult& Result, FResponseWriteResultDelegate InDelegate)
{
	FString Url = NetworkConfig::GetFullUrl(RequestAPI::wriite_result);
	auto Request = SetupHttpRequest(Url, NETWORK_POST);

	// Request Body 설정                                                                                                                                                                                                          
	FString RequestBody;
	if (Result.ToJsonString(RequestBody))
		Request->SetContentAsString(RequestBody);

	LogNetwork(ENetworkLogType::Post, *Request->GetURL(), *RequestBody);

	Request->OnProcessRequestComplete().BindLambda(
		[WeakThis = TWeakObjectPtr<UKLingoNetworkSystem>(this), InDelegate](FHttpRequestPtr Req, FHttpResponsePtr ResPtr, bool bWasSuccessful)
		{
			if (!WeakThis.IsValid() || IsEngineExitRequested())
				return;

			WeakThis->AddNetworkWaitCount(-1);
			FResponseWriteResult ResponseData;

			if (bWasSuccessful && ResPtr.IsValid())
			{
				const int32 ResponseCode = ResPtr->GetResponseCode();

				NETWORK_LOG(TEXT("[RES] Code: %d, Response: %s"), ResponseCode, *ResPtr->GetContentAsString());
				
				if (IsResSuccess(ResponseCode))
				{
					ResponseData.SetFromHttpResponse(ResPtr);
					InDelegate.ExecuteIfBound(ResponseData, true);
				}
				else
				{
					WeakThis->ShowNetworkErrorPopup(ResponseCode, ResPtr->GetContentAsString());
					InDelegate.ExecuteIfBound(ResponseData, false);
				}
			}
			else
			{
				NETWORK_LOG(TEXT("[POST] RequestOcrExtract failed - bSuccess: %s, Response valid: %s"),
					bWasSuccessful ? TEXT("true") : TEXT("false"),
					ResPtr.IsValid() ? TEXT("true") : TEXT("false"));
				
				int32 ErrorCode = ResPtr.IsValid() ? ResPtr->GetResponseCode() : 0;
				FString ErrorContent = ResPtr.IsValid() ? ResPtr->GetContentAsString() : TEXT("Network connection failed");
				WeakThis->ShowNetworkErrorPopup(ErrorCode, ErrorContent);
				
				InDelegate.ExecuteIfBound(ResponseData, false);
			}
		});

	AddNetworkWaitCount(1);
	Request->ProcessRequest();
}

void UKLingoNetworkSystem::RequestListenAudio(const FString& AudioText, FResponseListenAudioDelegate InDelegate)
{
	if (auto ACM = UVoiceCacheManager::Get(GetWorld()))
	{
		// HACK, 임시로 서버로 보내지 않게 하기 위해서
		// 일레븐랩스를 사용해서 하는데, 500이슈가 뜬다
		// 일레븐 랩스의 토큰을 모두 소진해서,
		// 그러면 이것을 당분간 해결하기전까지는 발생안하기 위해서 데이터를 스킵한다.
		TArray<uint8> CachedAudio;
		if (ACM->TryGetCachedAudio(AudioText, CachedAudio))
		{
			FResponseListenAudio Response;
			Response.audio_text = AudioText;
			Response.audio_base64 = CachedAudio;

			// ✅ 비동기 Delegate 호출 (타이밍 문제 해결)
			if (UWorld* World = GetWorld())
			{
				// Response를 힙에 할당하여 Lambda에서 안전하게 사용
				TSharedPtr<FResponseListenAudio> SharedResponse = MakeShared<FResponseListenAudio>(Response);

				World->GetTimerManager().SetTimerForNextTick([InDelegate, SharedResponse]()
				{
					// 다음 프레임에 Delegate 호출
					InDelegate.ExecuteIfBound(*SharedResponse, true);
				});
			}
			else
			{
				// World가 없으면 즉시 호출 (fallback)
				InDelegate.ExecuteIfBound(Response, true);
			}

			return;
		}
	}

	TMap<FString, FString> Query;
	Query.Add(TEXT("audio_text"), AudioText);
	FString Url = NetworkConfig::GetFullUrlWithQuery( RequestAPI::listenings_audio, Query );
	auto Request = SetupHttpRequest(Url, NETWORK_POST);
	LogNetwork(ENetworkLogType::Post, *Request->GetURL());

	Request->OnProcessRequestComplete().BindLambda(
		[WeakThis = TWeakObjectPtr<UKLingoNetworkSystem>(this), InDelegate, AudioText](FHttpRequestPtr Req, FHttpResponsePtr ResPtr, bool bWasSuccessful)
		{
			if (!WeakThis.IsValid() || IsEngineExitRequested())
				return;

			WeakThis->AddNetworkWaitCount(-1);
			FResponseListenAudio ResponseData;

			if (bWasSuccessful && ResPtr.IsValid())
			{
				const int32 ResponseCode = ResPtr->GetResponseCode();

				NETWORK_LOG(TEXT("[RES] RequestListenAudio - Code: %d"), ResponseCode);

				if (IsResSuccess(ResponseCode))
				{
					ResponseData.SetFromHttpResponse(ResPtr);
					ResponseData.PrintData();

					// [4] 캐시에 저장 (Lambda 캡처된 AudioText 사용)
					if (auto ACM = UVoiceCacheManager::Get(WeakThis->GetWorld()))
					{
						if (ResponseData.audio_base64.Num() > 0)
						{
							ACM->SaveToCache(AudioText, ResponseData.audio_base64);
						}
					}

					InDelegate.ExecuteIfBound(ResponseData, true);
				}
				else
				{
					WeakThis->ShowNetworkErrorPopup(ResponseCode, ResPtr->GetContentAsString());
					InDelegate.ExecuteIfBound(ResponseData, false);
				}
			}
			else
			{
				NETWORK_LOG(TEXT("[POST] RequestListenAudio failed - bSuccess: %s, Response valid: %s"),
					bWasSuccessful ? TEXT("true") : TEXT("false"),
					ResPtr.IsValid() ? TEXT("true") : TEXT("false"));

				int32 ErrorCode = ResPtr.IsValid() ? ResPtr->GetResponseCode() : 0;
				FString ErrorContent = ResPtr.IsValid() ? ResPtr->GetContentAsString() : TEXT("Network connection failed");
				WeakThis->ShowNetworkErrorPopup(ErrorCode, ErrorContent);

				InDelegate.ExecuteIfBound(ResponseData, false);
			}
		});

	AddNetworkWaitCount(1);
	Request->ProcessRequest();
}

void UKLingoNetworkSystem::RequestSpeakingJudges(
	const FString& Question,
	const FString& AudioPath,
	FResponseSpeakingJudesDelegate InDelegate)
{
	TMap<FString, FString> Query;
	Query.Add(TEXT("question"), Question);
	FString Url = NetworkConfig::GetFullUrlWithQuery( RequestAPI::speakings_judes, Query );
	auto Request = SetupHttpRequest(Url, NETWORK_POST);

	// 상대 경로를 절대 경로로 변환
	FString AbsoluteAudioPath = FPaths::IsRelative(AudioPath) 
		? FPaths::Combine(FPaths::ProjectDir(), AudioPath)
		: AudioPath;
	AbsoluteAudioPath = FPaths::ConvertRelativePathToFull(AbsoluteAudioPath);

	FHttpMultipartFormData Form;
	if (!Form.AddFile(TEXT("audio"), AbsoluteAudioPath))
	{
		NETWORK_LOG(TEXT("[POST] Speaking Questions: file load failed: %s"), *AudioPath);
		FResponseSpeakingJudes EmptyResponse;
		InDelegate.ExecuteIfBound(EmptyResponse, false);
		return;
	}
	Form.SetupHttpRequest(Request);

	LogNetwork(ENetworkLogType::Post, *Request->GetURL());

	Request->OnProcessRequestComplete().BindLambda(
		[WeakThis = TWeakObjectPtr<UKLingoNetworkSystem>(this), InDelegate](FHttpRequestPtr Req, FHttpResponsePtr ResPtr, bool bWasSuccessful)
		{
			if (!WeakThis.IsValid() || IsEngineExitRequested())
				return;

			WeakThis->AddNetworkWaitCount(-1);
			FResponseSpeakingJudes ResponseData;

			if (bWasSuccessful && ResPtr.IsValid())
			{
				const int32 ResponseCode = ResPtr->GetResponseCode();

				NETWORK_LOG(TEXT("[RES] RequestSpeakingQuestions - Code: %d, Response: %s"), ResponseCode, *ResPtr->GetContentAsString());
			
				if (IsResSuccess(ResponseCode))
				{
					ResponseData.SetFromHttpResponse(ResPtr);
					ResponseData.PrintData();
					InDelegate.ExecuteIfBound(ResponseData, true);
				}
				else
				{
					WeakThis->ShowNetworkErrorPopup(ResponseCode, ResPtr->GetContentAsString());
					InDelegate.ExecuteIfBound(ResponseData, false);
				}
			}
			else
			{
				NETWORK_LOG(TEXT("[POST] RequestSpeakingQuestions failed - bSuccess: %s, Response valid: %s"),
					bWasSuccessful ? TEXT("true") : TEXT("false"),
					ResPtr.IsValid() ? TEXT("true") : TEXT("false"));
			
				int32 ErrorCode = ResPtr.IsValid() ? ResPtr->GetResponseCode() : 0;
				FString ErrorContent = ResPtr.IsValid() ? ResPtr->GetContentAsString() : TEXT("Network connection failed");
				WeakThis->ShowNetworkErrorPopup(ErrorCode, ErrorContent);
			
				InDelegate.ExecuteIfBound(ResponseData, false);
			}
		});

	AddNetworkWaitCount(1);
	Request->ProcessRequest();
}





// =================================================================================
// RequestSpeakingQuestions
// =================================================================================

void UKLingoNetworkSystem::RequestInterviewHello(FResponseInterviewHelloDelegate InDelegate)
{
	FString Url = NetworkConfig::GetFullUrl(RequestAPI::interview_hello);
	auto Request = SetupHttpRequest(Url, NETWORK_GET);

	LogNetwork(ENetworkLogType::Get, *Request->GetURL());

	Request->OnProcessRequestComplete().BindLambda(
		[WeakThis = TWeakObjectPtr<UKLingoNetworkSystem>(this), InDelegate](FHttpRequestPtr Req, FHttpResponsePtr ResPtr, bool bWasSuccessful)
		{
			if (!WeakThis.IsValid() || IsEngineExitRequested())
				return;

			WeakThis->AddNetworkWaitCount(-1);
			FResponseInterviewHello ResponseData;

			if (bWasSuccessful && ResPtr.IsValid())
			{
				const int32 ResponseCode = ResPtr->GetResponseCode();

				NETWORK_LOG(TEXT("[RES] RequestInterviewHello - Code: %d, Response: %s"), ResponseCode, *ResPtr->GetContentAsString());
				
				if (IsResSuccess(ResponseCode))
				{
					ResponseData.SetFromHttpResponse(ResPtr);
					ResponseData.PrintData();
					InDelegate.ExecuteIfBound(ResponseData, true);
				}
				else
				{
					WeakThis->ShowNetworkErrorPopup(ResponseCode, ResPtr->GetContentAsString());
					InDelegate.ExecuteIfBound(ResponseData, false);
				}
			}
			else
			{
				NETWORK_LOG(TEXT("[POST] RequestInterviewHello failed - bSuccess: %s, Response valid: %s"),
					bWasSuccessful ? TEXT("true") : TEXT("false"),
					ResPtr.IsValid() ? TEXT("true") : TEXT("false"));
				
				int32 ErrorCode = ResPtr.IsValid() ? ResPtr->GetResponseCode() : 0;
				FString ErrorContent = ResPtr.IsValid() ? ResPtr->GetContentAsString() : TEXT("Network connection failed");
				WeakThis->ShowNetworkErrorPopup(ErrorCode, ErrorContent);
				
				InDelegate.ExecuteIfBound(ResponseData, false);
			}
		});

	AddNetworkWaitCount(1);
	Request->ProcessRequest();
}



void UKLingoNetworkSystem::RequestInterviewAnswer(const FRequestInterviewAnswer& Answer, FResponseInterviewAnswerDelegate InDelegate)
{
	// {room_id}?room_id=
	FString Endpoint = FString::Printf(TEXT("%s/%lld"), *RequestAPI::interview_answer, ULingoGameHelper::GetLingoGameState( GetWorld())->GetRoomId() );
	FString Url = NetworkConfig::GetFullUrl(Endpoint);
	
	auto Request = SetupHttpRequest(Url, NETWORK_POST);

	// Request Body 설정
	FString RequestBody;
	if (Answer.ToJsonString(RequestBody))
	{
		Request->SetContentAsString(RequestBody);
		NETWORK_LOG(TEXT("[POST] RequestInterviewAnswer Body: %s"), *RequestBody);
	}
	else
	{
		NETWORK_LOG(TEXT("[POST] RequestInterviewAnswer - Failed to serialize request body"));
	}

	LogNetwork(ENetworkLogType::Post, *Request->GetURL(), *RequestBody);

	Request->OnProcessRequestComplete().BindLambda(
		[WeakThis = TWeakObjectPtr<UKLingoNetworkSystem>(this), InDelegate](FHttpRequestPtr Req, FHttpResponsePtr ResPtr, bool bWasSuccessful)
		{
			if (!WeakThis.IsValid() || IsEngineExitRequested())
				return;

			WeakThis->AddNetworkWaitCount(-1);
			FResponseInterviewAnswer ResponseData;

			if (bWasSuccessful && ResPtr.IsValid())
			{
				const int32 ResponseCode = ResPtr->GetResponseCode();

				NETWORK_LOG(TEXT("[POST] RequestInterviewAnswer - Code: %d, Response: %s"),
					ResponseCode, *ResPtr->GetContentAsString());
				
				if (IsResSuccess(ResponseCode))
				{
					ResponseData.SetFromHttpResponse(ResPtr);
					ResponseData.PrintData();
					InDelegate.ExecuteIfBound(ResponseData, true);
				}
				else
				{
					WeakThis->ShowNetworkErrorPopup(ResponseCode, ResPtr->GetContentAsString());
					InDelegate.ExecuteIfBound(ResponseData, false);
				}
			}
			else
			{
				NETWORK_LOG(TEXT("[POST] RequestInterviewAnswer failed - bSuccess: %s, Response valid: %s"),
					bWasSuccessful ? TEXT("true") : TEXT("false"),
					ResPtr.IsValid() ? TEXT("true") : TEXT("false"));
				
				int32 ErrorCode = ResPtr.IsValid() ? ResPtr->GetResponseCode() : 0;
				FString ErrorContent = ResPtr.IsValid() ? ResPtr->GetContentAsString() : TEXT("Network connection failed");
				WeakThis->ShowNetworkErrorPopup(ErrorCode, ErrorContent);
				
				InDelegate.ExecuteIfBound(ResponseData, false);
			}
		});

	AddNetworkWaitCount(1);
	Request->ProcessRequest();
}

// void UKLingoNetworkSystem::RequestQuestResult(
// 	const FRequestReadQuestResult& Result,
// 	FResponseQuestResultDelegate InDelegate)
// {
// 	 FString Url = NetworkConfig::GetFullUrl(RequestAPI::quest_result);
//       auto Request = SetupHttpRequest(Url, NETWORK_POST);
//
//       // Request Body 설정                                                                                                                                                                                                          
//       FString RequestBody;
//       if (Result.ToJsonString(RequestBody))
//       {
// 	      Request->SetContentAsString(RequestBody);
//       }
//
//       LogNetwork(ENetworkLogType::Post, *Request->GetURL(), *RequestBody);
//
//       Request->OnProcessRequestComplete().BindLambda(
//           [WeakThis = TWeakObjectPtr<UKLingoNetworkSystem>(this), InDelegate](FHttpRequestPtr Req, FHttpResponsePtr ResPtr, bool bWasSuccessful)
//           {
//               if (!WeakThis.IsValid() || IsEngineExitRequested())
//                   return;
//
//               WeakThis->AddNetworkWaitCount(-1);
//               FResponseQuestResult ResponseData;
//
//               if (bWasSuccessful && ResPtr.IsValid())
//               {
//                   const int32 ResponseCode = ResPtr->GetResponseCode();
//
//                   NETWORK_LOG(TEXT("[POST] RequestQuestResult - Code: %d, Response: %s"),
//                       ResponseCode, *ResPtr->GetContentAsString());
//
//                   if (IsResSuccess(ResponseCode))
//                   {
//                       ResponseData.SetFromHttpResponse(ResPtr);
//                       ResponseData.PrintData();
//                       InDelegate.ExecuteIfBound(ResponseData, true);
//                   }
//                   else                                                                                                                                                                                                              
//                   {
//                       WeakThis->ShowNetworkErrorPopup(ResponseCode, ResPtr->GetContentAsString());
//                       InDelegate.ExecuteIfBound(ResponseData, false);
//                   }
//               }
//               else                                                                                                                                                                                                                  
//               {
//                   NETWORK_LOG(TEXT("[POST] RequestQuestResult failed - bSuccess: %s, Response valid: %s"),
//                       bWasSuccessful ? TEXT("true") : TEXT("false"),
//                       ResPtr.IsValid() ? TEXT("true") : TEXT("false"));
//
//                   int32 ErrorCode = ResPtr.IsValid() ? ResPtr->GetResponseCode() : 0;
//                   FString ErrorContent = ResPtr.IsValid() ? ResPtr->GetContentAsString() : TEXT("Network connection failed");
//                   WeakThis->ShowNetworkErrorPopup(ErrorCode, ErrorContent);
//
//                   InDelegate.ExecuteIfBound(ResponseData, false);
//               }
//           });
//
//       AddNetworkWaitCount(1);
//       Request->ProcessRequest();
// }
















void UKLingoNetworkSystem::RequestReadScenario(FResponseReadScenarioDelegate InDelegate)
{
	// URL 형식: /scenario/stages/redis/{room_id}/{scenario_id}/{stage_type}/{level}
	FString Endpoint = FString::Printf(TEXT("%s/%lld/%d/%d/%d"), *RequestAPI::scenario,
		ULingoGameHelper::GetLingoGameState( GetWorld())->GetRoomId(),
		1,
		ULingoGameHelper::GetStageTypeIndex(EQuestType::Read),
		ULingoGameHelper::GetLingoGameState( GetWorld())->GetRoomLevel());
	
	FString Url = NetworkConfig::GetFullUrl(Endpoint);
	auto Request = SetupHttpRequest(Url, NETWORK_GET);

	LogNetwork(ENetworkLogType::Get, *Request->GetURL());

	Request->OnProcessRequestComplete().BindLambda(
		[this, InDelegate](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSuccess)
		{
			AddNetworkWaitCount(-1);

			FResponseReadScenario ResponseData;

			if (bSuccess && HttpResponse.IsValid())
			{
				const int32 ResponseCode = HttpResponse->GetResponseCode();

				NETWORK_LOG(TEXT("[RES] Code: %d, Response: %s"), ResponseCode, *HttpResponse->GetContentAsString());

				if (IsResSuccess(ResponseCode))
				{
					ResponseData.SetFromHttpResponse(HttpResponse);
					// ResponseData.PrintData();
					InDelegate.ExecuteIfBound(ResponseData, true);
				}
				else
				{
					ShowNetworkErrorPopup(ResponseCode, HttpResponse->GetContentAsString());
					InDelegate.ExecuteIfBound(ResponseData, false);
				}
			}
			else
			{
				NETWORK_LOG(TEXT("[GET] RequestReadScenario failed - bSuccess: %s, Response valid: %s"),
					bSuccess ? TEXT("true") : TEXT("false"),
					HttpResponse.IsValid() ? TEXT("true") : TEXT("false"));
				
				int32 ErrorCode = HttpResponse.IsValid() ? HttpResponse->GetResponseCode() : 0;
				FString ErrorContent = HttpResponse.IsValid() ? HttpResponse->GetContentAsString() : TEXT("Network connection failed");
				ShowNetworkErrorPopup(ErrorCode, ErrorContent);
				
				InDelegate.ExecuteIfBound(ResponseData, false);
			}
		});

	AddNetworkWaitCount(1);
	Request->ProcessRequest();
}


void UKLingoNetworkSystem::RequestReadResult( const FRequestReadResult& Result, FResponseReadResultDelegate InDelegate)
{
	FString Url = NetworkConfig::GetFullUrl(RequestAPI::read_result);
    auto Request = SetupHttpRequest(Url, NETWORK_POST);

	// Request Body 설정                                                                                                                                                                                                          
	FString RequestBody;
	if (Result.ToJsonString(RequestBody))
		Request->SetContentAsString(RequestBody);

	LogNetwork(ENetworkLogType::Post, *Request->GetURL(), *RequestBody);

	Request->OnProcessRequestComplete().BindLambda(
	  [WeakThis = TWeakObjectPtr<UKLingoNetworkSystem>(this), InDelegate](FHttpRequestPtr Req, FHttpResponsePtr ResPtr, bool bWasSuccessful)
	  {
	      if (!WeakThis.IsValid() || IsEngineExitRequested())
	          return;

	      WeakThis->AddNetworkWaitCount(-1);
	      FResponseReadResult ResponseData;

	      if (bWasSuccessful && ResPtr.IsValid())
	      {
	          const int32 ResponseCode = ResPtr->GetResponseCode();

	          NETWORK_LOG(TEXT("[POST] RequestQuestResult - Code: %d, Response: %s"),
	              ResponseCode, *ResPtr->GetContentAsString());

	          if (IsResSuccess(ResponseCode))
	          {
	              ResponseData.SetFromHttpResponse(ResPtr);
	              ResponseData.PrintData();
	              InDelegate.ExecuteIfBound(ResponseData, true);
	          }
	          else                                                                                                                                                                                                              
	          {
	              WeakThis->ShowNetworkErrorPopup(ResponseCode, ResPtr->GetContentAsString());
	              InDelegate.ExecuteIfBound(ResponseData, false);
	          }
	      }
	      else                                                                                                                                                                                                                  
	      {
	          NETWORK_LOG(TEXT("[POST] RequestReadResult failed - bSuccess: %s, Response valid: %s"),
	              bWasSuccessful ? TEXT("true") : TEXT("false"),
	              ResPtr.IsValid() ? TEXT("true") : TEXT("false"));

	          int32 ErrorCode = ResPtr.IsValid() ? ResPtr->GetResponseCode() : 0;
	          FString ErrorContent = ResPtr.IsValid() ? ResPtr->GetContentAsString() : TEXT("Network connection failed");
	          WeakThis->ShowNetworkErrorPopup(ErrorCode, ErrorContent);

	          InDelegate.ExecuteIfBound(ResponseData, false);
	      }
	  });

	AddNetworkWaitCount(1);
	Request->ProcessRequest();
}



void UKLingoNetworkSystem::RequestListenScenario(FResponseListenScenarioDelegate InDelegate)
{
	// URL 형식: /scenario/stages/redis/{room_id}/{scenario_id}/{stage_type}/{level}
	FString Endpoint = FString::Printf(TEXT("%s/%lld/%d/%d/%d"), *RequestAPI::scenario,
		ULingoGameHelper::GetLingoGameState( GetWorld())->GetRoomId(),
		1,
		ULingoGameHelper::GetStageTypeIndex(EQuestType::Listen),
		ULingoGameHelper::GetLingoGameState( GetWorld())->GetRoomLevel());
	
	FString Url = NetworkConfig::GetFullUrl(Endpoint);
	auto Request = SetupHttpRequest(Url, NETWORK_GET);

	LogNetwork(ENetworkLogType::Get, *Request->GetURL());

	Request->OnProcessRequestComplete().BindLambda(
		[this, InDelegate](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSuccess)
		{
			AddNetworkWaitCount(-1);

			FResponseListenScenario ResponseData;

			if (bSuccess && HttpResponse.IsValid())
			{
				const int32 ResponseCode = HttpResponse->GetResponseCode();

				NETWORK_LOG(TEXT("[RES] Code: %d, Response: %s"), ResponseCode, *HttpResponse->GetContentAsString());

				if (IsResSuccess(ResponseCode))
				{
					ResponseData.SetFromHttpResponse(HttpResponse);
					// ResponseData.PrintData();
					InDelegate.ExecuteIfBound(ResponseData, true);
				}
				else
				{
					ShowNetworkErrorPopup(ResponseCode, HttpResponse->GetContentAsString());
					InDelegate.ExecuteIfBound(ResponseData, false);
				}
			}
			else
			{
				NETWORK_LOG(TEXT("[RES] RequestReadScenario failed - bSuccess: %s, Response valid: %s"),
					bSuccess ? TEXT("true") : TEXT("false"),
					HttpResponse.IsValid() ? TEXT("true") : TEXT("false"));
				
				int32 ErrorCode = HttpResponse.IsValid() ? HttpResponse->GetResponseCode() : 0;
				FString ErrorContent = HttpResponse.IsValid() ? HttpResponse->GetContentAsString() : TEXT("Network connection failed");
				ShowNetworkErrorPopup(ErrorCode, ErrorContent);
				
				InDelegate.ExecuteIfBound(ResponseData, false);
			}
		});

	AddNetworkWaitCount(1);
	Request->ProcessRequest();
}

void UKLingoNetworkSystem::RequestListenResult( const FRequestListenResult& Result, FResponseListenResultDelegate InDelegate)
{
	FString Url = NetworkConfig::GetFullUrl(RequestAPI::listen_result);
    auto Request = SetupHttpRequest(Url, NETWORK_POST);

	// Request Body 설정
	FString RequestBody;
	if (Result.ToJsonString(RequestBody))
		Request->SetContentAsString(RequestBody);

	LogNetwork(ENetworkLogType::Post, *Request->GetURL(), *RequestBody);

	Request->OnProcessRequestComplete().BindLambda(
	  [WeakThis = TWeakObjectPtr<UKLingoNetworkSystem>(this), InDelegate](FHttpRequestPtr Req, FHttpResponsePtr ResPtr, bool bWasSuccessful)
	  {
	      if (!WeakThis.IsValid() || IsEngineExitRequested())
	          return;

	      WeakThis->AddNetworkWaitCount(-1);
	      FResponseListenResult ResponseData;

	      if (bWasSuccessful && ResPtr.IsValid())
	      {
	          const int32 ResponseCode = ResPtr->GetResponseCode();

	          NETWORK_LOG(TEXT("[RES] RequestListenResult - Code: %d, Response: %s"),
	              ResponseCode, *ResPtr->GetContentAsString());

	          if (IsResSuccess(ResponseCode))
	          {
	              ResponseData.SetFromHttpResponse(ResPtr);
	              ResponseData.PrintData();
	              InDelegate.ExecuteIfBound(ResponseData, true);
	          }
	          else
	          {
	              WeakThis->ShowNetworkErrorPopup(ResponseCode, ResPtr->GetContentAsString());
	              InDelegate.ExecuteIfBound(ResponseData, false);
	          }
	      }
	      else
	      {
	          NETWORK_LOG(TEXT("[RES] RequestListenResult failed - bSuccess: %s, Response valid: %s"),
	              bWasSuccessful ? TEXT("true") : TEXT("false"),
	              ResPtr.IsValid() ? TEXT("true") : TEXT("false"));

	          int32 ErrorCode = ResPtr.IsValid() ? ResPtr->GetResponseCode() : 0;
	          FString ErrorContent = ResPtr.IsValid() ? ResPtr->GetContentAsString() : TEXT("Network connection failed");
	          WeakThis->ShowNetworkErrorPopup(ErrorCode, ErrorContent);

	          InDelegate.ExecuteIfBound(ResponseData, false);
	      }
	  });

	AddNetworkWaitCount(1);
	Request->ProcessRequest();
}

void UKLingoNetworkSystem::RequestSpeakScenario(FResponseSpeakScenarioDelegate InDelegate)
{
	// URL 형식: /scenario/stages/redis/{room_id}/{scenario_id}/{stage_type}/{level}
	FString Endpoint = FString::Printf(TEXT("%s/%lld/%d/%d/%d"), *RequestAPI::scenario,
		ULingoGameHelper::GetLingoGameState( GetWorld())->GetRoomId(),
		1,
		ULingoGameHelper::GetStageTypeIndex(EQuestType::Speak),
		ULingoGameHelper::GetLingoGameState( GetWorld())->GetRoomLevel());

	FString Url = NetworkConfig::GetFullUrl(Endpoint);
	auto Request = SetupHttpRequest(Url, NETWORK_GET);

	LogNetwork(ENetworkLogType::Get, *Request->GetURL());

	Request->OnProcessRequestComplete().BindLambda(
		[this, InDelegate](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSuccess)
		{
			AddNetworkWaitCount(-1);

			FResponseSpeakScenario ResponseData;

			if (bSuccess && HttpResponse.IsValid())
			{
				const int32 ResponseCode = HttpResponse->GetResponseCode();

				// NOTE : 굉장히 시끄러움.
				// NETWORK_LOG(TEXT("[RES] Code: %d, Response: %s"), ResponseCode, *HttpResponse->GetContentAsString());

				if (IsResSuccess(ResponseCode))
				{
					ResponseData.SetFromHttpResponse(HttpResponse);
					ResponseData.PrintData();
					InDelegate.ExecuteIfBound(ResponseData, true);
				}
				else
				{
					ShowNetworkErrorPopup(ResponseCode, HttpResponse->GetContentAsString());
					InDelegate.ExecuteIfBound(ResponseData, false);
				}
			}
			else
			{
				NETWORK_LOG(TEXT("[RES] RequestSpeakScenario failed - bSuccess: %s, Response valid: %s"),
					bSuccess ? TEXT("true") : TEXT("false"),
					HttpResponse.IsValid() ? TEXT("true") : TEXT("false"));

				int32 ErrorCode = HttpResponse.IsValid() ? HttpResponse->GetResponseCode() : 0;
				FString ErrorContent = HttpResponse.IsValid() ? HttpResponse->GetContentAsString() : TEXT("Network connection failed");
				ShowNetworkErrorPopup(ErrorCode, ErrorContent);

				InDelegate.ExecuteIfBound(ResponseData, false);
			}
		});

	AddNetworkWaitCount(1);
	Request->ProcessRequest();
}


void UKLingoNetworkSystem::RequestSpeakResult( const FRequestSpeakResult& Result, FResponseSpeakResultDelegate InDelegate)
{
	FString Url = NetworkConfig::GetFullUrl(RequestAPI::speak_result);
    auto Request = SetupHttpRequest(Url, NETWORK_POST);

	// Request Body 설정
	FString RequestBody;
	if (Result.ToJsonString(RequestBody))
		Request->SetContentAsString(RequestBody);

	LogNetwork(ENetworkLogType::Post, *Request->GetURL(), *RequestBody);

	Request->OnProcessRequestComplete().BindLambda(
	  [WeakThis = TWeakObjectPtr<UKLingoNetworkSystem>(this), InDelegate](FHttpRequestPtr Req, FHttpResponsePtr ResPtr, bool bWasSuccessful)
	  {
	      if (!WeakThis.IsValid() || IsEngineExitRequested())
	          return;

	      WeakThis->AddNetworkWaitCount(-1);
	      FResponseSpeakResult ResponseData;

	      if (bWasSuccessful && ResPtr.IsValid())
	      {
	          const int32 ResponseCode = ResPtr->GetResponseCode();

	          NETWORK_LOG(TEXT("[RES] FResponseSpeakResult - Code: %d, Response: %s"),
	              ResponseCode, *ResPtr->GetContentAsString());

	          if (IsResSuccess(ResponseCode))
	          {
	              ResponseData.SetFromHttpResponse(ResPtr);
	              ResponseData.PrintData();
	              InDelegate.ExecuteIfBound(ResponseData, true);
	          }
	          else
	          {
	              WeakThis->ShowNetworkErrorPopup(ResponseCode, ResPtr->GetContentAsString());
	              InDelegate.ExecuteIfBound(ResponseData, false);
	          }
	      }
	      else
	      {
	          NETWORK_LOG(TEXT("[RES] FResponseSpeakResult failed - bSuccess: %s, Response valid: %s"),
	              bWasSuccessful ? TEXT("true") : TEXT("false"),
	              ResPtr.IsValid() ? TEXT("true") : TEXT("false"));

	          int32 ErrorCode = ResPtr.IsValid() ? ResPtr->GetResponseCode() : 0;
	          FString ErrorContent = ResPtr.IsValid() ? ResPtr->GetContentAsString() : TEXT("Network connection failed");
	          WeakThis->ShowNetworkErrorPopup(ErrorCode, ErrorContent);

	          InDelegate.ExecuteIfBound(ResponseData, false);
	      }
	  });

	AddNetworkWaitCount(1);
	Request->ProcessRequest();
}

// =================================================================================
// RequestEvaluationResult
// =================================================================================

void UKLingoNetworkSystem::RequestEvaluationResult(int32 RoomId, FResponseEvaluationResultDelegate InDelegate)
{
	// URL 형식: /evaluations/rooms/{room_id}
	FString Endpoint = FString::Printf(TEXT("%s/%d"), *RequestAPI::evaluations_rooms, RoomId);
	FString Url = NetworkConfig::GetFullUrl(Endpoint);
	auto Request = SetupHttpRequest(Url, NETWORK_GET);

	LogNetwork(ENetworkLogType::Get, *Request->GetURL());

	Request->OnProcessRequestComplete().BindLambda(
		[this, InDelegate](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSuccess)
		{
			AddNetworkWaitCount(-1);

			FResponseEvaluationResult ResponseData;

			if (bSuccess && HttpResponse.IsValid())
			{
				const int32 ResponseCode = HttpResponse->GetResponseCode();

				NETWORK_LOG(TEXT("[RES] RequestEvaluationResult - Code: %d, Response: %s"), 
					ResponseCode, *HttpResponse->GetContentAsString());

				if (IsResSuccess(ResponseCode))
				{
					ResponseData.SetFromHttpResponse(HttpResponse);
					ResponseData.PrintData();
					InDelegate.ExecuteIfBound(ResponseData, true);
				}
				else
				{
					ShowNetworkErrorPopup(ResponseCode, HttpResponse->GetContentAsString());
					InDelegate.ExecuteIfBound(ResponseData, false);
				}
			}
			else
			{
				NETWORK_LOG(TEXT("[GET] RequestEvaluationResult failed - bSuccess: %s, Response valid: %s"),
					bSuccess ? TEXT("true") : TEXT("false"),
					HttpResponse.IsValid() ? TEXT("true") : TEXT("false"));

				int32 ErrorCode = HttpResponse.IsValid() ? HttpResponse->GetResponseCode() : 0;
				FString ErrorContent = HttpResponse.IsValid() ? HttpResponse->GetContentAsString() : TEXT("Network connection failed");
				ShowNetworkErrorPopup(ErrorCode, ErrorContent);

				InDelegate.ExecuteIfBound(ResponseData, false);
			}
		});

	AddNetworkWaitCount(1);
	Request->ProcessRequest();
}


// =================================================================================
// RequestChatAnswers (Text Question)
// =================================================================================

void UKLingoNetworkSystem::RequestChatQuestion(const FString& Context, const FString& Question, FResponseChatAnswersDelegate InDelegate)
{
	TMap<FString, FString> Query;
	Query.Add(TEXT("context"), Context);
	Query.Add(TEXT("question"), Question);
	FString Url = NetworkConfig::GetFullUrlWithQuery( RequestAPI::chats_answers, Query );
	auto Request = SetupHttpRequest(Url, NETWORK_POST);
	
	LogNetwork(ENetworkLogType::Post, *Request->GetURL(), FString::Printf(TEXT("Context: %s, Question: %s"), *Context, *Question));

	Request->OnProcessRequestComplete().BindLambda(
		[WeakThis = TWeakObjectPtr<UKLingoNetworkSystem>(this), InDelegate](
			FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSuccess)
		{
			if (!WeakThis.IsValid() || IsEngineExitRequested())
				return;

			WeakThis->AddNetworkWaitCount(-1);
			FResponseChatAnswers ResponseData;

			if (bSuccess && HttpResponse.IsValid())
			{
				const int32 ResponseCode = HttpResponse->GetResponseCode();

				NETWORK_LOG(TEXT("[RES] RequestChatAnswers - Code: %d, Response: %s"),
					ResponseCode, *HttpResponse->GetContentAsString());

				if (IsResSuccess(ResponseCode))
				{
					ResponseData.SetFromHttpResponse(HttpResponse);
					ResponseData.PrintData();

					InDelegate.ExecuteIfBound(ResponseData, true);
				}
				else
				{
					WeakThis->ShowNetworkErrorPopup(ResponseCode, HttpResponse->GetContentAsString());
					InDelegate.ExecuteIfBound(ResponseData, false);
				}
			}
			else
			{
				NETWORK_LOG(TEXT("[POST] RequestChatAnswers failed - bSuccess: %s, Response valid: %s"),
					bSuccess ? TEXT("true") : TEXT("false"),
					HttpResponse.IsValid() ? TEXT("true") : TEXT("false"));

				int32 ErrorCode = HttpResponse.IsValid() ? HttpResponse->GetResponseCode() : 0;
				FString ErrorContent = HttpResponse.IsValid() ? HttpResponse->GetContentAsString() : TEXT("Network connection failed");
				WeakThis->ShowNetworkErrorPopup(ErrorCode, ErrorContent);

				InDelegate.ExecuteIfBound(ResponseData, false);
			}
		});

	AddNetworkWaitCount(1);
	Request->ProcessRequest();
}


// =================================================================================
// RequestChatAnswers (Audio Question)
// =================================================================================

void UKLingoNetworkSystem::RequestChatAudio(const FString& Context, const FString& AudioPath, FResponseChatAnswersDelegate InDelegate)
{
	FString Url = NetworkConfig::GetFullUrl(RequestAPI::chats_answers);
	auto Request = SetupHttpRequest(Url, NETWORK_POST);

	// 상대 경로를 절대 경로로 변환
	FString AbsoluteAudioPath = FPaths::IsRelative(AudioPath)
		? FPaths::Combine(FPaths::ProjectDir(), AudioPath)
		: AudioPath;
	AbsoluteAudioPath = FPaths::ConvertRelativePathToFull(AbsoluteAudioPath);

	// multipart/form-data로 context와 audio 파일 전송
	FHttpMultipartFormData Form;
	Form.AddText(TEXT("context"), Context);

	if (!Form.AddFile(TEXT("audio"), AbsoluteAudioPath))
	{
		NETWORK_LOG(TEXT("[POST] RequestChatAnswersWithAudio: audio file load failed: %s"), *AudioPath);
		FResponseChatAnswers EmptyResponse;
		InDelegate.ExecuteIfBound(EmptyResponse, false);
		return;
	}

	Form.SetupHttpRequest(Request);

	LogNetwork(ENetworkLogType::Post, *Request->GetURL(), FString::Printf(TEXT("Context: %s, AudioPath: %s"), *Context, *AudioPath));

	Request->OnProcessRequestComplete().BindLambda(
		[WeakThis = TWeakObjectPtr<UKLingoNetworkSystem>(this), InDelegate](
			FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSuccess)
		{
			if (!WeakThis.IsValid() || IsEngineExitRequested())
				return;

			WeakThis->AddNetworkWaitCount(-1);
			FResponseChatAnswers ResponseData;

			if (bSuccess && HttpResponse.IsValid())
			{
				const int32 ResponseCode = HttpResponse->GetResponseCode();

				NETWORK_LOG(TEXT("[RES] RequestChatAnswersWithAudio - Code: %d, Response: %s"),
					ResponseCode, *HttpResponse->GetContentAsString());

				if (IsResSuccess(ResponseCode))
				{
					ResponseData.SetFromHttpResponse(HttpResponse);
					ResponseData.PrintData();
					InDelegate.ExecuteIfBound(ResponseData, true);
				}
				else
				{
					WeakThis->ShowNetworkErrorPopup(ResponseCode, HttpResponse->GetContentAsString());
					InDelegate.ExecuteIfBound(ResponseData, false);
				}
			}
			else
			{
				NETWORK_LOG(TEXT("[POST] RequestChatAnswersWithAudio failed - bSuccess: %s, Response valid: %s"),
					bSuccess ? TEXT("true") : TEXT("false"),
					HttpResponse.IsValid() ? TEXT("true") : TEXT("false"));

				int32 ErrorCode = HttpResponse.IsValid() ? HttpResponse->GetResponseCode() : 0;
				FString ErrorContent = HttpResponse.IsValid() ? HttpResponse->GetContentAsString() : TEXT("Network connection failed");
				WeakThis->ShowNetworkErrorPopup(ErrorCode, ErrorContent);

				InDelegate.ExecuteIfBound(ResponseData, false);
			}
		});

	AddNetworkWaitCount(1);
	Request->ProcessRequest();
}