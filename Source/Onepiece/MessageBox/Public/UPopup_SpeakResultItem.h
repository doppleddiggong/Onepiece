// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UPopup_SpeakResultItem.generated.h"

USTRUCT(BlueprintType)
struct FSpeakResultItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpeakResultItem")
	int32 Index;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpeakResultItem")
	FString Question;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpeakResultItem")
	FString Feedback;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpeakResultItem")
	float Score = 0.0f;
};


UCLASS()
class ONEPIECE_API UPopup_SpeakResultItem : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitData(const FSpeakResultItem& InItemData);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_Index;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_Question;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_Feedback;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Image_Grade;
};