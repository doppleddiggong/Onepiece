// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "UChatHistorySystem.h"
#include "ULingoGameHelper.h"
#include "UConfigLibrary.h"
#include "FChatHistoryItem.h"
#include "GameLogging.h"
#include "Algo/Reverse.h"

void UChatHistorySystem::SaveChatHistory(const FString& Question, const FString& Answer)
{
	const int32 UserId = ULingoGameHelper::GetUserId(GetWorld());
	if (UserId <= 0)
	{
		PRINTLOG(TEXT("[ChatHistory] Invalid UserId: %d"), UserId);
		return;
	}

	// 1. 현재 Count 가져오기 (O(1))
	const int32 CurrentCount = UConfigLibrary::GetUserInt(UserId, TEXT("ChatHistoryCount"), 0);

	// 2. 새 항목 생성
	FChatHistoryItem NewItem;
	NewItem.Index = CurrentCount;
	NewItem.Question = Question;
	NewItem.Answer = Answer;
	NewItem.Timestamp = FChatHistoryItem::CurrentTimestamp();

	// 3. JSON으로 직렬화
	const FString JsonString = NewItem.ToJson();

	// 4. 개별 키로 저장 (새 항목만!)
	const FString ItemKey = FString::Printf(TEXT("ChatHistory_%d"), CurrentCount);
	UConfigLibrary::SetUserString(UserId, ItemKey, JsonString, false);

	// 5. Count 증가 (한 번에 저장)
	UConfigLibrary::SetUserInt(UserId, TEXT("ChatHistoryCount"), CurrentCount + 1);
}

int32 UChatHistorySystem::LoadAllChatHistory(TArray<FChatHistoryItem>& OutHistoryList)
{
	OutHistoryList.Empty();

	const int32 UserId = ULingoGameHelper::GetUserId(GetWorld());
	if (UserId <= 0)
	{
		return 0;
	}

	// 1. Count 가져오기
	const int32 Count = UConfigLibrary::GetUserInt(UserId, TEXT("ChatHistoryCount"), 0);
	if (Count == 0)
	{
		return 0;
	}

	// 2. 각 항목을 개별적으로 로드
	for (int32 i = 0; i < Count; ++i)
	{
		const FString ItemKey = FString::Printf(TEXT("ChatHistory_%d"), i);
		const FString JsonString = UConfigLibrary::GetUserString(UserId, ItemKey, TEXT(""));

		if (!JsonString.IsEmpty())
		{
			FChatHistoryItem Item;
			if (FChatHistoryItem::FromJson(JsonString, Item))
			{
				OutHistoryList.Add(Item);
			}
		}
	}

	// 3. 최신순으로 정렬 (역순)
	Algo::Reverse(OutHistoryList);

	return OutHistoryList.Num();
}

void UChatHistorySystem::ClearChatHistory()
{
	const int32 UserId = ULingoGameHelper::GetUserId(GetWorld());
	if (UserId <= 0)
	{
		return;
	}

	// 1. Count 가져오기
	const int32 Count = UConfigLibrary::GetUserInt(UserId, TEXT("ChatHistoryCount"), 0);

	// 2. 모든 항목 키 삭제
	for (int32 i = 0; i < Count; ++i)
	{
		const FString ItemKey = FString::Printf(TEXT("ChatHistory_%d"), i);
		UConfigLibrary::DeleteUserKey(UserId, ItemKey, false); // 마지막에 한 번만 저장
	}

	// 3. Count 삭제 (한 번에 저장)
	UConfigLibrary::DeleteUserKey(UserId, TEXT("ChatHistoryCount"));
}

int32 UChatHistorySystem::GetHistoryCount() const
{
	const int32 UserId = ULingoGameHelper::GetUserId(GetWorld());
	if (UserId <= 0)
	{
		return 0;
	}

	// Count를 직접 반환 (O(1))
	return UConfigLibrary::GetUserInt(UserId, TEXT("ChatHistoryCount"), 0);
}

int32 UChatHistorySystem::GetNextIndex() const
{
	// GetHistoryCount()가 다음 Index를 반환
	return GetHistoryCount();
}
