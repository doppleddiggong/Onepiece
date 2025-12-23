// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file UStateWidget.cpp
 * @brief UStateWidget의 동작을 구현합니다.
 */
#include "UStateWidget.h"

#include "Components/TextBlock.h"


void UStateWidget::InitWidget()
{
    Txt_UserId->SetText(FText::FromString(""));
    Txt_UserName->SetText(FText::FromString(""));
}

void UStateWidget::UpdateUserName(int32 InUserId, const FString& InUserName)
{
    const FString Result = FString::Format(TEXT("[{0}]"),{ InUserId });
    Txt_UserId->SetText(FText::FromString(Result));
    Txt_UserName->SetText(FText::FromString(InUserName));
}