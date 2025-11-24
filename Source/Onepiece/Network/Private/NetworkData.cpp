// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file NetworkData.cpp
 * @brief FResponseHealth의 동작을 구현합니다.
 */
#include "NetworkData.h"

#include "JsonObjectConverter.h"
#include "NetworkLog.h"
#include "UCommonFunctionLibrary.h"
#include "Misc/Base64.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

void FResponseHealth::SetFromHttpResponse(const TSharedPtr<IHttpResponse, ESPMode::ThreadSafe>& Response)
{
    if (Response.IsValid())
    {
        status = Response->GetResponseCode();
    }
}

void FResponseHealth::PrintData()
{
    FString OutputString;
    FJsonObjectConverter::UStructToJsonObjectString(
        *this,
        OutputString,
        0,
        0
    );
    NETWORK_LOG( TEXT("[RES] %s"), *OutputString);
}




void FResponseUserRegister::SetFromHttpResponse(const TSharedPtr<IHttpResponse, ESPMode::ThreadSafe>& Response)
{
	if (!Response.IsValid())
	{
		return;
	}

	FString JsonString = Response->GetContentAsString();
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		id = JsonObject->GetIntegerField(TEXT("id"));
		username = JsonObject->GetStringField(TEXT("username"));
		email = JsonObject->GetStringField(TEXT("email"));
		is_active = JsonObject->GetBoolField(TEXT("is_active"));
	}
}

void FResponseUserRegister::PrintData() const
{
	FString OutputString;
	FJsonObjectConverter::UStructToJsonObjectString(
		*this,
		OutputString,
		0,
		0
	);
	NETWORK_LOG( TEXT("[RES] %s"), *OutputString);
}


void FResponseUserToken::SetFromHttpResponse(const TSharedPtr<IHttpResponse, ESPMode::ThreadSafe>& Response)
{
	if (!Response.IsValid())
	{
		return;
	}

	FString JsonString = Response->GetContentAsString();
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		access_token = JsonObject->GetStringField(TEXT("access_token"));
	}
}

void FResponseUserToken::PrintData() const
{
	FString OutputString;
	FJsonObjectConverter::UStructToJsonObjectString(
		*this,
		OutputString,
		0,
		0
	);
	NETWORK_LOG( TEXT("[RES] %s"), *OutputString);
}



void FResponseUserMe::SetFromHttpResponse(const TSharedPtr<IHttpResponse, ESPMode::ThreadSafe>& Response)
{
	if (!Response.IsValid())
	{
		return;
	}

	FString JsonString = Response->GetContentAsString();
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		detail = JsonObject->GetStringField(TEXT("detail"));
	}
}

void FResponseUserMe::PrintData() const
{
	FString OutputString;
	FJsonObjectConverter::UStructToJsonObjectString(
		*this,
		OutputString,
		0,
		0
	);
	NETWORK_LOG( TEXT("[RES] %s"), *OutputString);
}

// =================================================================================
// FResponseScenario
// =================================================================================

void FResponseScenario::SetFromHttpResponse(const TSharedPtr<IHttpResponse, ESPMode::ThreadSafe>& Response)
{
	if (!Response.IsValid())
	{
		return;
	}

	FString JsonString = Response->GetContentAsString();
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		index = JsonObject->GetIntegerField(TEXT("index"));
		dificulity = JsonObject->GetIntegerField(TEXT("dificulity"));
		correct_answer_index = JsonObject->GetIntegerField(TEXT("correct_answer_index"));

		// target_data 배열 파싱
		const TArray<TSharedPtr<FJsonValue>>* TargetDataArray;
		if (JsonObject->TryGetArrayField(TEXT("target_data"), TargetDataArray))
		{
			for (const auto& Item : *TargetDataArray)
			{
				TSharedPtr<FJsonObject> TargetObj = Item->AsObject();
				if (TargetObj.IsValid())
				{
					FScenarioTargetData TargetItem;
					TargetItem.symbol = TargetObj->GetStringField(TEXT("symbol"));
					TargetItem.color = TargetObj->GetStringField(TEXT("color"));
					target_data.Add(TargetItem);
				}
			}
		}

		// word_data1 파싱
		if (JsonObject->HasTypedField<EJson::Object>(TEXT("word_data1")))
		{
			TSharedPtr<FJsonObject> WordData1Obj = JsonObject->GetObjectField(TEXT("word_data1"));
			FJsonObjectConverter::JsonObjectToUStruct(WordData1Obj.ToSharedRef(), FWordData::StaticStruct(), &word_data1);
		}

		// word_data2 파싱
		if (JsonObject->HasTypedField<EJson::Object>(TEXT("word_data2")))
		{
			TSharedPtr<FJsonObject> WordData2Obj = JsonObject->GetObjectField(TEXT("word_data2"));
			FJsonObjectConverter::JsonObjectToUStruct(WordData2Obj.ToSharedRef(), FWordData::StaticStruct(), &word_data2);
		}

		// full_data 파싱
		if (JsonObject->HasTypedField<EJson::Object>(TEXT("full_data")))
		{
			TSharedPtr<FJsonObject> FullDataObj = JsonObject->GetObjectField(TEXT("full_data"));
			FJsonObjectConverter::JsonObjectToUStruct(FullDataObj.ToSharedRef(), FWordData::StaticStruct(), &full_data);
		}
	}
}

void FResponseScenario::PrintData() const
{
	// NETWORK_LOG( TEXT("[Scenario] Response - Index: %d, Difficulty: %d, Targets: %d, Correct: %d"),
	// 	index, dificulity, target_data.Num(), correct_answer_index);

	FString OutputString;
	FJsonObjectConverter::UStructToJsonObjectString(
		*this,
		OutputString,
		0,
		0
	);
	NETWORK_LOG( TEXT("[RES] %s"), *OutputString);
	
}

// =================================================================================
// FResponseOcrExtract
// =================================================================================

void FResponseOcrExtract::SetFromHttpResponse(const TSharedPtr<IHttpResponse, ESPMode::ThreadSafe>& Response)
{
	if (!Response.IsValid())
	{
		return;
	}

	FString JsonString = Response->GetContentAsString();
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		success = JsonObject->GetBoolField(TEXT("success"));
		extracted_text = JsonObject->GetStringField(TEXT("extracted_text"));
	}
}

void FResponseOcrExtract::PrintData() const
{
	NETWORK_LOG( TEXT("[OCR Extract] Response - Success: %d, Text: %s"), success, *extracted_text);
}

// =================================================================================
// FResponseSpeakingQuestions
// =================================================================================

void FResponseSpeakingQuestions::SetFromHttpResponse(const TSharedPtr<IHttpResponse, ESPMode::ThreadSafe>& Response)
{
	if (!Response.IsValid())
	{
		return;
	}

	FString JsonString = Response->GetContentAsString();
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		answer = JsonObject->GetStringField(TEXT("answer"));
	}
}

void FResponseSpeakingQuestions::PrintData() const
{
	NETWORK_LOG( TEXT("[Speaking Questions] Response - Answer: %s"), *answer);
}

/*

// =================================================================================
// FResponseLogin
// =================================================================================

void FResponseLogin::SetFromHttpResponse(const TSharedPtr<IHttpResponse, ESPMode::ThreadSafe>& Response)
{
	if (!Response.IsValid())
	{
		return;
	}

	FString JsonString = Response->GetContentAsString();
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		Token = JsonObject->GetStringField(TEXT("token"));
		if (JsonObject->HasTypedField<EJson::Object>(TEXT("playerInfo")))
		{
			TSharedPtr<FJsonObject> PlayerInfoObj = JsonObject->GetObjectField(TEXT("playerInfo"));
			FJsonObjectConverter::JsonObjectToUStruct(PlayerInfoObj.ToSharedRef(), FPlayerInfo::StaticStruct(), &PlayerInfo);
		}
	}
}

void FResponseLogin::PrintData() const
{
	NETWORK_LOG( TEXT("[KLingo] Login Response - Token: %s, Nickname: %s"), *Token, *PlayerInfo.Nickname);
}

// =================================================================================
// FResponseCreateUser
// =================================================================================

void FResponseCreateUser::SetFromHttpResponse(const TSharedPtr<IHttpResponse, ESPMode::ThreadSafe>& Response)
{
	if (!Response.IsValid())
	{
		return;
	}

	FString JsonString = Response->GetContentAsString();
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		bSuccess = JsonObject->GetBoolField(TEXT("success"));
		Message = JsonObject->GetStringField(TEXT("message"));
	}
}

void FResponseCreateUser::PrintData() const
{
	NETWORK_LOG( TEXT("[KLingo] CreateUser Response - Success: %d, Message: %s"), bSuccess, *Message);
}

// =================================================================================
// FResponseInterview
// =================================================================================

void FResponseInterview::SetFromHttpResponse(const TSharedPtr<IHttpResponse, ESPMode::ThreadSafe>& Response)
{
	if (!Response.IsValid())
	{
		return;
	}

	FString JsonString = Response->GetContentAsString();
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		const TArray<TSharedPtr<FJsonValue>>* QuestionsArray;
		if (JsonObject->TryGetArrayField(TEXT("questions"), QuestionsArray))
		{
			for (const auto& Item : *QuestionsArray)
			{
				Questions.Add(Item->AsString());
			}
		}
	}
}

void FResponseInterview::PrintData() const
{
	NETWORK_LOG( TEXT("[KLingo] Interview Response - Questions Count: %d"), Questions.Num());
}

// =================================================================================
// FResponseStartGame
// =================================================================================

void FResponseStartGame::SetFromHttpResponse(const TSharedPtr<IHttpResponse, ESPMode::ThreadSafe>& Response)
{
	if (!Response.IsValid())
	{
		return;
	}

	FString JsonString = Response->GetContentAsString();
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		if (JsonObject->HasTypedField<EJson::Object>(TEXT("questRead")))
		{
			TSharedPtr<FJsonObject> QuestReadObj = JsonObject->GetObjectField(TEXT("questRead"));
			FJsonObjectConverter::JsonObjectToUStruct(QuestReadObj.ToSharedRef(), FQuestReadInfo::StaticStruct(), &QuestRead);
		}

		if (JsonObject->HasTypedField<EJson::Object>(TEXT("questListen")))
		{
			TSharedPtr<FJsonObject> QuestListenObj = JsonObject->GetObjectField(TEXT("questListen"));
			FJsonObjectConverter::JsonObjectToUStruct(QuestListenObj.ToSharedRef(), FQuestListenInfo::StaticStruct(), &QuestListen);
		}

		if (JsonObject->HasTypedField<EJson::Object>(TEXT("questWrite")))
		{
			TSharedPtr<FJsonObject> QuestWriteObj = JsonObject->GetObjectField(TEXT("questWrite"));
			FJsonObjectConverter::JsonObjectToUStruct(QuestWriteObj.ToSharedRef(), FQuestWriteInfo::StaticStruct(), &QuestWrite);
		}

		if (JsonObject->HasTypedField<EJson::Object>(TEXT("questSpeak")))
		{
			TSharedPtr<FJsonObject> QuestSpeakObj = JsonObject->GetObjectField(TEXT("questSpeak"));
			FJsonObjectConverter::JsonObjectToUStruct(QuestSpeakObj.ToSharedRef(), FQuestSpeakInfo::StaticStruct(), &QuestSpeak);
		}
	}
}

void FResponseStartGame::PrintData() const
{
	NETWORK_LOG( TEXT("[KLingo] StartGame Response - Read: %d, Listen: %d, Write: %d, Speak: %d"),
		QuestRead.ReadIndex, QuestListen.ListenIndex, QuestWrite.WriteIndex, QuestSpeak.SpeakIndex);
}

// =================================================================================
// FResponseGameLogin
// =================================================================================

void FResponseGameLogin::SetFromHttpResponse(const TSharedPtr<IHttpResponse, ESPMode::ThreadSafe>& Response)
{
	if (!Response.IsValid())
	{
		return;
	}

	FString JsonString = Response->GetContentAsString();
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		bSuccess = JsonObject->GetBoolField(TEXT("success"));
		SessionToken = JsonObject->GetStringField(TEXT("sessionToken"));
	}
}

void FResponseGameLogin::PrintData() const
{
	NETWORK_LOG( TEXT("[KLingo] GameLogin Response - Success: %d, Token: %s"), bSuccess, *SessionToken);
}

// =================================================================================
// FResponseQuestAnswer
// =================================================================================

void FResponseQuestAnswer::SetFromHttpResponse(const TSharedPtr<IHttpResponse, ESPMode::ThreadSafe>& Response)
{
	if (!Response.IsValid())
	{
		return;
	}

	FString JsonString = Response->GetContentAsString();
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		Result = JsonObject->GetIntegerField(TEXT("result"));
		TeachString = JsonObject->GetStringField(TEXT("teachString"));
		FailCount = JsonObject->GetIntegerField(TEXT("failCount"));
	}
}

void FResponseQuestAnswer::PrintData() const
{
	NETWORK_LOG( TEXT("[KLingo] QuestAnswer Response - Result: %d, FailCount: %d"), Result, FailCount);
}

// =================================================================================
// FResponseQuestWrite
// =================================================================================

void FResponseQuestWrite::SetFromHttpResponse(const TSharedPtr<IHttpResponse, ESPMode::ThreadSafe>& Response)
{
	if (!Response.IsValid())
	{
		return;
	}

	FString JsonString = Response->GetContentAsString();
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		bSuccess = JsonObject->GetBoolField(TEXT("success"));

		const TArray<TSharedPtr<FJsonValue>>* TeachDataArray;
		if (JsonObject->TryGetArrayField(TEXT("teachData"), TeachDataArray))
		{
			for (const auto& Item : *TeachDataArray)
			{
				TSharedPtr<FJsonObject> TeachObj = Item->AsObject();
				if (TeachObj.IsValid())
				{
					FWriteTeachData TeachItem;
					TeachItem.Index = TeachObj->GetIntegerField(TEXT("index"));
					TeachItem.TeachString = TeachObj->GetStringField(TEXT("teachString"));
					TeachData.Add(TeachItem);
				}
			}
		}
	}
}

void FResponseQuestWrite::PrintData() const
{
	NETWORK_LOG( TEXT("[KLingo] QuestWrite Response - Success: %d, TeachData Count: %d"), bSuccess, TeachData.Num());
}

// =================================================================================
// FResponseQuestSpeak
// =================================================================================

void FResponseQuestSpeak::SetFromHttpResponse(const TSharedPtr<IHttpResponse, ESPMode::ThreadSafe>& Response)
{
	if (!Response.IsValid())
	{
		return;
	}

	FString JsonString = Response->GetContentAsString();
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		bSuccess = JsonObject->GetBoolField(TEXT("success"));
		Feedback = JsonObject->GetStringField(TEXT("feedback"));
		Score = JsonObject->GetIntegerField(TEXT("score"));
	}
}

void FResponseQuestSpeak::PrintData() const
{
	NETWORK_LOG( TEXT("[KLingo] QuestSpeak Response - Success: %d, Score: %d"), bSuccess, Score);
}

// =================================================================================
// FResponseGameResult
// =================================================================================

void FResponseGameResult::SetFromHttpResponse(const TSharedPtr<IHttpResponse, ESPMode::ThreadSafe>& Response)
{
	if (!Response.IsValid())
	{
		return;
	}

	FString JsonString = Response->GetContentAsString();
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		Grade = JsonObject->GetStringField(TEXT("grade"));

		const TArray<TSharedPtr<FJsonValue>>* ScoresArray;
		if (JsonObject->TryGetArrayField(TEXT("scores"), ScoresArray))
		{
			for (const auto& Item : *ScoresArray)
			{
				Scores.Add(Item->AsNumber());
			}
		}
	}
}

void FResponseGameResult::PrintData() const
{
	NETWORK_LOG( TEXT("[KLingo] GameResult Response - Grade: %s, Scores Count: %d"), *Grade, Scores.Num());
}


TSharedPtr<FJsonObject> FGPTContext::ToJsonObject() const
{
	return MakeShared<FJsonObject>();
}

// --- Ask Endpoint Implementation ---
bool FRequestASK::ToJsonString(FString& OutJson) const
{
	TSharedPtr<FJsonObject> Root = MakeShared<FJsonObject>();

	if (TSharedPtr<FJsonObject> ContextJson = context.ToJsonObject())
		Root->SetObjectField(TEXT("context"), ContextJson);

	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutJson);
	return FJsonSerializer::Serialize(Root.ToSharedRef(), Writer);
}

void FResponseAsk::SetFromHttpResponse(const TSharedPtr<IHttpResponse, ESPMode::ThreadSafe>& Response)
{
	if (!Response.IsValid())
	{
		return;
	}

	FString ResponseBody = Response->GetContentAsString();

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseBody);

	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		JsonObject->TryGetStringField(TEXT("transcribed_text"), transcribed_text);
		JsonObject->TryGetStringField(TEXT("gpt_response_text"), gpt_response_text);

		gpt_response_text = UCommonFunctionLibrary::RemoveLineBreaks(gpt_response_text);

		FString audio_content;
		JsonObject->TryGetStringField(TEXT("audio_content"), audio_content);
		FBase64::Decode(audio_content, audio_data);
	}
}

void FResponseAsk::PrintData()
{
	FString OutputString;
	FJsonObjectConverter::UStructToJsonObjectString(
		*this,
		OutputString,
		0,
		0
	);
	NETWORK_LOG( TEXT("[RES] %s"), *OutputString);
}
*/