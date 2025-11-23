// Copyright (c) 2025 Doppleddiggong. All rights reserved.
// Unauthorized copying, modification, or distribution of this file,
// via any medium is strictly prohibited. Proprietary and confidential.

/// @file UKLingoNetworkSystem.cpp
/// @brief KLingo API 요청 서브시스템의 구현을 제공합니다.

#include "UKLingoNetworkSystem.h"

#include "ALingoPlayerState.h"
#include "HttpModule.h"
#include "NetworkLog.h"
#include "NetworkData.h"
#include "FHttpMultipartFormData.h"
#include "ULingoGameHelper.h"
#include "Misc/Paths.h"

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


void UKLingoNetworkSystem::AddNetworkWaitCount(int Value)
{
	NetworkWaitCount += Value;
	NETWORK_LOG( TEXT("[KLingo] Network Wait Count: %d"), NetworkWaitCount);
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
	JsonObject->SetStringField(TEXT("email"), UserName + TEXT("@klingo.com"));
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

				if (ResponseCode == 200)
				{
					ResponseData.SetFromHttpResponse(HttpResponse);
					ResponseData.PrintData();
					InDelegate.ExecuteIfBound(ResponseData, true);
				}
				else
				{
					NETWORK_LOG(TEXT("[POST] RequestUserRegister failed - Code: %d, Response: %s"),
						ResponseCode, *HttpResponse->GetContentAsString());
					InDelegate.ExecuteIfBound(ResponseData, false);
				}
			}
			else
			{
				NETWORK_LOG(TEXT("[POST] RequestUserRegister failed - bSuccess: %s, Response valid: %s"),
					bSuccess ? TEXT("true") : TEXT("false"),
					HttpResponse.IsValid() ? TEXT("true") : TEXT("false"));
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

	LogNetwork(ENetworkLogType::Post, *Request->GetURL(), TEXT("grant_type=password&username=***&password=***"));
	
	Request->OnProcessRequestComplete().BindLambda(
		[this, InDelegate](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSuccess)
		{
			AddNetworkWaitCount(-1);

			FResponseUserToken ResponseData;

			if (bSuccess && HttpResponse.IsValid())
			{
				const int32 ResponseCode = HttpResponse->GetResponseCode();

				if (ResponseCode == 200)
				{
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
					InDelegate.ExecuteIfBound(ResponseData, false);
				}
			}
			else
			{
				NETWORK_LOG(TEXT("[POST] RequestUserToken failed - bSuccess: %s, Response valid: %s"),
					bSuccess ? TEXT("true") : TEXT("false"),
					HttpResponse.IsValid() ? TEXT("true") : TEXT("false"));
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

				if (ResponseCode == 200)
				{
					ResponseData.SetFromHttpResponse(HttpResponse);
					ResponseData.PrintData();
					InDelegate.ExecuteIfBound(ResponseData, true);
				}
				else
				{
					NETWORK_LOG(TEXT("[GET] RequestUserMe failed - Code: %d, Response: %s"),
						ResponseCode, *HttpResponse->GetContentAsString());
					InDelegate.ExecuteIfBound(ResponseData, false);
				}
			}
			else
			{
				NETWORK_LOG(TEXT("[GET] RequestUserMe failed - bSuccess: %s, Response valid: %s"),
					bSuccess ? TEXT("true") : TEXT("false"),
					HttpResponse.IsValid() ? TEXT("true") : TEXT("false"));
				InDelegate.ExecuteIfBound(ResponseData, false);
			}
		});

	AddNetworkWaitCount(1);
	Request->ProcessRequest();
}

// =================================================================================
// RequestScenario
// =================================================================================

void UKLingoNetworkSystem::RequestScenario(int32 Index, int32 Difficulty, int32 Lang, FResponseScenarioDelegate InDelegate)
{
	// URL 형식: /scenario/{index}/{dificulity}/{lang}
	FString Endpoint = FString::Printf(TEXT("%s/%d/%d/%d"), *RequestAPI::scenario, Index, Difficulty, Lang);
	FString Url = NetworkConfig::GetFullUrl(Endpoint);
	auto Request = SetupHttpRequest(Url, NETWORK_GET);

	LogNetwork(ENetworkLogType::Get, *Request->GetURL());

	Request->OnProcessRequestComplete().BindLambda(
		[this, InDelegate](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSuccess)
		{
			AddNetworkWaitCount(-1);

			FResponseScenario ResponseData;

			if (bSuccess && HttpResponse.IsValid())
			{
				const int32 ResponseCode = HttpResponse->GetResponseCode();

				if (ResponseCode == 200)
				{
					ResponseData.SetFromHttpResponse(HttpResponse);
					ResponseData.PrintData();
					InDelegate.ExecuteIfBound(ResponseData, true);
				}
				else
				{
					NETWORK_LOG(TEXT("[GET] RequestScenario failed - Code: %d, Response: %s"),
						ResponseCode, *HttpResponse->GetContentAsString());
					InDelegate.ExecuteIfBound(ResponseData, false);
				}
			}
			else
			{
				NETWORK_LOG(TEXT("[GET] RequestScenario failed - bSuccess: %s, Response valid: %s"),
					bSuccess ? TEXT("true") : TEXT("false"),
					HttpResponse.IsValid() ? TEXT("true") : TEXT("false"));
				InDelegate.ExecuteIfBound(ResponseData, false);
			}
		});

	AddNetworkWaitCount(1);
	Request->ProcessRequest();
}

// =================================================================================
// RequestOcrExtract
// =================================================================================

void UKLingoNetworkSystem::RequestOcrExtract(const FString& ImagePath, FResponseOcrExtractDelegate InDelegate)
{
	FString Url = NetworkConfig::GetFullUrl(RequestAPI::writes_ocr_extract);
	auto Request = SetupHttpRequest(Url, NETWORK_POST);

	// 상대 경로를 절대 경로로 변환
	FString AbsoluteImagePath = FPaths::IsRelative(ImagePath) 
		? FPaths::Combine(FPaths::ProjectDir(), ImagePath)
		: ImagePath;
	AbsoluteImagePath = FPaths::ConvertRelativePathToFull(AbsoluteImagePath);

	FHttpMultipartFormData Form;
	if (!Form.AddFile(TEXT("file"), AbsoluteImagePath))
	{
		NETWORK_LOG(TEXT("[POST] OCR Extract: file load failed: %s"), *AbsoluteImagePath);
		FResponseOcrExtract EmptyResponse;
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
			FResponseOcrExtract ResponseData;

			if (bWasSuccessful && ResPtr.IsValid())
			{
				const int32 ResponseCode = ResPtr->GetResponseCode();

				if (ResponseCode == 200)
				{
					ResponseData.SetFromHttpResponse(ResPtr);
					ResponseData.PrintData();
					InDelegate.ExecuteIfBound(ResponseData, true);
				}
				else
				{
					NETWORK_LOG(TEXT("[POST] RequestOcrExtract failed - Code: %d, Response: %s"),
						ResponseCode, *ResPtr->GetContentAsString());
					InDelegate.ExecuteIfBound(ResponseData, false);
				}
			}
			else
			{
				NETWORK_LOG(TEXT("[POST] RequestOcrExtract failed - bSuccess: %s, Response valid: %s"),
					bWasSuccessful ? TEXT("true") : TEXT("false"),
					ResPtr.IsValid() ? TEXT("true") : TEXT("false"));
				InDelegate.ExecuteIfBound(ResponseData, false);
			}
		});

	AddNetworkWaitCount(1);
	Request->ProcessRequest();
}

// =================================================================================
// RequestSpeakingQuestions
// =================================================================================

void UKLingoNetworkSystem::RequestSpeakingQuestions(const FString& AudioPath, FResponseSpeakingQuestionsDelegate InDelegate)
{
	FString Url = NetworkConfig::GetFullUrl(RequestAPI::speakings_questions);
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
		FResponseSpeakingQuestions EmptyResponse;
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
			FResponseSpeakingQuestions ResponseData;

			if (bWasSuccessful && ResPtr.IsValid())
			{
				const int32 ResponseCode = ResPtr->GetResponseCode();

				if (ResponseCode == 200)
				{
					ResponseData.SetFromHttpResponse(ResPtr);
					ResponseData.PrintData();
					InDelegate.ExecuteIfBound(ResponseData, true);
				}
				else
				{
					NETWORK_LOG(TEXT("[POST] RequestSpeakingQuestions failed - Code: %d, Response: %s"),
						ResponseCode, *ResPtr->GetContentAsString());
					InDelegate.ExecuteIfBound(ResponseData, false);
				}
			}
			else
			{
				NETWORK_LOG(TEXT("[POST] RequestSpeakingQuestions failed - bSuccess: %s, Response valid: %s"),
					bWasSuccessful ? TEXT("true") : TEXT("false"),
					ResPtr.IsValid() ? TEXT("true") : TEXT("false"));
				InDelegate.ExecuteIfBound(ResponseData, false);
			}
		});

	AddNetworkWaitCount(1);
	Request->ProcessRequest();
}


#pragma region READY
/*

// =================================================================================
// RequestLogin
// =================================================================================

void UKLingoNetworkSystem::RequestLogin(const FString& Account, FResponseLoginDelegate InDelegate)
{
	FString Url = NetworkConfig::GetFullUrl(RequestAPI::Login);
	auto Request = SetupHttpRequest( Url, NETWORK_GET );
	
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField(TEXT("account"), Account);

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
	Request->SetContentAsString(RequestBody);

	LogNetwork(ENetworkLogType::Get, *Request->GetURL(), *RequestBody);
	
	Request->OnProcessRequestComplete().BindLambda(
		[this, InDelegate](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSuccess)
		{
			AddNetworkWaitCount(-1);

			FResponseLogin ResponseData;

			if (bSuccess && HttpResponse.IsValid() && HttpResponse->GetResponseCode() == 200)
			{
				ResponseData.SetFromHttpResponse(HttpResponse);
				ResponseData.PrintData();

				access_token = ResponseData.Token;
				
				if (auto PS = ULingoGameHelper::GetLingoPlayerState(this))
					PS->SetToken(ResponseData.Token);
			
				InDelegate.ExecuteIfBound(ResponseData, true);
			}
			else
			{
				NETWORK_LOG( TEXT("[POST] Login failed"));
				InDelegate.ExecuteIfBound(ResponseData, false);
			}
		});

	AddNetworkWaitCount(1);
	Request->ProcessRequest();
}

// =================================================================================
// RequestCreateUser
// =================================================================================

void UKLingoNetworkSystem::RequestCreateUser(int32 Character, int32 CharacterColor, FResponseCreateUserDelegate InDelegate)
{
	FString Url = NetworkConfig::GetFullUrl(RequestAPI::CreateUser);

	auto Request = SetupHttpRequest( Url, NETWORK_POST );
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetNumberField(TEXT("character"), Character);
	JsonObject->SetNumberField(TEXT("characterColor"), CharacterColor);

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
	Request->SetContentAsString(RequestBody);

	LogNetwork(ENetworkLogType::Post, *Request->GetURL(), *RequestBody);
	
	Request->OnProcessRequestComplete().BindLambda(
		[this, InDelegate](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSuccess)
		{
			AddNetworkWaitCount(-1);

			FResponseCreateUser ResponseData;

			if (bSuccess && HttpResponse.IsValid() && HttpResponse->GetResponseCode() == 200)
			{
				ResponseData.SetFromHttpResponse(HttpResponse);
				ResponseData.PrintData();
				InDelegate.ExecuteIfBound(ResponseData, true);
			}
			else
			{
				NETWORK_LOG(TEXT("[POST] CreateUser failed"));
				InDelegate.ExecuteIfBound(ResponseData, false);
			}
		});

	AddNetworkWaitCount(1);
	Request->ProcessRequest();
}

// =================================================================================
// RequestInterview
// =================================================================================

void UKLingoNetworkSystem::RequestInterview(FResponseInterviewDelegate InDelegate)
{
	FString Url = NetworkConfig::GetFullUrl(RequestAPI::Interview);

	auto Request = SetupHttpRequest( Url, NETWORK_GET );

	LogNetwork(ENetworkLogType::Get, *Request->GetURL() );

	Request->OnProcessRequestComplete().BindLambda(
		[this, InDelegate](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSuccess)
		{
			AddNetworkWaitCount(-1);

			FResponseInterview ResponseData;

			if (bSuccess && HttpResponse.IsValid() && HttpResponse->GetResponseCode() == 200)
			{
				ResponseData.SetFromHttpResponse(HttpResponse);
				ResponseData.PrintData();
				InDelegate.ExecuteIfBound(ResponseData, true);
			}
			else
			{
				NETWORK_LOG(TEXT("[GET] RequestInterview failed"));
				InDelegate.ExecuteIfBound(ResponseData, false);
			}
		});

	AddNetworkWaitCount(1);
	Request->ProcessRequest();
}

// =================================================================================
// RequestSubmitInterview
// =================================================================================

void UKLingoNetworkSystem::RequestSubmitInterview(const TArray<FString>& Answers, FResponseInterviewDelegate InDelegate)
{
	FString Url = NetworkConfig::GetFullUrl(RequestAPI::SubmitInterview);

	auto Request = SetupHttpRequest( Url, NETWORK_POST );

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	TArray<TSharedPtr<FJsonValue>> AnswersArray;
	for (const FString& Answer : Answers)
		AnswersArray.Add(MakeShareable(new FJsonValueString(Answer)));
	JsonObject->SetArrayField(TEXT("answers"), AnswersArray);

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
	Request->SetContentAsString(RequestBody);

	Request->OnProcessRequestComplete().BindLambda(
		[this, InDelegate](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSuccess)
		{
			AddNetworkWaitCount(-1);

			FResponseInterview ResponseData;

			if (bSuccess && HttpResponse.IsValid() && HttpResponse->GetResponseCode() == 200)
			{
				ResponseData.SetFromHttpResponse(HttpResponse);
				ResponseData.PrintData();
				InDelegate.ExecuteIfBound(ResponseData, true);
			}
			else
			{
				NETWORK_LOG( TEXT("[POST] SubmitInterview failed"));
				InDelegate.ExecuteIfBound(ResponseData, false);
			}
		});

	AddNetworkWaitCount(1);
	Request->ProcessRequest();
}

// =================================================================================
// RequestStartGame
// =================================================================================

void UKLingoNetworkSystem::RequestStartGame(const TArray<FString>& PlayerList, const TArray<FString>& Nicknames, FResponseStartGameDelegate InDelegate)
{
	FString Url = NetworkConfig::GetFullUrl(RequestAPI::StartGame);

	auto Request = SetupHttpRequest( Url, NETWORK_POST );
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

	TArray<TSharedPtr<FJsonValue>> PlayerListArray;
	for (const FString& Player : PlayerList)
		PlayerListArray.Add(MakeShareable(new FJsonValueString(Player)));
	JsonObject->SetArrayField(TEXT("playerList"), PlayerListArray);

	TArray<TSharedPtr<FJsonValue>> NicknamesArray;
	for (const FString& Nickname : Nicknames)
		NicknamesArray.Add(MakeShareable(new FJsonValueString(Nickname)));
	JsonObject->SetArrayField(TEXT("nicknames"), NicknamesArray);

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
	Request->SetContentAsString(RequestBody);

	LogNetwork(ENetworkLogType::Post, *Request->GetURL(), *RequestBody);

	Request->OnProcessRequestComplete().BindLambda(
		[this, InDelegate](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSuccess)
		{
			AddNetworkWaitCount(-1);

			FResponseStartGame ResponseData;

			if (bSuccess && HttpResponse.IsValid() && HttpResponse->GetResponseCode() == 200)
			{
				ResponseData.SetFromHttpResponse(HttpResponse);
				ResponseData.PrintData();
				InDelegate.ExecuteIfBound(ResponseData, true);
			}
			else
			{
				NETWORK_LOG(TEXT("[POST] StartGame failed"));
				InDelegate.ExecuteIfBound(ResponseData, false);
			}
		});

	AddNetworkWaitCount(1);
	Request->ProcessRequest();
}

// =================================================================================
// RequestGameLogin
// =================================================================================

void UKLingoNetworkSystem::RequestGameLogin(FResponseGameLoginDelegate InDelegate)
{
	FString Url = NetworkConfig::GetFullUrl(RequestAPI::GameLogin);

	auto Request = SetupHttpRequest( Url, NETWORK_GET );

	LogNetwork(ENetworkLogType::Get, *Request->GetURL());

	Request->OnProcessRequestComplete().BindLambda(
		[this, InDelegate](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSuccess)
		{
			AddNetworkWaitCount(-1);

			FResponseGameLogin ResponseData;

			if (bSuccess && HttpResponse.IsValid() && HttpResponse->GetResponseCode() == 200)
			{
				ResponseData.SetFromHttpResponse(HttpResponse);
				ResponseData.PrintData();
				InDelegate.ExecuteIfBound(ResponseData, true);
			}
			else
			{
				NETWORK_LOG(TEXT("[POST] GameLogin failed"));
				InDelegate.ExecuteIfBound(ResponseData, false);
			}
		});

	AddNetworkWaitCount(1);
	Request->ProcessRequest();
}

// =================================================================================
// RequestQuestAnswer
// =================================================================================

void UKLingoNetworkSystem::RequestQuestAnswer(int32 QuestStep, int32 QuestAnswer, float PlayTime, FResponseQuestAnswerDelegate InDelegate)
{
	FString Url = NetworkConfig::GetFullUrl(RequestAPI::QuestAnswer);

	auto Request = SetupHttpRequest( Url, NETWORK_POST );
	
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetNumberField(TEXT("questStep"), QuestStep);
	JsonObject->SetNumberField(TEXT("questAnswer"), QuestAnswer);
	JsonObject->SetNumberField(TEXT("playTime"), PlayTime);

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
	Request->SetContentAsString(RequestBody);

	LogNetwork(ENetworkLogType::Post, *Request->GetURL(), *RequestBody);
	
	Request->OnProcessRequestComplete().BindLambda(
		[this, InDelegate](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSuccess)
		{
			AddNetworkWaitCount(-1);

			FResponseQuestAnswer ResponseData;

			if (bSuccess && HttpResponse.IsValid() && HttpResponse->GetResponseCode() == 200)
			{
				ResponseData.SetFromHttpResponse(HttpResponse);
				ResponseData.PrintData();
				InDelegate.ExecuteIfBound(ResponseData, true);
			}
			else
			{
				NETWORK_LOG(TEXT("[POST] QuestAnswer failed"));
				InDelegate.ExecuteIfBound(ResponseData, false);
			}
		});

	AddNetworkWaitCount(1);
	Request->ProcessRequest();
}

// =================================================================================
// RequestQuestFail
// =================================================================================

void UKLingoNetworkSystem::RequestQuestFail(int32 QuestStep, FResponseQuestAnswerDelegate InDelegate)
{
	FString Url = NetworkConfig::GetFullUrl(RequestAPI::QuestFail);

	auto Request = SetupHttpRequest( Url, NETWORK_POST );

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetNumberField(TEXT("questStep"), QuestStep);
	
	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
	Request->SetContentAsString(RequestBody);

	LogNetwork(ENetworkLogType::Post, *Request->GetURL(), *RequestBody);

	Request->OnProcessRequestComplete().BindLambda(
		[this, InDelegate](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSuccess)
		{
			AddNetworkWaitCount(-1);

			FResponseQuestAnswer ResponseData;

			if (bSuccess && HttpResponse.IsValid() && HttpResponse->GetResponseCode() == 200)
			{
				ResponseData.SetFromHttpResponse(HttpResponse);
				ResponseData.PrintData();
				InDelegate.ExecuteIfBound(ResponseData, true);
			}
			else
			{
				NETWORK_LOG(TEXT("[POST] QuestFail failed"));
				InDelegate.ExecuteIfBound(ResponseData, false);
			}
		});

	AddNetworkWaitCount(1);
	Request->ProcessRequest();
}

// =================================================================================
// RequestQuestWrite
// =================================================================================

void UKLingoNetworkSystem::RequestQuestWrite(const TArray<FString>& ImagePaths, const TArray<FString>& TextData, FResponseQuestWriteDelegate InDelegate)
{
	FString Url = NetworkConfig::GetFullUrl(RequestAPI::QuestWrite);

	auto Request = SetupHttpRequest( Url, NETWORK_POST );
	
	FHttpMultipartFormData FormData;
	for (int32 i = 0; i < ImagePaths.Num(); ++i)
		FormData.AddFile(FString::Printf(TEXT("imageData[%d]"), i), ImagePaths[i], TEXT("image/png"));
	for (int32 i = 0; i < TextData.Num(); ++i)
		FormData.AddText(FString::Printf(TEXT("textData[%d]"), i), TextData[i]);

	FormData.SetupHttpRequest(Request);

	LogNetwork(ENetworkLogType::Post, *Request->GetURL());
	
	Request->OnProcessRequestComplete().BindLambda(
		[this, InDelegate](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSuccess)
		{
			AddNetworkWaitCount(-1);

			FResponseQuestWrite ResponseData;

			if (bSuccess && HttpResponse.IsValid() && HttpResponse->GetResponseCode() == 200)
			{
				ResponseData.SetFromHttpResponse(HttpResponse);
				ResponseData.PrintData();
				InDelegate.ExecuteIfBound(ResponseData, true);
			}
			else
			{
				NETWORK_LOG(TEXT("[POST] QuestWrite failed"));
				InDelegate.ExecuteIfBound(ResponseData, false);
			}
		});

	AddNetworkWaitCount(1);
	Request->ProcessRequest();
}

// =================================================================================
// RequestQuestSpeak
// =================================================================================

void UKLingoNetworkSystem::RequestQuestSpeak(int32 SpeakStep, const FString& WavFilePath, FResponseQuestSpeakDelegate InDelegate)
{
	FString Url = NetworkConfig::GetFullUrl(RequestAPI::QuestSpeak);

	auto Request = SetupHttpRequest( Url, NETWORK_POST );

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetNumberField(TEXT("speakStep"), SpeakStep);

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
	
	FHttpMultipartFormData FormData;
	FormData.AddFile(TEXT("waveData"), WavFilePath, TEXT("audio/wav"));
	FormData.AddText(TEXT("context"), RequestBody);
	FormData.SetupHttpRequest(Request);
	
	LogNetwork(ENetworkLogType::Post, *Request->GetURL(), *RequestBody);
	
	Request->OnProcessRequestComplete().BindLambda(
		[this, InDelegate](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSuccess)
		{
			AddNetworkWaitCount(-1);

			FResponseQuestSpeak ResponseData;

			if (bSuccess && HttpResponse.IsValid() && HttpResponse->GetResponseCode() == 200)
			{
				ResponseData.SetFromHttpResponse(HttpResponse);
				ResponseData.PrintData();
				InDelegate.ExecuteIfBound(ResponseData, true);
			}
			else
			{
				NETWORK_LOG(TEXT("[POST] QuestSpeak failed"));
				InDelegate.ExecuteIfBound(ResponseData, false);
			}
		});

	AddNetworkWaitCount(1);
	Request->ProcessRequest();
}

// =================================================================================
// RequestGameResult
// =================================================================================

void UKLingoNetworkSystem::RequestGameResult(FResponseGameResultDelegate InDelegate)
{
	FString Url = NetworkConfig::GetFullUrl(RequestAPI::GameResult);

	auto Request = SetupHttpRequest( Url, NETWORK_GET );

	LogNetwork(ENetworkLogType::Get, *Request->GetURL());
	
	Request->OnProcessRequestComplete().BindLambda(
		[this, InDelegate](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSuccess)
		{
			AddNetworkWaitCount(-1);

			FResponseGameResult ResponseData;

			if (bSuccess && HttpResponse.IsValid() && HttpResponse->GetResponseCode() == 200)
			{
				ResponseData.SetFromHttpResponse(HttpResponse);
				ResponseData.PrintData();
				InDelegate.ExecuteIfBound(ResponseData, true);
			}
			else
			{
				NETWORK_LOG(TEXT("[GET] GameResult failed"));
				InDelegate.ExecuteIfBound(ResponseData, false);
			}
		});

	AddNetworkWaitCount(1);
	Request->ProcessRequest();
}
*/
#pragma endregion READY