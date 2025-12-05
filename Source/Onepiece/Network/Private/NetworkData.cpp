// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file NetworkData.cpp
 * @brief FResponseHealth의 동작을 구현합니다.
 */
#include "NetworkData.h"

#include "ALingoGameState.h"
#include "GameLogging.h"
#include "JsonObjectConverter.h"
#include "NetworkLog.h"
#include "FResultStatData.h"
#include "ULingoGameHelper.h"

#include "Misc/Base64.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"


FString FPhonemeData::ToRichTextString(int32 Index) const
{
	return FString::Printf(TEXT("<a id=\"%d\" content=\"link\">%s</> "), Index, *Kor);
}

TArray<FPhonemeData> FWordData::GetPhonemeData() const
{
	TArray<FString> KorWords;
	Kor.ParseIntoArray(KorWords, TEXT(" "), true);

	TArray<FString> PronWords;
	Pronunciation.ParseIntoArray(PronWords, TEXT(" "), true);

	TArray<FPhonemeData> WordDataArray;

	for (int32 i = 0; i < KorWords.Num(); ++i)
	{
		FPhonemeData Data;
		Data.Kor = i < KorWords.Num() ? KorWords[i] : TEXT("");
		Data.Pronunciation = i < PronWords.Num() ? PronWords[i] : TEXT("");

		WordDataArray.Add(Data);
	}

	return WordDataArray;
}



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
		id = JsonObject->GetIntegerField(TEXT("id"));
		username = JsonObject->GetStringField(TEXT("username"));
		fullname = JsonObject->GetStringField(TEXT("fullname"));
		is_active = JsonObject->GetBoolField(TEXT("is_active"));
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


void FResponseUserHost::SetFromHttpResponse(const TSharedPtr<IHttpResponse, ESPMode::ThreadSafe>& Response)
{
	return;
}

void FResponseUserHost::PrintData() const
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

					// word1 파싱                                                                                                                                                                                                         
					if (TargetObj->HasTypedField<EJson::Object>(TEXT("word1")))
					{
						TSharedPtr<FJsonObject> Word1Obj = TargetObj->GetObjectField(TEXT("word1"));
						if (Word1Obj.IsValid())
						{
							TargetItem.word1.name = Word1Obj->GetStringField(TEXT("name"));
							TargetItem.word1.code = Word1Obj->GetStringField(TEXT("code"));
						}
					}

					// word2 파싱                                                                                                                                                           
					if (TargetObj->HasTypedField<EJson::Object>(TEXT("word2")))
					{
						TSharedPtr<FJsonObject> Word2Obj = TargetObj->GetObjectField(TEXT("word2"));
						if (Word2Obj.IsValid())
						{
							TargetItem.word2.name = Word2Obj->GetStringField(TEXT("name"));
							TargetItem.word2.code = Word2Obj->GetStringField(TEXT("code"));
						}
					}

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

		// voice data 파싱
		FString VoiceDataString;
		if (JsonObject->TryGetStringField(TEXT("voice_data"), VoiceDataString))
		{
			FBase64::Decode(VoiceDataString, voice_data);
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

TArray<FString> FResponseScenario::GetWord1List() const
{
	TSet<FString> UniqueSet;

	for (const FScenarioTargetData& TargetData : target_data)
	{
		if (!TargetData.word1.name.IsEmpty())
		{
			UniqueSet.Add(TargetData.word1.name);
		}
	}

	return UniqueSet.Array();
}

TArray<FString> FResponseScenario::GetWord2List() const
{
	TSet<FString> UniqueSet;

	for (const FScenarioTargetData& TargetData : target_data)
	{
		if (!TargetData.word2.name.IsEmpty())
		{
			UniqueSet.Add(TargetData.word2.name);
		}
	}

	return UniqueSet.Array();
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
	PRINTLOG(TEXT("hmm.... %s"), *JsonString);
	TArray<TSharedPtr<FJsonValue>> JsonArray;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if (FJsonSerializer::Deserialize(Reader, JsonArray))
	{
		for (const TSharedPtr<FJsonValue>& EntryValue : JsonArray)
		{
			if (EntryValue.IsValid())
			{
				TSharedPtr<FJsonObject> EntryObject = EntryValue->AsObject();
				if (!EntryObject.IsValid())
				{
					continue;
				}

				FResponseOcrData Entry;    // Display & Record를 포함하는 사용자 정의 구조체

				if (EntryObject->HasTypedField<EJson::Object>(TEXT("display")))
				{
					const TSharedPtr<FJsonObject> DisplayObj = EntryObject->GetObjectField(TEXT("display"));
					Entry.display.is_pass    = DisplayObj->GetBoolField(TEXT("is_pass"));
					Entry.display.message    = DisplayObj->GetStringField(TEXT("message"));
					Entry.display.correction = DisplayObj->GetStringField(TEXT("correction"));
				}

				if (EntryObject->HasTypedField<EJson::Object>(TEXT("record")))
				{
					const TSharedPtr<FJsonObject> RecordObj = EntryObject->GetObjectField(TEXT("record"));
					Entry.record.score  = RecordObj->GetIntegerField(TEXT("score"));
					Entry.record.target = RecordObj->GetStringField(TEXT("target"));
					Entry.record.input  = RecordObj->GetStringField(TEXT("input"));
					Entry.record.stage  = RecordObj->GetStringField(TEXT("stage"));
				}

				ResponseOcrDataArray.Add(Entry);
			}
		}
	}
}

void FResponseOcrExtract::PrintData() const
{
	for (const FResponseOcrData& data : ResponseOcrDataArray)
	{
		NETWORK_LOG( TEXT("[OCR Extract] Response - Is_Pass: %d, Text: %s"), data.display.is_pass, *(data.display.message));
	}
}

void FResponseListenAudio::SetFromHttpResponse(const TSharedPtr<class IHttpResponse, ESPMode::ThreadSafe>& Response)
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
		JsonObject->TryGetStringField(TEXT("audio_text"), audio_text);

		FString audio_data;
		JsonObject->TryGetStringField(TEXT("audio_base64"), audio_data);
		FBase64::Decode(audio_data, audio_base64);
	}
}

void FResponseListenAudio::PrintData() const
{
	// FString OutputString;
	// FJsonObjectConverter::UStructToJsonObjectString(
	// 	*this,
	// 	OutputString,
	// 	0,
	// 	0
	// );
	// NETWORK_LOG( TEXT("[RES] %s"), *OutputString);

	NETWORK_LOG( TEXT("[RES] audio_text: %s"), *audio_text );
}

// =================================================================================
// FResponseSpeakingQuestions
// =================================================================================

void FResponseSpeakingJudes::SetFromHttpResponse(const TSharedPtr<IHttpResponse, ESPMode::ThreadSafe>& Response)
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
		grammar_score = JsonObject->GetIntegerField(TEXT("grammar_score"));
		context_score = JsonObject->GetIntegerField(TEXT("context_score"));
		final_overall_score = JsonObject->GetIntegerField(TEXT("final_overall_score"));
		final_feedback = JsonObject->GetStringField(TEXT("final_feedback"));
	}
}

void FResponseSpeakingJudes::PrintData() const
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

TArray<FResultStatData> FResponseSpeakingJudes::GetResultStatData()
{
	TArray<FResultStatData> StatDataList;

	// Grammar Score
	FResultStatData GrammarData;
	GrammarData.WidgetType = EResultItemWidgetType::Grade;
	GrammarData.ColorType = EColorStyleType::Green;
	GrammarData.TitleText = FText::FromString(TEXT("GRAMMER"));
	GrammarData.ScoreValue = grammar_score;
	GrammarData.GradeTextureType = ULingoGameHelper::ConvertGradeType(grammar_score);
	StatDataList.Add(GrammarData);

	// Context Score
	FResultStatData ContextData;
	ContextData.WidgetType = EResultItemWidgetType::Grade;
	ContextData.ColorType = EColorStyleType::Blue;
	ContextData.TitleText = FText::FromString(TEXT("CONTEXT"));
	ContextData.ScoreValue = context_score;
	ContextData.GradeTextureType = ULingoGameHelper::ConvertGradeType(context_score);
	StatDataList.Add(ContextData);

	// Final Overall Score
	FResultStatData OverallData;
	OverallData.WidgetType = EResultItemWidgetType::Grade;
	OverallData.ColorType = EColorStyleType::Yellow;
	OverallData.TitleText = FText::FromString(TEXT("SCORE"));
	OverallData.ScoreValue = final_overall_score;
	OverallData.GradeTextureType = ULingoGameHelper::ConvertGradeType(final_overall_score);
	StatDataList.Add(OverallData);

	return StatDataList;
}


void FResponseInterviewHello::SetFromHttpResponse(const TSharedPtr<IHttpResponse, ESPMode::ThreadSafe>& Response)
{
	if (!Response.IsValid())
	{
		return;
	}

	FString JsonString = Response->GetContentAsString();
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
	TArray<TSharedPtr<FJsonValue>> JsonArray;

	if (FJsonSerializer::Deserialize(Reader, JsonArray))
	{
		Questions.Empty();

		for (const auto& JsonValue : JsonArray)
		{
			TSharedPtr<FJsonObject> JsonObject = JsonValue->AsObject();
			if (JsonObject.IsValid())
			{
				FInterviewQuestionData QuestionData;
				QuestionData.Id = JsonObject->GetIntegerField(TEXT("id"));
				QuestionData.TypeCode = JsonObject->GetIntegerField(TEXT("type_code"));
				QuestionData.Eng = JsonObject->GetStringField(TEXT("eng"));
				QuestionData.Kor = JsonObject->GetStringField(TEXT("kor"));
				QuestionData.EngKey = JsonObject->GetStringField(TEXT("eng_key"));
				QuestionData.KorKey = JsonObject->GetStringField(TEXT("kor_key"));

				// created_at 필드 파싱 (optional)
				JsonObject->TryGetStringField(TEXT("created_at"), QuestionData.CreatedAt);

				Questions.Add(QuestionData);
			}
		}

		NETWORK_LOG(TEXT("[Interview Hello] Successfully parsed %d questions"), Questions.Num());
	}
	else
	{
		NETWORK_LOG(TEXT("[Interview Hello] Failed to parse JSON array"));
	}
}

void FResponseInterviewHello::PrintData() const
{
	NETWORK_LOG(TEXT("[Interview Hello] Response - Questions Count: %d"), Questions.Num());
	for (const auto& Question : Questions)
	{
		NETWORK_LOG(TEXT("  - ID: %d, TypeCode: %d, Eng: %s, Kor: %s"),
			Question.Id, Question.TypeCode, *Question.Eng, *Question.Kor);
	}
}

// =================================================================================
// FRequestInterviewAnswer
// =================================================================================

bool FRequestInterviewAnswer::ToJsonString(FString& OutJson) const
{
	TArray<TSharedPtr<FJsonValue>> JsonArray;

	for (const FInterviewAnswerData& AnswerData : answer)
	{
		TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
		JsonObject->SetNumberField(TEXT("interview_id"), AnswerData.interview_id);
		JsonObject->SetStringField(TEXT("answer"), AnswerData.answer);
		JsonObject->SetNumberField(TEXT("user_id"), AnswerData.user_id);

		JsonArray.Add(MakeShared<FJsonValueObject>(JsonObject));
	}

	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutJson);
	return FJsonSerializer::Serialize(JsonArray, Writer);
}

// =================================================================================
// FResponseInterviewAnswer
// =================================================================================

void FResponseInterviewAnswer::SetFromHttpResponse(const TSharedPtr<IHttpResponse, ESPMode::ThreadSafe>& Response)
{
	if (!Response.IsValid())
	{
		return;
	}

	FString JsonString = Response->GetContentAsString();
	NETWORK_LOG(TEXT("[Interview Answer] Response: %s"), *JsonString);
}

void FResponseInterviewAnswer::PrintData() const
{
	NETWORK_LOG(TEXT("[Interview Answer] Response processed successfully"));
}

// =================================================================================
// Read & Listen Result API Structures
// =================================================================================

bool FRequestReadQuestResult::ToJsonString(FString& OutJson) const
{
	TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
	
	JsonObject->SetNumberField(TEXT("user_id"), user_id);
	JsonObject->SetNumberField(TEXT("scenario_id"), scenario_id);
	JsonObject->SetNumberField(TEXT("stage_type"), stage_type);
	JsonObject->SetNumberField(TEXT("state_type"), state_type);
	JsonObject->SetNumberField(TEXT("result_time"), result_time);

	TArray<TSharedPtr<FJsonValue>> WrongIdxArray;
	for (int32 Idx : wrong_idx)
	{
		WrongIdxArray.Add(MakeShared<FJsonValueNumber>(Idx));
	}
	JsonObject->SetArrayField(TEXT("wrong_idx"), WrongIdxArray);

	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutJson);
	return FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
}

void FResponseQuestResult::SetFromHttpResponse(const TSharedPtr<class IHttpResponse, ESPMode::ThreadSafe>& Response)
{
	if (Response.IsValid())
	{
		return;
	}

	FString JsonString = Response->GetContentAsString();
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		grade = JsonObject->GetStringField(TEXT("grade"));
		point = JsonObject->GetNumberField(TEXT("point"));
		top_percent = JsonObject->GetNumberField(TEXT("top_percent"));
	}
}

void FResponseQuestResult::PrintData() const
{
	FString OutputString;
	FJsonObjectConverter::UStructToJsonObjectString(
		*this,
		OutputString,
		0,
		0                                                                                                                                                                                                                         
	);
	NETWORK_LOG(TEXT("[Quest Result] Response: %s"), *OutputString);
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