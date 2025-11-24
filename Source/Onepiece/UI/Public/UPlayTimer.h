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
	UPlayTimer(const FObjectInitializer& ObjectInitializer);

public:
	/// @brief 타이머 텍스트 블록 (BindWidget)
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	class UTextBlock* RemainPlayTimeText;

	/// @brief 타이머 텍스트를 업데이트합니다.
	/// @param TimeText 표시할 시간 문자열 (예: "03:00")
	void UpdateTimerText(const FString& TimeText);
};
