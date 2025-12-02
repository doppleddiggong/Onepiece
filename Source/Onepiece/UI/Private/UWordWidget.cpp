// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "UWordWidget.h"

#include "URichText.h"
#include "NetworkData.h"
#include "UPopupManager.h"
#include "Components/TextBlock.h"

void UWordWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Rich_Kor)
		Rich_Kor->SetText(FText::FromString(TEXT("")));
	
	if (Txt_Eng)
		Txt_Eng->SetText(FText::FromString(TEXT("")));
}

void UWordWidget::InitWordData(const FWordData& InWordData)
{
	Rich_Kor->SetText(InWordData);
	Rich_Kor->SetOnClickHyperLink(FOnClickHyperLink::CreateUObject(this, &UWordWidget::OnClickHyperLink));
	
	Txt_Eng->SetText(FText::FromString(InWordData.Eng));
}

void UWordWidget::OnClickHyperLink(const FPhonemeData& Data)
{
	FString Msg = FString::Printf( TEXT("%s, %s"), *Data.Kor, *Data.Pronunciation);
	UPopupManager::Get(GetWorld())->ShowMsgBox(TEXT("Notice"), Msg,
		EMsgBoxType::OK, FOnMsgBoxOkDelegate());
}
