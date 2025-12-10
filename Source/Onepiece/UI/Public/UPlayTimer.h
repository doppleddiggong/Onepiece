// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UPlayTimer.generated.h"

/**
 * @brief 플레이 타이머를 표시하는 위젯
 * @details 남은 시간을 TextBlock으로 표시합니다.
 */
UCLASS()
class ONEPIECE_API UPlayTimer : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateTimerText(const float InTime);
	
public:
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UTextBlock> Txt_MinSec;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UTextBlock> Txt_MilliSec;
};