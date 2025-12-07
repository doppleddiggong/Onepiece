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
	void InitInfo(
		EQuestType QuestType, bool bCorrect,
		int32 InOrder, int32 Word1Code, int32 Word2Code);
	
public:
	/* Symbol Switcher */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UWidgetSwitcher> WidgetSwitcherSymbol;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Image_Wrong;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Image_Correct;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_Order;

	/* Target1 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UHorizontalBox> Target1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Image_Target1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_Target1_Kor;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_Target1_Eng;

	/* Spacer */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Image_Spacer;

	/* Target2 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UHorizontalBox> Target2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Image_Target2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_Target2_Kor;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_Target2_Eng;
};
