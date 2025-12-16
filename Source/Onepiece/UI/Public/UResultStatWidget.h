// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "FColorStyleData.h"
#include "FResultStatData.h"
#include "FResourceTextureData.h"
#include "Blueprint/UserWidget.h"
#include "UResultStatWidget.generated.h"


UCLASS()
class ONEPIECE_API UResultStatWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativePreConstruct() override;

public:
	/** 통합 데이터로 위젯 설정 */
	UFUNCTION(BlueprintCallable, Category="ResultStat")
	void InitData(const FResultStatData& InData);

	/** 위젯 타입 설정 */
	UFUNCTION(BlueprintCallable)
	void SetWidgetType(const EResultItemWidgetType InType);
	
	/** 스타일 설정 */
	UFUNCTION(BlueprintCallable)
	void SetColorType(const EColorStyleType InType);

	UFUNCTION(BlueprintCallable)
	void SetTitleText(const FText InText);
	
	/** 패널 데이터 설정 */
	UFUNCTION(BlueprintCallable)
	void SetGradeValue(const EResourceTextureType TextureType);

	UFUNCTION(BlueprintCallable)
	void SetScoreValue(const float InValue, const FLinearColor InScoreTextColor);

	UFUNCTION(BlueprintCallable)
	void SetRateValue(const float InPercent);

	UFUNCTION(BlueprintCallable)
	void SetSymbolValue(const EResourceTextureType TextureType, const FString& InValue);

private:
	void ApplyStyle();
	void LoadStyleTable();

	void UpdateWidgetPanel() const;

public:
	// ----------------------------
	// BindWidget references
	// ----------------------------
	// Style widgets
	UPROPERTY(meta=(BindWidget), BlueprintReadOnly)
	TObjectPtr<class UImage> Image_BG;

	UPROPERTY(meta=(BindWidget), BlueprintReadOnly)
	TObjectPtr<class UBorder> Border_Title;

	UPROPERTY(meta=(BindWidget), BlueprintReadOnly)
	TObjectPtr<class UTextBlock> Text_Title;

	UPROPERTY(meta=(BindWidget), BlueprintReadOnly)
	TObjectPtr<class UWidgetSwitcher> WidgetSwitcher;

	// Grade Panel
	UPROPERTY(meta=(BindWidget), BlueprintReadOnly)
	TObjectPtr<class UImage> Image_Grade;

	// Score Panel
	UPROPERTY(meta=(BindWidget), BlueprintReadOnly)
	TObjectPtr<class UTextBlock> Txt_Score;

	// Rate Panel
	UPROPERTY(meta=(BindWidget), BlueprintReadOnly)
	TObjectPtr<class UTextBlock> Txt_Rate;

	UPROPERTY(meta=(BindWidget), BlueprintReadOnly)
	TObjectPtr<class UCircularProgressBar> ImageProgress_Rate;

	// Symbol Panel
	UPROPERTY(meta=(BindWidget), BlueprintReadOnly)
	TObjectPtr<class UImage> Image_Symbol;

	UPROPERTY(meta=(BindWidget), BlueprintReadOnly)
	TObjectPtr<class UTextBlock> Txt_ImageRate;

	// -----------------------------------------------------
	// Data
	// -----------------------------------------------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Style")
	EColorStyleType ColorType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Style")
	TMap<EColorStyleType, FColorStyleData> StyleTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Data")
	EResultItemWidgetType WidgetType;

	// 저장값
	float GradeValue = 0.f;
	float ScoreValue = 0.f;
	FLinearColor ScoreTextColor = FLinearColor::White;
	float RateValue = 0.f;
	FString SymbolValue;
};
