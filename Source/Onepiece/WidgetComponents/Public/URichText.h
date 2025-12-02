// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Blueprint/UserWidget.h"
#include "URichText.generated.h"


struct FWordData;
struct FPhonemeData;

DECLARE_DELEGATE_OneParam(FOnClickHyperLink, const FPhonemeData&);

UCLASS()
class ONEPIECE_API URichText : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
public:
	void SetText(const FWordData& WordData);
	void SetText(const FString& InString);
	void SetText(const FText& InText);

	void SetOnClickHyperLink(const FOnClickHyperLink& InClick)	{ OnClickHyperLink = InClick; }

private:
	UFUNCTION()
	void OnClickLink(const FString& LinkID, const FString& Content);

	void ApplyStyleWithFontSize(int32 InFontSize, bool bPreserveText);
	void UpdateFontSizeToFitWidth(const FGeometry& MyGeometry);
	
public:
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class URichTextBlock> RichTxt;

	// URichText.h
	UPROPERTY(EditAnywhere, Category = "Style")
	int32 FontSize = 32;

	UPROPERTY(EditAnywhere, Category = "Style")
	bool bAutoFitToWidth = true;

	UPROPERTY(EditAnywhere, Category = "Style", meta = (EditCondition = "bAutoFitToWidth", ClampMin = "4"))
	int32 MinAutoFontSize = 12;
	
	UPROPERTY(EditAnywhere, Category = "Style")
	FLinearColor TextColor = FLinearColor::White;

	UPROPERTY(EditAnywhere, Category = "Style")
	FString DefaultText;

	
private:
	TArray<FPhonemeData> CachedPhonemeData;

	FOnClickHyperLink OnClickHyperLink;

	int32 CurrentFontSize = 0;
	
	/** 폰트 크기 조정이 필요한지 여부 */
	bool bNeedsFontSizeUpdate = false;
	
	/** 안정화를 위한 프레임 카운터 */
	int32 StabilizationFrames = 0;
	
	/** 이전 위젯 너비 (변경 감지용) */
	float LastWidgetWidth = 0.0f;
};
