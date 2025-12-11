// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "ALingoGameState.h"
#include "Blueprint/UserWidget.h"
#include "UAnswerItem.generated.h"

UCLASS()
class ONEPIECE_API UAnswerItem : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitInfo(	EQuestType QuestType, int32 InOrder,
		FScenarioTargetData TargetData, FScenarioTargetData CorrectData );

private:
	void InitInfo(
		EQuestType QuestType, bool bCorrect,
		int32 InOrder, int32 Word1Code, int32 Word2Code);
	
public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UBorder> Border_Order;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_Order;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UWordItem> TargetWord1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UWordItem> TargetWord2;
};
