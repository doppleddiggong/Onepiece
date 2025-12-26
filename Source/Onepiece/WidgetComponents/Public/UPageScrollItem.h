// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UPageScrollItem.generated.h"

/// @brief 페이지 포커스 변경 이벤트 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemFocusChanged, bool, bFocused);

/**
 * @brief 페이지 스크롤 뷰의 개별 페이지 아이템
 * UPageScrollView에 의해 동적으로 생성되며, 개별 페이지의 콘텐츠를 담는 컨테이너입니다.
 */
UCLASS()
class ONEPIECE_API UPageScrollItem : public UUserWidget
{
	GENERATED_BODY()

public:
	/// @brief 페이지 아이템을 초기화합니다.
	/// @param [in] InIndex 페이지 인덱스
	/// @param [in] InOwner 소유 ScrollView
	virtual void InitData(int32 InIndex, class UPageScrollView* InOwner);

	/// @brief 포커스 상태를 업데이트합니다.
	/// @param [in] bInFocused 포커스 여부
	/// @param [in] InScale 스케일 값 (0.0 ~ 1.0)
	UFUNCTION(BlueprintNativeEvent, Category = "PageScrollItem")
	void UpdateFocusState(bool bInFocused, float InScale);

	/// @brief 페이지 콘텐츠를 설정합니다 (Blueprint에서 재정의).
	/// @param [in] Index 페이지 인덱스
	UFUNCTION(BlueprintImplementableEvent, Category = "PageScrollItem")
	void SetPageContent(int32 Index);

	/// @brief 페이지 인덱스를 반환합니다.
	/// @return 페이지 인덱스
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "PageScrollItem")
	int32 GetPageIndex() const { return pageIndex; }

	/// @brief 포커스 상태를 반환합니다.
	/// @return 포커스 여부
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "PageScrollItem")
	bool IsFocused() const { return bIsFocused; }

public:
	/// @brief 포커스 변경 이벤트
	UPROPERTY(BlueprintAssignable, Category = "PageScrollItem|Events")
	FOnItemFocusChanged OnItemFocusChanged;

private:
	/// @brief 이 페이지를 소유한 부모 ScrollView
	TWeakObjectPtr<class UPageScrollView> ownerScrollView;

	/// @brief 이 페이지의 인덱스
	int32 pageIndex = -1;

	/// @brief 현재 포커스 상태
	bool bIsFocused = false;
};
