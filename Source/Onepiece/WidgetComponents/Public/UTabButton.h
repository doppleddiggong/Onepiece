// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UTabButton.generated.h"

/**
 * @brief UTabButtonGroup에 의해 관리되는 개별 탭 버튼 위젯.
 * @note 사용자가 직접 생성하지 않습니다.
 */
UCLASS()
class ONEPIECE_API UTabButton : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	
public:
	/// @brief 버튼을 초기화하는 함수. 부모 그룹에 의해 호출됩니다.
	/// @param [in] InTabIndex 이 버튼에 할당될 인덱스
	/// @param [in] InOwnerGroup 이 버튼을 소유한 부모 그룹
	void InitData(const int32 InTabIndex, class UTabButtonGroup* InOwnerGroup);

	/// @brief 이 버튼의 선택 상태를 갱신합니다.
	/// @param [in] bIsSelected 새로운 선택 상태
	void SetSelected(const bool bIsSelected) const;

	/// @brief 버튼의 텍스트 라벨을 설정합니다.
	/// @param [in] InText 설정할 텍스트
	void SetLabel(const FText& InText) const;

	/// @brief 탭 버튼 클릭 이벤트 델리게이트
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTabButtonClicked, int32, TabIndex);

	/// @brief 탭 버튼이 클릭되었을 때 발생하는 이벤트
	UPROPERTY(BlueprintAssignable, Category = "TabButton")
	FOnTabButtonClicked OnTabButtonClicked;

protected:
	/// @brief 버튼 클릭 시 호출될 내부 함수
	UFUNCTION()
	void OnClicked();

protected:
	/// @brief 클릭을 감지할 루트 버튼. 블루프린트에서 'Button_Tab' 이름으로 생성해야 합니다.
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<class UButton> Button_Tab;

	/// @brief 탭의 텍스트 라벨. 블루프린트에서 'Txt_ButtonLabel' 이름으로 생성해야 합니다.
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<class UTextBlock> Txt_ButtonLabel;

	/// @brief 탭이 선택되었을 때 활성화될 이미지 (예: 하이라이트 이미지). 블루프린트에서 'Image_ActivateState' 이름으로 생성해야 합니다.
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<class UImage> Image_ActivateState;

private:
	/// @brief 이 탭 버튼을 소유한 부모 그룹
	TWeakObjectPtr<class UTabButtonGroup> OwnerTabGroup;

	/// @brief 이 탭 버튼의 그룹 내 인덱스
	int32 TabIndex = -1;
};
