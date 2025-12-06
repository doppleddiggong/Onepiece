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

protected:
	/// @brief 위젯 초기화
	virtual void NativeConstruct() override;

	/// @brief 매 프레임 호출되어 타이머를 업데이트합니다.
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
public:
	void SetMissionTimerState(bool bIsActive) const;
	
	/// @brief 훅 인디케이터 상태 업데이트 (에임/비에임)
	/// @param bIsAiming true면 타겟 감지됨(파란색), false면 비감지(회색)
	UFUNCTION(BlueprintCallable, Category = "Hook")
	void UpdateHookState(bool bIsAiming);

	// /// @brief 튜터 메시지를 설정하고 애니메이션을 재생합니다.
	// /// @param NewMessage 표시할 메시지
	// UFUNCTION(BlueprintCallable, Category = "Tutor")
	// void OnTutorMessage(const FText& NewMessage);
	//
	// UFUNCTION(BlueprintCallable, Category = "Item")
	// void AddItemToBoxList(const TArray<FResultStatData>& InDataList);
	//
	// UFUNCTION(BlueprintCallable, Category = "Item")
	// void AddItemToBoxItem(const FResultStatData& InData);
	//
		
private:
	// void InitTutorMessage();

	/// @brief 타이머 텍스트를 업데이트합니다.
	void UpdateTimerDisplay() const;

	/// @brief SpeakWidget UI를 업데이트합니다.
	void UpdateSpeakWidget();

	/// @brief 미션 타이머 상태 변경 핸들러
	UFUNCTION()
	void OnUpdateMissionTimerState(bool bIsActive, float TimeLimit);

	// /// @brief TutorHideAnim 완료 시 호출되는 콜백
	// UFUNCTION()
	// void OnTutorHideComplete();
	//
	// /// @brief 자동 Hide 타이머 시작
	// void StartTutorHideTimer();
	//
	// /// @brief 다음 아이템을 HorizontalBox에 추가 (타이머 콜백)
	// void ProcessNextItem();

protected:
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

	// /// @brief 튜터 메시지 위젯 (옵션, BindWidgetOptional)
	// UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
	// TObjectPtr<class UTutorMessage> TutorMessage;
	//
	// /// @brief 아이템 표시용 HorizontalBox
	// UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	// TObjectPtr<class UHorizontalBox> ItemHorizontalBox;
	//
	// /// @brief TutorMessage Show Animation (MainWidget에서 제어)
	// UPROPERTY(Transient, meta = (BindWidgetAnim))
	// TObjectPtr<class UWidgetAnimation> TutorShowAnim;
	//
	// /// @brief TutorMessage Hide Animation (MainWidget에서 제어)
	// UPROPERTY(Transient, meta = (BindWidgetAnim))
	// TObjectPtr<class UWidgetAnimation> TutorHideAnim;

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
