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

	void StartMissionTimer() const;
	void StopMissionTimer() const;
	
protected:
	/// @brief 위젯 초기화
	virtual void NativeConstruct() override;

	/// @brief 매 프레임 호출되어 타이머를 업데이트합니다.
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	/// @brief 플레이 타이머 위젯 (BindWidget)
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UPlayTimer> PlayTimer;

	/// @brief 플레이어 상태 위젯 (BindWidget)
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UStateWidget> StateWidget;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UQuestInfoWidget> QuestInfoWidget;

	/// @brief Speak Stage UI 위젯 (옵션, BindWidgetOptional)
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
	TObjectPtr<class USpeakWidget> SpeakWidget;

	/// @brief 훅 가능 대상 표시 위젯 (옵션, BindWidgetOptional)
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
	TObjectPtr<class UImage> HookTargetIndicator;

	/// @brief 튜터 메시지 위젯 (옵션, BindWidgetOptional)
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
	TObjectPtr<class UTutorMessage> TutorMessage;

	/// @brief TutorMessage Show Animation (MainWidget에서 제어)
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<class UWidgetAnimation> TutorShowAnim;

	/// @brief TutorMessage Hide Animation (MainWidget에서 제어)
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<class UWidgetAnimation> TutorHideAnim;

public:
	/// @brief 훅 인디케이터 상태 업데이트 (에임/비에임)
	/// @param bIsAiming true면 타겟 감지됨(파란색), false면 비감지(회색)
	UFUNCTION(BlueprintCallable, Category = "Hook")
	void UpdateHookIndicatorState(bool bIsAiming);

	/// @brief 튜터 메시지를 설정하고 애니메이션을 재생합니다.
	/// @param NewMessage 표시할 메시지
	UFUNCTION(BlueprintCallable, Category = "Tutor")
	void SetTutorMessage(const FText& NewMessage);

private:
	/// @brief 훅 타겟 감지 시 이미지 (파란색)
	UPROPERTY(EditDefaultsOnly, Category = "Hook")
	TObjectPtr<class UTexture2D> HookAimTexture;

	/// @brief 훅 타겟 미감지 시 이미지 (회색)
	UPROPERTY(EditDefaultsOnly, Category = "Hook")
	TObjectPtr<class UTexture2D> HookNoAimTexture;

protected:
	/// @brief GameState 참조 캐싱
	UPROPERTY()
	TObjectPtr<class ALingoGameState> CachedGameState;

	/// @brief 타이머 텍스트를 업데이트합니다.
	void UpdateTimerDisplay();

	/// @brief SpeakWidget UI를 업데이트합니다.
	void UpdateSpeakWidget();

	/// @brief 미션 타이머 상태 변경 핸들러
	UFUNCTION()
	void OnUpdateMissionTimerState(bool bIsActive, float TimeLimit);

	/// @brief TutorHideAnim 완료 시 호출되는 콜백
	UFUNCTION()
	void OnTutorHideComplete();

	/// @brief 자동 Hide 타이머 시작
	void StartAutoHideTimer();

private:
	/// @brief 튜터 메시지 표시 지속 시간 (초)
	UPROPERTY(EditDefaultsOnly, Category = "Tutor")
	float TutorMessageDisplayDuration = 3.0f;

	/// @brief 자동 Hide 타이머 핸들
	FTimerHandle AutoHideTimerHandle;

	/// @brief 펜딩 중인 메시지 (Hide 완료 후 표시할 메시지)
	FText PendingMessage;

	/// @brief 펜딩 메시지 존재 여부
	bool bHasPendingMessage = false;

	/// @brief 튜터 메시지 표시 여부
	bool bIsTutorVisible = false;
};
