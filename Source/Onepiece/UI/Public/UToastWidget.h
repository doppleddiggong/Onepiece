// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UToastWidget.generated.h"

/**
 * 
 */
UCLASS()
class ONEPIECE_API UToastWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	/// @brief 위젯 초기화
	virtual void NativeConstruct() override;
	
public:
	/// @brief 튜터 메시지를 설정하고 애니메이션을 재생합니다.
	/// @param NewMessage 표시할 메시지
	UFUNCTION(BlueprintCallable, Category = "Tutor")
	void OnTutorMessage(const FText& NewMessage);

	// 자동숨김 없는 버전
	UFUNCTION(BlueprintCallable, Category = "Tutor")
	void OnShowTutorialMessage(const FText& NewMessage);

	// 숨기기
	UFUNCTION(BlueprintCallable, Category = "Tutor")
	void OnHideTutorialMessage();
	
	UFUNCTION(BlueprintCallable, Category = "Item")
	void AddItemToBoxList(const TArray<FResultStatData>& InDataList);

	UFUNCTION(BlueprintCallable, Category = "Item")
	void AddItemToBoxItem(const FResultStatData& InData);

private:
	void InitTutorMessage();

	/// @brief TutorHideAnim 완료 시 호출되는 콜백
	UFUNCTION()
	void OnTutorHideComplete();

	/// @brief 자동 Hide 타이머 시작
	void StartTutorHideTimer();

	/// @brief 다음 아이템을 HorizontalBox에 추가 (타이머 콜백)
	void ProcessNextItem();


protected:
	/// @brief 튜터 메시지 위젯 (옵션, BindWidgetOptional)
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
	TObjectPtr<class UTutorMessage> TutorMessage;

	/// @brief 아이템 표시용 HorizontalBox
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<class UHorizontalBox> ItemHorizontalBox;

	/// @brief TutorMessage Show Animation (MainWidget에서 제어)
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<class UWidgetAnimation> TutorShowAnim;

	/// @brief TutorMessage Hide Animation (MainWidget에서 제어)
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<class UWidgetAnimation> TutorHideAnim;

private:
	/// @brief 아이템 위젯 클래스
	UPROPERTY(EditDefaultsOnly, Category = "Item")
	TSubclassOf<class UAutoDespawnItem> ItemWidgetClass;

	
	/// @brief 자동 Hide 타이머 핸들
	FTimerHandle TutorHideTimerHandle;

	/// @brief 펜딩 중인 메시지 (Hide 완료 후 표시할 메시지)
	FText PendingMessage;

	/// @brief 튜터 메시지 표시 지속 시간 (초)
	UPROPERTY(EditDefaultsOnly, Category = "Tutor")
	float TutorMessageDisplayDuration = 3.0f;

	/// @brief 펜딩 메시지 존재 여부
	bool bHasPendingMessage = false;

	/// @brief 튜터 메시지 표시 여부
	bool bIsTutorVisible = false;

	/// @brief 아이템 순차 추가 타이머 핸들
	FTimerHandle ItemAddTimerHandle;

	/// @brief 순차 추가 대기 중인 아이템 데이터 리스트
	TArray<FResultStatData> PendingItemDataList;

	/// @brief 현재 추가 중인 아이템 인덱스
	int32 CurItemIndex = 0;

	/// @brief 아이템 추가 간격 (초)
	UPROPERTY(EditDefaultsOnly, Category = "Item")
	float ItemAddInterval = 0.1f;
};
