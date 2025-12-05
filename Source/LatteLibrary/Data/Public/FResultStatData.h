#pragma once

#include "CoreMinimal.h"
#include "FColorStyleData.h"
#include "FResourceTextureData.h"
#include "FResultStatData.generated.h"

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
struct LATTELIBRARY_API FResultStatData
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
