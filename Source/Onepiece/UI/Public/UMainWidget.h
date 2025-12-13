// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "ALingoPlayerState.h"
#include "ASpeakStageActor.h"
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

protected:
	/// @brief 위젯 초기화
	virtual void NativeConstruct() override;

	/// @brief 매 프레임 호출되어 타이머를 업데이트합니다.
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
public:
	void SetMissionTimerState(bool bIsActive) const;

	void UpdateStateWidget(int32 UserId, const FString& UserName);
	void UpdateRoomWidget(int32 InRoomLevel, int64 InRoomId);
	void UpdateSpeakWidget();
	bool CanShowSpeakWidget(ASpeakStageActor*& OutSpeakStage, ALingoPlayerState*& OutLocalPlayerState) const;

	/// @brief 훅 인디케이터 상태 업데이트 (에임/비에임)
	/// @param bIsAiming true면 타겟 감지됨(파란색), false면 비감지(회색)
	UFUNCTION(BlueprintCallable, Category = "Hook")
	void UpdateHookState(bool bIsAiming);

	/// @brief 페이드 아웃 효과 시작 (화면이 어두워짐)
	/// @param Duration 페이드 아웃 지속 시간 (초)
	UFUNCTION(BlueprintCallable, Category = "Fade")
	void FadeOut(float Duration = 0.5f);

	/// @brief 페이드 인 효과 시작 (화면이 밝아짐)
	/// @param Duration 페이드 인 지속 시간 (초)
	UFUNCTION(BlueprintCallable, Category = "Fade")
	void FadeIn(float Duration = 0.5f);

	/// @brief FadeWidget 가져오기
	UFUNCTION(BlueprintPure, Category = "Fade")
	class UFadeWidget* GetFadeWidget() const { return FadeWidget; }

	UFUNCTION(BlueprintPure, Category = "Quest")
	class UQuestInfoWidget* GetQuestInfoWidget() const { return QuestInfoWidget; }
	
private:
	/// @brief 타이머 텍스트를 업데이트합니다.
	void UpdateTimerDisplay() const;

	/// @brief 미션 타이머 상태 변경 핸들러
	UFUNCTION()
	void OnUpdateMissionTimerState(bool bIsActive, float TimeLimit);

protected:
	/// @brief 플레이 타이머 위젯 (BindWidget)
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UPlayTimer> PlayTimer;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class URoomWidget>  RoomWidget;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UStateWidget> StateWidget;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UWidgetSwitcher> WidgetSwitcher;

	/// @brief Speak Stage UI 위젯 (옵션, BindWidgetOptional)
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class USpeakWidget> SpeakWidget;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UQuestInfoWidget> QuestInfoWidget;

	/// @brief 훅 가능 대상 표시 위젯 (옵션, BindWidgetOptional)
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
	TObjectPtr<class UImage> HookTargetIndicator;

	/// @brief 페이드 인/아웃 위젯 (옵션, BindWidgetOptional)
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
	TObjectPtr<class UFadeWidget> FadeWidget;

private:
	/// @brief 훅 타겟 감지 시 이미지 (파란색)
	UPROPERTY(EditDefaultsOnly, Category = "Hook")
	TObjectPtr<class UTexture2D> HookAimTexture;

	/// @brief 훅 타겟 미감지 시 이미지 (회색)
	UPROPERTY(EditDefaultsOnly, Category = "Hook")
	TObjectPtr<class UTexture2D> HookNoAimTexture;

	/// @brief 아이템 위젯 클래스
	UPROPERTY(EditDefaultsOnly, Category = "Item")
	TSubclassOf<class UAutoDespawnItem> ItemWidgetClass;

	/// @brief GameState 참조 캐싱
	UPROPERTY()
	TObjectPtr<class ALingoGameState> CachedGameState;
};
