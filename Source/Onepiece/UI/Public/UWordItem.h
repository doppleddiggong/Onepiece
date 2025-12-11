// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UWordItem.generated.h"

UENUM(Blueprintable)
enum class EWordType : uint8
{
	Animal,
	Color,
	Region,
	Food
};

UCLASS()
class ONEPIECE_API UWordItem : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitInfo( EWordType WordType, int32 WordCode );
	void SetTextColor(FLinearColor InColor);
	
public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UHorizontalBox> Target;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Image_Target;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_Kor;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_Eng;
};
