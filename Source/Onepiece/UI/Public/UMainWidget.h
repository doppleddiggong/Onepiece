// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UMainWidget.generated.h"

UCLASS()
class ONEPIECE_API UMainWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UMainWidget(const FObjectInitializer& ObjectInitializer);

protected:
	/// @brief 위젯 초기화와 브로드캐스트 구독을 수행합니다.
	virtual void NativeConstruct() override;

	/// @brief 매 프레임 호출되어 타이머를 업데이트합니다.
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	/// @brief 타이머 텍스트를 업데이트합니다.
	void UpdateTimerDisplay();
	
public:
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	class UTextBlock* RemainPlayTimeText;

protected:
	UPROPERTY()
	TObjectPtr<class UBroadcastManager> BroadcastManager;

	UPROPERTY()
	TObjectPtr<class ALingoGameState> CachedGameState;
};
