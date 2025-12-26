// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "UPageScrollItem.h"
#include "UPageScrollView.h"

void UPageScrollItem::InitData(int32 InIndex, UPageScrollView* InOwner)
{
	pageIndex = InIndex;
	ownerScrollView = InOwner;

	// Blueprint에서 구현할 SetPageContent 호출
	SetPageContent(InIndex);
}

void UPageScrollItem::UpdateFocusState_Implementation(bool bInFocused, float InScale)
{
	if (bIsFocused == bInFocused)
		return;

	bIsFocused = bInFocused;

	// 포커스 변경 이벤트 발생
	OnItemFocusChanged.Broadcast(bIsFocused);
}
