// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "UChatHistorySystem.h"
#include "ULingoGameHelper.h"
#include "GameLogging.h"

UChatHistorySystem::UChatHistorySystem()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UChatHistorySystem::SaveChatHistory(const FString& Question, const FString& Answer)
{
	const int32 UserId = ULingoGameHelper::GetUserId(GetWorld());
	if (UserId <= 0)
	{
		PRINTLOG(TEXT("[ChatHistory] Invalid UserId: %d"), UserId);
		return;
	}

	const int32 NextIndex = GetNextIndex();
	const FString ConfigSection = GetConfigSection();
	const FString Timestamp = GetCurrentTimestamp();

	// 질문 저장
	GConfig->SetString(
		*ConfigSection,
		*GetConfigKey(UserId, TEXT("Question"), NextIndex),
		*Question,
		GGameUserSettingsIni
	);

	// 답변 저장
	GConfig->SetString(
		*ConfigSection,
		*GetConfigKey(UserId, TEXT("Answer"), NextIndex),
		*Answer,
		GGameUserSettingsIni
	);

	// 타임스탬프 저장
	GConfig->SetString(
		*ConfigSection,
		*GetConfigKey(UserId, TEXT("Timestamp"), NextIndex),
		*Timestamp,
		GGameUserSettingsIni
	);

	// Count 업데이트
	GConfig->SetInt(
		*ConfigSection,
		*GetConfigKey(UserId, TEXT("Count")),
		NextIndex + 1,
		GGameUserSettingsIni
	);

	GConfig->Flush(false, GGameUserSettingsIni);

	PRINTLOG(TEXT("[ChatHistory] Saved Index=%d, Q=%s, A=%s"), NextIndex, *Question, *Answer);
}

int32 UChatHistorySystem::LoadAllChatHistory(TArray<FChatHistoryItem>& OutHistoryList)
{
	OutHistoryList.Empty();

	const int32 UserId = ULingoGameHelper::GetUserId(GetWorld());
	if (UserId <= 0)
	{
		return 0;
	}

	const FString ConfigSection = GetConfigSection();
	int32 Count = 0;

	// Count 읽기
	GConfig->GetInt(
		*ConfigSection,
		*GetConfigKey(UserId, TEXT("Count")),
		Count,
		GGameUserSettingsIni
	);

	if (Count <= 0)
	{
		return 0;
	}

	// 모든 히스토리 읽기
	for (int32 i = 0; i < Count; ++i)
	{
		FChatHistoryItem Item;
		Item.Index = i;

		GConfig->GetString(
			*ConfigSection,
			*GetConfigKey(UserId, TEXT("Question"), i),
			Item.Question,
			GGameUserSettingsIni
		);

		GConfig->GetString(
			*ConfigSection,
			*GetConfigKey(UserId, TEXT("Answer"), i),
			Item.Answer,
			GGameUserSettingsIni
		);

		GConfig->GetString(
			*ConfigSection,
			*GetConfigKey(UserId, TEXT("Timestamp"), i),
			Item.Timestamp,
			GGameUserSettingsIni
		);

		OutHistoryList.Add(Item);
	}

	PRINTLOG(TEXT("[ChatHistory] Loaded %d items for User %d"), Count, UserId);
	return Count;
}

void UChatHistorySystem::ClearChatHistory()
{
	const int32 UserId = ULingoGameHelper::GetUserId(GetWorld());
	if (UserId <= 0)
	{
		return;
	}

	const FString ConfigSection = GetConfigSection();
	const int32 Count = GetHistoryCount();

	// 모든 히스토리 데이터 삭제
	for (int32 i = 0; i < Count; ++i)
	{
		GConfig->RemoveKey(*ConfigSection, *GetConfigKey(UserId, TEXT("Question"), i), GGameUserSettingsIni);
		GConfig->RemoveKey(*ConfigSection, *GetConfigKey(UserId, TEXT("Answer"), i), GGameUserSettingsIni);
		GConfig->RemoveKey(*ConfigSection, *GetConfigKey(UserId, TEXT("Timestamp"), i), GGameUserSettingsIni);
	}

	// Count 초기화
	GConfig->SetInt(
		*ConfigSection,
		*GetConfigKey(UserId, TEXT("Count")),
		0,
		GGameUserSettingsIni
	);

	GConfig->Flush(false, GGameUserSettingsIni);

	PRINTLOG(TEXT("[ChatHistory] Cleared all history for User %d"), UserId);
}

int32 UChatHistorySystem::GetHistoryCount() const
{
	const int32 UserId = ULingoGameHelper::GetUserId(GetWorld());
	if (UserId <= 0)
	{
		return 0;
	}

	const FString ConfigSection = GetConfigSection();
	int32 Count = 0;

	GConfig->GetInt(
		*ConfigSection,
		*GetConfigKey(UserId, TEXT("Count")),
		Count,
		GGameUserSettingsIni
	);

	return Count;
}

int32 UChatHistorySystem::GetNextIndex()
{
	const int32 UserId = ULingoGameHelper::GetUserId(GetWorld());
	const FString ConfigSection = GetConfigSection();
	int32 Count = 0;

	GConfig->GetInt(
		*ConfigSection,
		*GetConfigKey(UserId, TEXT("Count")),
		Count,
		GGameUserSettingsIni
	);

	return Count;
}

FString UChatHistorySystem::GetCurrentTimestamp() const
{
	const FDateTime Now = FDateTime::Now();
	return FString::Printf(TEXT("%04d-%02d-%02d %02d:%02d:%02d"),
		Now.GetYear(), Now.GetMonth(), Now.GetDay(),
		Now.GetHour(), Now.GetMinute(), Now.GetSecond());
}

FString UChatHistorySystem::GetConfigSection() const
{
	return TEXT("/Script/Onepiece.ChatHistory");
}

FString UChatHistorySystem::GetConfigKey(int32 UserId, const FString& KeyType, int32 Index) const
{
	if (Index < 0)
	{
		// Count 키
		return FString::Printf(TEXT("ChatHistory_%d_%s"), UserId, *KeyType);
	}
	else
	{
		// Question, Answer, Timestamp 키
		return FString::Printf(TEXT("ChatHistory_%d_%s_%d"), UserId, *KeyType, Index);
	}
}
