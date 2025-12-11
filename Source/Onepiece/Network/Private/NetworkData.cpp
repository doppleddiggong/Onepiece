// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file NetworkData.cpp
 * @brief FResponseHealth의 동작을 구현합니다.
 */
#include "NetworkData.h"

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

void FQuestWriteInfo::SetFromHttpResponse(const TSharedPtr<class IHttpResponse, ESPMode::ThreadSafe>& Response)
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
		user_id = JsonObject->GetIntegerField(TEXT("user_id"));
		
		TArray<TSharedPtr<FJsonValue>> questionArray = JsonObject->GetArrayField(TEXT("question"));
		
		for (const auto& data : questionArray)
		{
			FWriteQuestionData temp;
			TSharedPtr<FJsonObject> dataObject = data->AsObject();
			if (!dataObject.IsValid())
			{
				continue;
			}
			
			if (dataObject->HasTypedField<EJson::Object>(TEXT("word_data")))
			{
				const TSharedPtr<FJsonObject> DisplayObj = dataObject->GetObjectField(TEXT("word_data"));
				temp.word_data.kor = DisplayObj->GetStringField(TEXT("kor"));
				temp.word_data.eng = DisplayObj->GetStringField(TEXT("eng"));
				temp.word_data.pronunciation = DisplayObj->GetStringField(TEXT("pronunciation"));
			}
			
			temp.answer = dataObject->GetStringField(TEXT("answer"));
			temp.answer_kor = dataObject->GetStringField(TEXT("answer_kor"));
			question.Add(temp);
		}
	}
	bIsValid = true;
}

void FQuestWriteInfo::PrintData() const
{
	
}

bool FQuestWriteInfo::IsValid() const
{
	return bIsValid;
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


//
// // =================================================================================
// // FResponseScenario
// // =================================================================================
//
// void FResponseScenario::SetFromHttpResponse(const TSharedPtr<IHttpResponse, ESPMode::ThreadSafe>& Response)
// {
// 	if (!Response.IsValid())
// 	{
// 		return;
// 	}
//
// 	FString JsonString = Response->GetContentAsString();
// 	TSharedPtr<FJsonObject> JsonObject;
// 	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
//
// 	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
// 	{
// 		index = JsonObject->GetIntegerField(TEXT("index"));
// 		dificulity = JsonObject->GetIntegerField(TEXT("dificulity"));
// 		correct_answer_index = JsonObject->GetIntegerField(TEXT("correct_answer_index"));
//
// 		// target_data 배열 파싱
// 		const TArray<TSharedPtr<FJsonValue>>* TargetDataArray;
// 		if (JsonObject->TryGetArrayField(TEXT("target_data"), TargetDataArray))
// 		{
// 			for (const auto& Item : *TargetDataArray)
// 			{
// 				TSharedPtr<FJsonObject> TargetObj = Item->AsObject();
// 				if (TargetObj.IsValid())
// 				{
// 					FScenarioTargetData TargetItem;
//
// 					// word1 파싱                                                                                                                                                                                                         
// 					if (TargetObj->HasTypedField<EJson::Object>(TEXT("word1")))
// 					{
// 						TSharedPtr<FJsonObject> Word1Obj = TargetObj->GetObjectField(TEXT("word1"));
// 						if (Word1Obj.IsValid())
// 						{
// 							TargetItem.word1.name = Word1Obj->GetStringField(TEXT("name"));
// 							TargetItem.word1.code = Word1Obj->GetStringField(TEXT("code"));
// 						}
// 					}
//
// 					// word2 파싱                                                                                                                                                           
// 					if (TargetObj->HasTypedField<EJson::Object>(TEXT("word2")))
// 					{
// 						TSharedPtr<FJsonObject> Word2Obj = TargetObj->GetObjectField(TEXT("word2"));
// 						if (Word2Obj.IsValid())
// 						{
// 							TargetItem.word2.name = Word2Obj->GetStringField(TEXT("name"));
// 							TargetItem.word2.code = Word2Obj->GetStringField(TEXT("code"));
// 						}
// 					}
//
// 					target_data.Add(TargetItem);
// 				}
// 			}
// 		}
//
// 		// word_data1 파싱
// 		if (JsonObject->HasTypedField<EJson::Object>(TEXT("word_data1")))
// 		{
// 			TSharedPtr<FJsonObject> WordData1Obj = JsonObject->GetObjectField(TEXT("word_data1"));
// 			FJsonObjectConverter::JsonObjectToUStruct(WordData1Obj.ToSharedRef(), FWordData::StaticStruct(), &word_data1);
// 		}
//
// 		// word_data2 파싱
// 		if (JsonObject->HasTypedField<EJson::Object>(TEXT("word_data2")))
// 		{
// 			TSharedPtr<FJsonObject> WordData2Obj = JsonObject->GetObjectField(TEXT("word_data2"));
// 			FJsonObjectConverter::JsonObjectToUStruct(WordData2Obj.ToSharedRef(), FWordData::StaticStruct(), &word_data2);
// 		}
//
// 		// full_data 파싱
// 		if (JsonObject->HasTypedField<EJson::Object>(TEXT("full_data")))
// 		{
// 			TSharedPtr<FJsonObject> FullDataObj = JsonObject->GetObjectField(TEXT("full_data"));
// 			FJsonObjectConverter::JsonObjectToUStruct(FullDataObj.ToSharedRef(), FWordData::StaticStruct(), &full_data);
// 		}
//
// 		// voice data 파싱
// 		FString VoiceDataString;
// 		if (JsonObject->TryGetStringField(TEXT("voice_data"), VoiceDataString))
// 		{
// 			FBase64::Decode(VoiceDataString, voice_data);
// 		}
// 	}
// }
//
// void FResponseScenario::PrintData() const
// {
// 	FString OutputString;
// 	FJsonObjectConverter::UStructToJsonObjectString(
// 		*this,
// 		OutputString,
// 		0,
// 		0
// 	);
// 	NETWORK_LOG( TEXT("[RES] %s"), *OutputString);
// }
//
// TArray<FString> FResponseScenario::GetWord1List() const
// {
// 	TSet<FString> UniqueSet;
//
// 	for (const FScenarioTargetData& TargetData : target_data)
// 	{
// 		if (!TargetData.word1.name.IsEmpty())
// 		{
// 			UniqueSet.Add(TargetData.word1.name);
// 		}
// 	}
//
// 	return UniqueSet.Array();
// }
//
// TArray<FString> FResponseScenario::GetWord2List() const
// {
// 	TSet<FString> UniqueSet;
//
// 	for (const FScenarioTargetData& TargetData : target_data)
// 	{
// 		if (!TargetData.word2.name.IsEmpty())
// 		{
// 			UniqueSet.Add(TargetData.word2.name);
// 		}
// 	}
//
// 	return UniqueSet.Array();
// }


// =================================================================================
// FResponseWriteSubmit
// =================================================================================

void FResponseWriteSubmit::SetFromHttpResponse(const TSharedPtr<IHttpResponse, ESPMode::ThreadSafe>& Response)
{
	if (!Response.IsValid())
	{
		return;
	}

	FString JsonString = Response->GetContentAsString();
	
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

				FResponseWriteData Entry;    // Display & Record를 포함하는 사용자 정의 구조체

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

				ResponseWriteDataArray.Add(Entry);
			}
		}
	}
}

void FResponseWriteSubmit::PrintData() const
{
	for (const FResponseWriteData& data : ResponseWriteDataArray)
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
	GrammarData.GradeTextureType = ULingoGameHelper::ConvertGradeScore(grammar_score);
	StatDataList.Add(GrammarData);

	// Context Score
	FResultStatData ContextData;
	ContextData.WidgetType = EResultItemWidgetType::Grade;
	ContextData.ColorType = EColorStyleType::Blue;
	ContextData.TitleText = FText::FromString(TEXT("CONTEXT"));
	ContextData.ScoreValue = context_score;
	ContextData.GradeTextureType = ULingoGameHelper::ConvertGradeScore(context_score);
	StatDataList.Add(ContextData);

	// Final Overall Score
	FResultStatData OverallData;
	OverallData.WidgetType = EResultItemWidgetType::Grade;
	OverallData.ColorType = EColorStyleType::Yellow;
	OverallData.TitleText = FText::FromString(TEXT("SCORE"));
	OverallData.ScoreValue = final_overall_score;
	OverallData.GradeTextureType = ULingoGameHelper::ConvertGradeScore(final_overall_score);
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
// FResponseReadScenario
// =================================================================================
void FResponseReadScenario::SetFromHttpResponse(const TSharedPtr<IHttpResponse, ESPMode::ThreadSafe>& Response)
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
		room_id = JsonObject->GetIntegerField(TEXT("room_id"));
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
	}
}

void FResponseReadScenario::PrintData() const
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

TArray<FString> FResponseReadScenario::GetWord1List() const
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

TArray<FString> FResponseReadScenario::GetWord2List() const
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

FScenarioTargetData FResponseReadScenario::GetCorrectAnswerData() const
{
	return target_data[correct_answer_index];
}

// =================================================================================
// FResponseReadResult
// =================================================================================
bool FRequestReadResult::ToJsonString(FString& OutJson) const
{
	TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();

	JsonObject->SetNumberField(TEXT("room_id"), room_id);
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

void FResponseReadResult::SetFromHttpResponse(const TSharedPtr<class IHttpResponse, ESPMode::ThreadSafe>& Response)
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
		average_score = JsonObject->GetIntegerField(TEXT("average_score"));
		top_percent = JsonObject->GetNumberField(TEXT("top_percent"));
	}
}

void FResponseReadResult::PrintData() const
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
// FResponseListenScenario
// =================================================================================

void FResponseListenScenario::SetFromHttpResponse(const TSharedPtr<IHttpResponse, ESPMode::ThreadSafe>& Response)
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
		room_id = JsonObject->GetIntegerField(TEXT("room_id"));
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

void FResponseListenScenario::PrintData() const
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

TArray<FString> FResponseListenScenario::GetWord1List() const
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

TArray<FString> FResponseListenScenario::GetWord2List() const
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

FScenarioTargetData FResponseListenScenario::GetCorrectAnswerData() const
{
	return target_data[correct_answer_index];
}

// =================================================================================
// FResponseListenResult
// =================================================================================
bool FRequestListenResult::ToJsonString(FString& OutJson) const
{
	TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();

	JsonObject->SetNumberField(TEXT("room_id"), room_id);
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

void FResponseListenResult::SetFromHttpResponse(const TSharedPtr<class IHttpResponse, ESPMode::ThreadSafe>& Response)
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
		average_score = JsonObject->GetIntegerField(TEXT("average_score"));
		top_percent = JsonObject->GetNumberField(TEXT("top_percent"));
	}
}

void FResponseListenResult::PrintData() const
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
// FResponseSpeakScenario
// =================================================================================

void FResponseSpeakScenario::SetFromHttpResponse(const TSharedPtr<IHttpResponse, ESPMode::ThreadSafe>& Response)
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
		dificulity = JsonObject->GetIntegerField(TEXT("difficulty"));
		room_id = JsonObject->GetIntegerField(TEXT("room_id"));

		// question 배열 파싱
		const TArray<TSharedPtr<FJsonValue>>* QuestionArray;
		if (JsonObject->TryGetArrayField(TEXT("question"), QuestionArray))
		{
			for (const auto& Item : *QuestionArray)
			{
				TSharedPtr<FJsonObject> QuestionObj = Item->AsObject();
				if (QuestionObj.IsValid())
				{
					FSpeakQuestionData QuestionItem;

					// word_data 파싱
					if (QuestionObj->HasTypedField<EJson::Object>(TEXT("word_data")))
					{
						TSharedPtr<FJsonObject> WordDataObj = QuestionObj->GetObjectField(TEXT("word_data"));
						FJsonObjectConverter::JsonObjectToUStruct(WordDataObj.ToSharedRef(), FWordData::StaticStruct(), &QuestionItem.word_data);
					}

					// answer 파싱
					QuestionItem.answer = QuestionObj->GetStringField(TEXT("answer"));

					// answer_kor 파싱
					QuestionItem.answer_kor = QuestionObj->GetStringField(TEXT("answer_kor"));

					question.Add(QuestionItem);
				}
			}
		}
	}
}

void FResponseSpeakScenario::PrintData() const
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
// FResponseEvaluationResult
// =================================================================================

void FResponseEvaluationResult::SetFromHttpResponse(const TSharedPtr<IHttpResponse, ESPMode::ThreadSafe>& Response)
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
		// total_result 파싱
		if (JsonObject->HasTypedField<EJson::Object>(TEXT("total_result")))
		{
			TSharedPtr<FJsonObject> TotalResultObj = JsonObject->GetObjectField(TEXT("total_result"));
			if (TotalResultObj.IsValid())
			{
				total_result.final_score = TotalResultObj->GetIntegerField(TEXT("final_score"));
				total_result.grade = TotalResultObj->GetStringField(TEXT("grade"));
				total_result.feedback_summary = TotalResultObj->GetStringField(TEXT("feedback_summary"));
			}
		}

		// scenario_results 배열 파싱
		const TArray<TSharedPtr<FJsonValue>>* ScenarioResultsArray;
		if (JsonObject->TryGetArrayField(TEXT("scenario_results"), ScenarioResultsArray))
		{
			for (const auto& Item : *ScenarioResultsArray)
			{
				TSharedPtr<FJsonObject> ScenarioObj = Item->AsObject();
				if (ScenarioObj.IsValid())
				{
					FScenarioResult ScenarioItem;

					// scenario_type 파싱
					FString ScenarioTypeStr = ScenarioObj->GetStringField(TEXT("scenario_type"));
					if (ScenarioTypeStr == TEXT("READING"))
						ScenarioItem.scenario_type = EScenarioType::READING;
					else if (ScenarioTypeStr == TEXT("LISTENING"))
						ScenarioItem.scenario_type = EScenarioType::LISTENING;
					else if (ScenarioTypeStr == TEXT("WRITING"))
						ScenarioItem.scenario_type = EScenarioType::WRITING;
					else if (ScenarioTypeStr == TEXT("SPEAKING"))
						ScenarioItem.scenario_type = EScenarioType::SPEAKING;

					// display_name 파싱
					ScenarioItem.display_name = ScenarioObj->GetStringField(TEXT("display_name"));

					// final_score 파싱
					ScenarioItem.final_score = ScenarioObj->GetIntegerField(TEXT("final_score"));

					// grade 파싱
					ScenarioItem.grade = ScenarioObj->GetStringField(TEXT("grade"));

					// feedback_summary 파싱
					if (ScenarioObj->HasTypedField<EJson::Object>(TEXT("feedback_summary")))
					{
						TSharedPtr<FJsonObject> FeedbackObj = ScenarioObj->GetObjectField(TEXT("feedback_summary"));
						if (FeedbackObj.IsValid())
						{
							ScenarioItem.feedback_summary.title = FeedbackObj->GetStringField(TEXT("title"));
							ScenarioItem.feedback_summary.message = FeedbackObj->GetStringField(TEXT("message"));
						}
					}

					// action_item 파싱
					ScenarioItem.action_item = ScenarioObj->GetStringField(TEXT("action_item"));

					scenario_results.Add(ScenarioItem);
				}
			}
		}
	}
}

void FResponseEvaluationResult::PrintData() const
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

