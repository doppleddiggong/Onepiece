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

	/// @brief 매 프레임 업데이트 (필요시)
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	//----------------------------------------------------------
	// UI Elements (BindWidget)
	//----------------------------------------------------------

	/// @brief 현재 발화자 이름 표시 텍스트
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	class UTextBlock* CurrentSpeakerText;

	/// @brief 현재 질문 표시 텍스트
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	class UTextBlock* CurrentQuestionText;

	/// @brief 진행률 표시 텍스트 (예: 3/5)
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	class UTextBlock* ProgressText;

	/// @brief 대기 순서 표시 텍스트 (선택사항)
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
	class UTextBlock* QueuePositionText;

public:
	//----------------------------------------------------------
	// Public Interface
	//----------------------------------------------------------

	/**
	 * @brief SpeakStage 상태를 UI에 반영
	 * @param SpeakStage SpeakStageActor 참조
	 * @param LocalPlayerState 로컬 플레이어의 PlayerState (내 턴인지 확인용)
	 *
	 * 이 함수를 주기적으로 호출하거나 RepNotify 이벤트에서 호출
	 */
	UFUNCTION(BlueprintCallable, Category = "SpeakWidget")
	void UpdateSpeakStageUI(class ASpeakStageActor* SpeakStage, class APlayerState* LocalPlayerState);

	/**
	 * @brief 위젯 표시/숨김
	 * @param bShow true면 표시, false면 숨김
	 */
	UFUNCTION(BlueprintCallable, Category = "SpeakWidget")
	void SetWidgetVisibility(bool bShow);

protected:
	/// @brief SpeakStage 참조 캐싱 (최적화용)
	UPROPERTY()
	TObjectPtr<class ASpeakStageActor> CachedSpeakStage;

	/// @brief 로컬 플레이어 PlayerState 캐싱
	UPROPERTY()
	TObjectPtr<APlayerState> CachedLocalPlayerState;
};
