// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Blueprint/UserWidget.h"
#include "URichText.generated.h"


struct FWordData;
struct FPhonemeData;
/**
 * @brief Custom RichText widget with clickable hyperlink support for phoneme data
 */
UCLASS()
class ONEPIECE_API URichText : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	
public:
	void SetText(const FWordData& WordData);
	void SetText(const FString& InString);
	void SetText(const FText& InText);

private:
	UFUNCTION()
	void OnClickLink(const FString& LinkID, const FString& Content);

	void ApplyStyle();
	
public:
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class URichTextBlock> RichTxt;

	// URichText.h
	UPROPERTY(EditAnywhere, Category = "Style")
	int32 FontSize = 32;

	UPROPERTY(EditAnywhere, Category = "Style")
	FLinearColor TextColor = FLinearColor::White;

	UPROPERTY(EditAnywhere, Category = "Style")
	FString DefaultText;

	
private:
	/// @brief Stores phoneme data for click handling
	TArray<FPhonemeData> CachedPhonemeData;
};
