// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UMainWidget.generated.h"

/**
 * @brief 메인 UI 위젯
 * @details PlayTimer와 StateWidget을 포함하고 타이머 업데이트를 관리합니다.
 */
UCLASS()
class ONEPIECE_API UMainWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UMainWidget(const FObjectInitializer& ObjectInitializer);

	void StartMissionTimer();
	void StopMissionTimer();
	
protected:
	/// @brief 위젯 초기화
	virtual void NativeConstruct() override;

	/// @brief 매 프레임 호출되어 타이머를 업데이트합니다.
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	/// @brief 플레이 타이머 위젯 (BindWidget)
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	class UPlayTimer* PlayTimer;

	/// @brief 플레이어 상태 위젯 (BindWidget)
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	class UStateWidget* StateWidget;

protected:
	/// @brief GameState 참조 캐싱
	UPROPERTY()
	TObjectPtr<class ALingoGameState> CachedGameState;

	/// @brief 타이머 텍스트를 업데이트합니다.
	void UpdateTimerDisplay();

	/// @brief 미션 타이머 상태 변경 핸들러
	UFUNCTION()
	void OnMissionTimerStateChanged(bool bIsActive);
};
