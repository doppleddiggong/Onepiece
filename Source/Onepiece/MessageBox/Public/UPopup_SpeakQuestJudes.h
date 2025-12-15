// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UPopup_SpeakQuestJudes.generated.h"

/**
 * 
 */
UCLASS()
class ONEPIECE_API UPopup_SpeakQuestJudes : public UUserWidget
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
	void InitPopup(struct FResponseSpeakingJudes& Response);

	/// @brief 확인 버튼 클릭 시 호출
	UFUNCTION()
	void OnClickOk();
	
private:
	/// @brief HideAnim 완료 시 호출되는 콜백
	UFUNCTION()
	void OnHideAnimComplete();

public:
	/// @brief 확인 버튼
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImageButton> Btn_Ok;

	/// @brief 피드백 메시지 표시 텍스트
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_Feedback;

	/// @brief 점수 표시용 HorizontalBox
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UHorizontalBox> ItemHorizontalBox;

	/// @brief Show Animation
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<class UWidgetAnimation> ShowAnim;

	/// @brief Hide Animation
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<class UWidgetAnimation> HideAnim;

private:
	/// @brief 아이템 위젯 클래스
	UPROPERTY(EditDefaultsOnly, Category = "Item")
	TSubclassOf<class UAutoDespawnItem> ItemWidgetClass;
};
