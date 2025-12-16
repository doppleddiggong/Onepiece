// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UTabButtonGroup.generated.h"

/**
 * @brief 데이터를 기반으로 탭 버튼들을 동적으로 생성하고 관리하는 탭 그룹 위젯.
 */
UCLASS()
class ONEPIECE_API UTabButtonGroup : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

public:
	/// @brief 특정 인덱스의 탭을 강제로 선택합니다.
	/// @param [in] TabIndex 선택할 탭의 인덱스
	/// @param [in] bBroadcastEvent OnTabSelected 델리게이트를 호출할지 여부
	UFUNCTION(BlueprintCallable, Category="TabButtonGroup")
	void OnSelectTab(int32 TabIndex, bool bBroadcastEvent = true);

	/// @brief 선택된 탭의 색상을 반환합니다.
	/// @return 선택된 탭의 색상
	FLinearColor GetSelectedColor() const { return Color_Selected; }

	/// @brief 비선택된 탭의 색상을 반환합니다.
	/// @return 비선택된 탭의 색상
	FLinearColor GetUnselectedColor() const { return Color_UnSelected; }
	
	/// @brief 탭 선택 이벤트 델리게이트
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTabSelected, int32, TabIndex);

	/// @brief 탭 선택이 변경될 때 발생하는 이벤트
	UPROPERTY(BlueprintAssignable, Category = "TabButtonGroup|Events")
	FOnTabSelected OnTabSelected;


protected:
	/// @brief TabLabels 데이터를 기반으로 탭 버튼들을 다시 생성하고 배치합니다.
	UFUNCTION(BlueprintCallable, Category="TabButtonGroup")
	void RebuildTabs();

	/// @brief 탭 버튼 중 하나가 클릭되었을 때 호출될 내부 핸들러.
	UFUNCTION()
	void OnTabClicked(int32 TabIndex);
	
private:
	/// @brief 선택 인디케이터의 위치를 현재 선택된 탭으로 업데이트합니다.
	void UpdateIndicatorPosition();

	/// @brief 인디케이터 애니메이션을 위한 Tick 함수
	void TickIndicatorAnimation();

protected:
	/// @brief 생성된 탭 버튼들이 추가될 컨테이너. 블루프린트에서 'TabContainer' 이름으로 생성해야 합니다.
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<class UHorizontalBox> TabContainer;

	/// @brief 각 탭에 표시될 텍스트 라벨 배열. 이 배열의 크기에 따라 탭이 생성됩니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TabButtonGroup|Data")
	TArray<FText> TabLabels;

	/// @brief 선택된 탭을 따라 움직이는 이미지 위젯. 블루프린트에서 'Image_SelectedIndicator' 이름으로 생성해야 합니다.
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<class UImage> Image_SelectedIndicator;

	/// @brief 선택된 탭의 텍스트 색상.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TabButtonGroup|Appearance")
	FLinearColor Color_Selected = FLinearColor::White;

	/// @brief 비선택된 탭의 텍스트 색상.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TabButtonGroup|Appearance")
	FLinearColor Color_UnSelected = FLinearColor(0.5f, 0.5f, 0.5f);

	/// @brief 탭 버튼으로 생성할 UTabButton 위젯 블루프린트 클래스.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TabButtonGroup|Data")
	TSubclassOf<class UTabButton> TabButtonClass;

	/// @brief 선택 인디케이터의 이동 애니메이션 활성화 여부.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TabButtonGroup|Animation")
	bool bAnimateIndicator = true;

	/// @brief 선택 인디케이터의 애니메이션 속도 (초 단위).
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TabButtonGroup|Animation", meta=(EditCondition="bAnimateIndicator"))
	float IndicatorAnimationSpeed = 0.15f;

	/// @brief 위젯이 생성될 때 기본으로 선택될 탭의 인덱스.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TabButtonGroup|Data")
	int32 DefaultTabIndex = 0;
	
private:
	/// @brief 동적으로 생성되어 현재 관리 중인 탭 버튼 위젯 인스턴스 배열.
	UPROPERTY(Transient)
	TArray<TObjectPtr<class UTabButton>> TabButtonList;

	/// @brief 현재 선택된 탭의 인덱스.
	int32 CurTabIndex = -1;

	/// @brief 애니메이션을 위한 타이머 핸들
	FTimerHandle indicatorAnimTimerHandle;

	/// @brief 애니메이션 시작 위치
	FVector2D animStartPosition = FVector2D::ZeroVector;

	/// @brief 애니메이션 목표 위치
	FVector2D animTargetPosition = FVector2D::ZeroVector;

	/// @brief 애니메이션 진행 시간
	float animElapsedTime = 0.0f;
};
