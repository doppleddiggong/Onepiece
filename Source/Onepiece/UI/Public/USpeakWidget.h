// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "USpeakWidget.generated.h"

/**
 * @brief Speak Stage UI 위젯
 * @details 현재 발화자, 질문, 진행률 등을 표시
 *
 * 필수 위젯 (BindWidget):
 * - CurrentSpeakerText: 현재 발화자 이름
 * - CurrentQuestionText: 현재 질문
 * - ProgressText: 진행률 (예: 3/5)
 *
 * 선택 위젯 (BindWidgetOptional):
 * - QueuePositionText: 대기 순서 표시
 */
UCLASS()
class ONEPIECE_API USpeakWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/// @brief 생성자
	USpeakWidget(const FObjectInitializer& ObjectInitializer);
	
protected:
	/// @brief 위젯 초기화
	virtual void NativeConstruct() override;

public:
	/**
	 * @brief SpeakStage 상태를 UI에 반영
	 * @param SpeakStage SpeakStageActor 참조
	 * @param LocalPlayerState 로컬 플레이어의 PlayerState (내 턴인지 확인용)
	 *
	 * 이 함수를 주기적으로 호출하거나 RepNotify 이벤트에서 호출
	 */
	UFUNCTION(BlueprintCallable, Category = "SpeakWidget")
	void UpdateSpeakStage(class ASpeakStageActor* SpeakStage, class APlayerState* LocalPlayerState);

	/**
	 * @brief 위젯 표시/숨김
	 * @param bShow true면 표시, false면 숨김
	 */
	UFUNCTION(BlueprintCallable, Category = "SpeakWidget")
	void SetWidgetVisibility(bool bShow);

protected:
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UTextBlock> CurrentQuestionText;
};
