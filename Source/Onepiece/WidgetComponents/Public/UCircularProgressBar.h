// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UCircularProgressBar.generated.h"

/**
 * 
 */
UCLASS()
class ONEPIECE_API UCircularProgressBar : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativePreConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	UFUNCTION(BlueprintCallable, Category = "Progress")
	void SetPercent(const float InPercent);

	/** Start에서 End까지 부드럽게 애니메이션 */
	UFUNCTION(BlueprintCallable, Category = "Progress")
	void StartProgress(const float Start, const float End, const float Duration = 1.0f);

	/** 애니메이션 중단 */
	UFUNCTION(BlueprintCallable, Category = "Progress")
	void StopProgress();

private:
	void ApplyStyle();

	// 이징 함수
	FORCEINLINE double easeOutSine(const double Value)
	{
		return FMath::Sin((Value * PI) / 2.0);
	}
	
public:
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	class UImage* Img_CircularBar;

	/** 진행률 (0.0 ~ 1.0) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float Percent = 0.0f;

	/** 낮은 진행률 색상 (0% ~ 50%) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style", meta = (ExposeOnSpawn = true))
	FLinearColor LowColor = FLinearColor(0.9f, 0.2f, 0.2f); // 빨강

	/** 중간 진행률 색상 (50%) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style", meta = (ExposeOnSpawn = true))
	FLinearColor MidColor = FLinearColor(1.0f, 0.8f, 0.2f); // 노랑

	/** 높은 진행률 색상 (50% ~ 100%) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style", meta = (ExposeOnSpawn = true))
	FLinearColor HighColor = FLinearColor(0.2f, 0.9f, 0.3f); // 초록


private:
	bool bIsAnimating = false;
	
	float AnimStartPercent = 0.0f;
	float AnimTargetPercent = 0.0f;
	float AnimElapsedTime = 0.0f;
	float AnimDuration = 1.0f;
};
