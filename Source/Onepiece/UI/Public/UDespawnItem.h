// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UDespawnItem.generated.h"

/**
 * 
 */
UCLASS()
class ONEPIECE_API UDespawnItem : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	void InitData(const struct FResultStatData& InData);

private:
	/// @brief FadeOut 시작
	UFUNCTION()
	void StartFadeOut();

	/// @brief 위젯 제거
	UFUNCTION()
	void RemoveSelf();

public:
	/// @brief 결과 통계 위젯 (Grade, Score, Rate, Symbol 표시)
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UResultStatWidget> ItemWidget;

	/// @brief FadeOut 애니메이션 (BindWidgetAnim)
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<class UWidgetAnimation> FadeOutAnim;

	/// @brief 아이템 생존 시간 (초)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float Lifetime = 3.0f;

	/// @brief FadeOut 애니메이션 시간 (초) - UMG 애니메이션 길이와 동일해야 함
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float FadeOutDuration = 0.3f;

private:
	/// @brief Lifetime 타이머 핸들
	FTimerHandle LifetimeTimer;
};
