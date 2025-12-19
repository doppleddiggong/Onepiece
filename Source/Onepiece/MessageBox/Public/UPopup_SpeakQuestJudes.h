// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "UBasePopup.h"
#include "Blueprint/UserWidget.h"
#include "UPopup_SpeakQuestJudes.generated.h"

/**
 * 
 */
UCLASS()
class ONEPIECE_API UPopup_SpeakQuestJudes : public UBasePopup
{
	GENERATED_BODY()

protected:
	/// @brief 위젯 초기화
	virtual void NativeConstruct() override;

public:
	/**
	 * @brief 팝업을 초기화하고 데이터를 설정합니다
	 * @param Response SpeakingJudes 응답 데이터
	 */
	void InitPopup(const struct FResponseSpeakingJudes& Response);

	/// @brief 확인 버튼 클릭 시 호출
	UFUNCTION()
	void OnClickOk();
	
private:
	/// @brief HideAnim 완료 시 호출되는 콜백
	UFUNCTION()
	void OnHideAnimComplete();

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Image_Grade;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UDespawnItem> GrammerItem;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UDespawnItem> ContextItem;
	
	/// @brief Show Animation
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<class UWidgetAnimation> ShowAnim;

	/// @brief Hide Animation
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<class UWidgetAnimation> HideAnim;

	/// @brief 아이템 생존 시간 (초)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float Lifetime = 3.0f;

private:
	/// @brief Lifetime 타이머 핸들
	FTimerHandle LifetimeTimer;
};
