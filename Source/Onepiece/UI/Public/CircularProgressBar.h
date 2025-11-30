// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CircularProgressBar.generated.h"

/**
 * 
 */
UCLASS()
class ONEPIECE_API UCircularProgressBar : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	class UImage* Img_CircularBar;

	UFUNCTION(BlueprintCallable)
	void SetPercent(float Percent);

protected:
	// 차오르는 애니메이션

	// 이징 함수
	double easeOutSine(double x);
};
