// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "FColorStyleData.h"
#include "FResourceTextureData.h"
#include "Blueprint/UserWidget.h"
#include "UWidgetResultItem.generated.h"

UENUM(BlueprintType)
enum class EResultItemWidgetType : uint8
{
	Grade,       // GradePanel
	Score,       // ScorePanel
	Rate,        // RatePanel
	Symbol		 // SymbolPanel
};

UCLASS()
class ONEPIECE_API UWidgetResultItem : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativePreConstruct() override;

public:
	/** 위젯 타입 설정 */
	UFUNCTION(BlueprintCallable)
	void SetWidgetType(EResultItemWidgetType InType);

	/** 패널 데이터 설정 */
	UFUNCTION(BlueprintCallable)
	void SetGradeValue(EResourceTextureType TextureType);

	UFUNCTION(BlueprintCallable)
	void SetScoreValue(float InValue);

	UFUNCTION(BlueprintCallable)
	void SetRateValue(float InPercent);

	UFUNCTION(BlueprintCallable)
	void SetSymbolValue(float InValue);

	/** 스타일 설정 */
	UFUNCTION(BlueprintCallable)
	void SetColorType(EColorStyleType InType);

	/** GameDataManager에서 StyleTable 로드 */
	UFUNCTION(BlueprintCallable)
	void LoadStyleTableFromDataManager();

private:
	void ApplyStyle();
	void UpdateActivePanel();

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

	// Image Rate Panel
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
	float RateValue = 0.f;
	float SymbolValue = 0.f;
};
