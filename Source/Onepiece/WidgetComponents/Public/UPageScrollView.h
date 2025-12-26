// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EPageScrollDirection.h"
#include "UPageScrollView.generated.h"

/// @brief 페이지 변경 이벤트 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPageChanged, int32, PrevPage, int32, CurrentPage);

/**
 * @brief 페이지 단위 스크롤 뷰 위젯
 * 드래그 기반 스크롤과 자동 스냅 기능을 제공하는 메인 컨트롤러 위젯입니다.
 */
UCLASS()
class ONEPIECE_API UPageScrollView : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	/// @brief 마우스 버튼 다운 이벤트
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	/// @brief 마우스 버튼 업 이벤트
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	/// @brief 마우스 이동 이벤트
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

public:
	/// @brief 특정 페이지로 이동합니다.
	/// @param [in] PageIndex 이동할 페이지 인덱스
	/// @param [in] bAnimate 애니메이션 여부
	UFUNCTION(BlueprintCallable, Category = "PageScrollView")
	void MovePage(int32 PageIndex, bool bAnimate = true);

	/// @brief 페이지 개수를 설정하고 동적으로 생성합니다.
	/// @param [in] ItemCount 생성할 페이지 수
	UFUNCTION(BlueprintCallable, Category = "PageScrollView")
	void SetNumberOfPages(int32 ItemCount);

	/// @brief 다음 페이지로 이동합니다.
	UFUNCTION(BlueprintCallable, Category = "PageScrollView")
	void NextPage();

	/// @brief 이전 페이지로 이동합니다.
	UFUNCTION(BlueprintCallable, Category = "PageScrollView")
	void PrevPage();

	/// @brief 현재 페이지 인덱스를 반환합니다.
	/// @return 현재 페이지 인덱스
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "PageScrollView")
	int32 GetCurrentPage() const { return CurPageIndex; }

	/// @brief 총 페이지 수를 반환합니다.
	/// @return 총 페이지 수
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "PageScrollView")
	int32 GetTotalPages() const { return pageItemList.Num(); }

	/// @brief 특정 인덱스의 페이지 아이템을 반환합니다.
	/// @param [in] Index 페이지 인덱스
	/// @return 페이지 아이템 (없으면 nullptr)
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "PageScrollView")
	UPageScrollItem* GetPageAt(int32 Index) const
	{
		return pageItemList.IsValidIndex(Index) ? pageItemList[Index] : nullptr;
	}
	
private:
	/// @brief 드래그 종료 시 목표 페이지를 계산합니다.
	/// @param [in] DragDelta 드래그 이동 거리
	/// @param [in] Velocity 드래그 속도 (픽셀/초)
	/// @return 목표 페이지 인덱스
	int32 CalculateTargetPage(FVector2D DragDelta, float Velocity) const;

	/// @brief 스냅 애니메이션을 Tick합니다.
	void TickSnapAnimation();

	/// @brief 스냅 애니메이션을 중지합니다.
	void StopAnimation();

	/// @brief 페이지 인덱스에 해당하는 목표 위치를 계산합니다.
	/// @param [in] PageIndex 페이지 인덱스
	/// @return 목표 위치 (2D Vector)
	FVector2D GetPagePosition(int32 PageIndex) const;

	/// @brief 페이지 간 거리를 계산합니다.
	/// @return 페이지 크기 + 간격
	FORCEINLINE float GetPageStride() const
	{
		return (ScrollDirection == EPageScrollDirection::Horizontal)
			? (PageSize.X + PageSpacing)
			: (PageSize.Y + PageSpacing);
	}

	/// @brief 컨테이너 위치를 설정합니다.
	/// @param [in] Position 설정할 위치
	void SetContainerPosition(FVector2D Position);

	/// @brief 페이지 인디케이터를 업데이트합니다.
	void UpdatePageDotIndicator();

	/// @brief 페이지 아이템들의 스케일을 거리 기반으로 업데이트합니다.
	void UpdateItemScales();

	/// @brief 드래그 중 컨테이너 위치를 업데이트합니다.
	void UpdateContainerPositionDuringDrag();

public:
	/// @brief 스크롤 방향 (수평/수직)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PageScrollView|Layout")
	EPageScrollDirection ScrollDirection = EPageScrollDirection::Horizontal;

	/// @brief 각 페이지의 크기 (Width × Height)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PageScrollView|Layout")
	FVector2D PageSize = FVector2D(1920.0f, 1080.0f);

	/// @brief 페이지 간 간격 (픽셀)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PageScrollView|Layout")
	float PageSpacing = 0.0f;

	/// @brief 빠른 스와이프 인식 임계값 (픽셀/초)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PageScrollView|Behavior")
	float FastSwipeThreshold = 500.0f;

	/// @brief 스냅 애니메이션 속도 (초 단위)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PageScrollView|Behavior")
	float SnapAnimationSpeed = 0.3f;

	/// @brief 중앙 스케일 효과 활성화 여부
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PageScrollView|Behavior")
	bool bEnableScaleEffect = false;

	/// @brief 중앙에 있는 페이지의 스케일 값
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PageScrollView|Behavior", meta=(EditCondition="bEnableScaleEffect"))
	float FocusScale = 1.0f;

	/// @brief 중앙에서 벗어난 페이지의 스케일 값
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PageScrollView|Behavior", meta=(EditCondition="bEnableScaleEffect"))
	float SideScale = 0.8f;

	/// @brief 페이지 아이템으로 생성할 위젯 블루프린트 클래스
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PageScrollView|Data")
	TSubclassOf<class UPageScrollItem> PageItemClass;

	/// @brief 기본으로 선택될 페이지 인덱스
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PageScrollView|Data")
	int32 DefaultPageIndex = 0;

	/// @brief 페이지 아이템들을 담을 컨테이너 (드래그로 위치 이동)
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<class UCanvasPanel> PageContainer;

	/// @brief 페이지 인디케이터 (선택적)
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<class UPageScrollDot> PageDotIndicator;

	/// @brief 페이지 변경 이벤트
	UPROPERTY(BlueprintAssignable, Category = "PageScrollView|Events")
	FOnPageChanged OnPageChanged;

private:
	/// @brief 현재 선택된 페이지 인덱스
	int32 CurPageIndex = 0;

	/// @brief 이전 페이지 인덱스 (스냅 계산용)
	int32 PrevPageIndex = 0;

	/// @brief 생성된 페이지 아이템 목록
	UPROPERTY()
	TArray<TObjectPtr<class UPageScrollItem>> pageItemList;

	/// @brief 드래그 상태 추적
	bool bIsDragging = false;
	FVector2D dragStartPosition = FVector2D::ZeroVector;
	FVector2D dragCurrentPosition = FVector2D::ZeroVector;
	float dragStartTime = 0.0f;

	/// @brief 애니메이션 상태
	FTimerHandle snapAnimTimerHandle;
	FVector2D animStartPosition = FVector2D::ZeroVector;
	FVector2D animTargetPosition = FVector2D::ZeroVector;
	float animElapsedTime = 0.0f;
	bool bIsAnimating = false;

	/// @brief 뷰포트 크기 (SizeBox에서 계산)
	FVector2D ViewportSize = FVector2D::ZeroVector;
};
