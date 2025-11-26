// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "UWordWidget.h"
#include "UWordButton.h"
#include "Components/TextBlock.h"

void UWordWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 초기 상태에서는 정보를 숨김
	if (Txt_Eng)
		Txt_Eng->SetText(FText::FromString(TEXT("")));

	if (Txt_Kor)
		Txt_Kor->SetText(FText::FromString(TEXT("")));

	if (Txt_Pronunciation)
		Txt_Pronunciation->SetText(FText::FromString(TEXT("")));
}

void UWordWidget::InitWordData(const FWordData& InWordData)
{
	Txt_Eng->SetText(FText::FromString(InWordData.Eng));
	Txt_Kor->SetText(FText::FromString(InWordData.Kor));
	Txt_Pronunciation->SetText(FText::FromString(InWordData.Pronunciation));
}