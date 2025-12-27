// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "FChatHistoryItem.h"

#include "JsonObjectConverter.h"
#include "Misc/Base64.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"


FString FChatHistoryItem::CurrentTimestamp()
{
	const FDateTime Now = FDateTime::Now();
	return FString::Printf(TEXT("%04d-%02d-%02d %02d:%02d:%02d"),
		Now.GetYear(), Now.GetMonth(), Now.GetDay(),
		Now.GetHour(), Now.GetMinute(), Now.GetSecond());
}


FString FChatHistoryItem::ToJson() const
{
	FString OutputString;
	FJsonObjectConverter::UStructToJsonObjectString(
		*this,
		OutputString,
		0,
		0
	);

	return OutputString;
}

bool FChatHistoryItem::FromJson(const FString& JsonString, FChatHistoryItem& OutItem)
{
	return FJsonObjectConverter::JsonObjectStringToUStruct(
		JsonString,
		&OutItem,
		0,
		0
	);
}