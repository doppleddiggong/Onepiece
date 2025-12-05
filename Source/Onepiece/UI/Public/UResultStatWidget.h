// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "FColorStyleData.h"
#include "FResourceTextureData.h"
#include "Blueprint/UserWidget.h"
#include "UResultStatWidget.generated.h"

UENUM(BlueprintType)
enum class EResultItemWidgetType : uint8
{
	Grade,       // GradePanel
	Score,       // ScorePanel
	Rate,        // RatePanel
	Symbol		 // SymbolPanel
};

/**
 * Result Stat 위젯 통합 데이터 구조
 * 위젯 타입, 색상 스타일, 각 타입별 데이터를 통합 관리
 */
USTRUCT(BlueprintType)
struct FResultStatData
{
	GENERATED_BODY()

	/** 위젯 타입 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ResultStat")
	EResultItemWidgetType WidgetType = EResultItemWidgetType::Score;

	/** 색상 스타일 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ResultStat")
	EColorStyleType ColorType = EColorStyleType::Green;

	/** 타이틀 텍스트 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ResultStat")
	FText TitleText;

	/** Grade 타입 전용: 텍스처 타입 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ResultStat|Grade", meta=(EditCondition="WidgetType == EResultItemWidgetType::Grade", EditConditionHides))
	EResourceTextureType GradeTextureType = EResourceTextureType::Rarity_D;

	/** Score 타입 전용: 점수 값 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ResultStat|Score", meta=(EditCondition="WidgetType == EResultItemWidgetType::Score", EditConditionHides))
	float ScoreValue = 0.f;

	/** Rate 타입 전용: 퍼센트 값 (0.0 ~ 1.0) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ResultStat|Rate", meta=(EditCondition="WidgetType == EResultItemWidgetType::Rate", EditConditionHides))
	float RatePercent = 0.f;

	/** Symbol 타입 전용: 심볼 값 (퍼센트로 표시, 0.0 ~ 1.0) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ResultStat|Symbol", meta=(EditCondition="WidgetType == EResultItemWidgetType::Symbol", EditConditionHides))
	float SymbolValue = 0.f;

	FResultStatData()
		: WidgetType(EResultItemWidgetType::Score)
		, ColorType(EColorStyleType::Green)
		, TitleText(FText::GetEmpty())
		, GradeTextureType(EResourceTextureType::Rarity_D)
		, ScoreValue(0.f)
		, RatePercent(0.f)
		, SymbolValue(0.f)
	{
	}
};

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
	void SetScoreValue(const float InValue);

	UFUNCTION(BlueprintCallable)
	void SetRateValue(const float InPercent);

	UFUNCTION(BlueprintCallable)
	void SetSymbolValue(const float InValue);


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
