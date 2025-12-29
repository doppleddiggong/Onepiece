// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "UInteractWidget.h"
#include "UDailyKioskWidget.generated.h"

UCLASS()
class ONEPIECE_API UDailyKioskWidget : public UInteractWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	/** Daily Best Score를 로드하고 UI에 표시 */
	UFUNCTION(BlueprintCallable, Category = "DailyKiosk")
	void UpdateBestScore();

protected:
	/** Best Score 표시 텍스트 (Blueprint에서 바인딩) */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_BestScore;
};
