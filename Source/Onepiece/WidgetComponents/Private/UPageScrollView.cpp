// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "UPageScrollView.h"
#include "UPageScrollItem.h"
#include "UPageScrollDot.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "TimerManager.h"

void UPageScrollView::NativeConstruct()
{
	Super::NativeConstruct();

	// 뷰포트 크기 계산 (SizeBox의 크기)
	if (UCanvasPanelSlot* RootSlot = Cast<UCanvasPanelSlot>(Slot))
	{
		ViewportSize = RootSlot->GetSize();
	}

	// ViewportSize 계산 실패 시 PageSize 사용
	if (ViewportSize.X <= 1.0f || ViewportSize.Y <= 1.0f)
	{
		ViewportSize = PageSize;
	}

	// 기본 페이지로 이동 (애니메이션 없이)
	if (pageItemList.Num() > 0 && pageItemList.IsValidIndex(DefaultPageIndex))
	{
		MovePage(DefaultPageIndex, false);
	}
}

void UPageScrollView::NativeDestruct()
{
	Super::NativeDestruct();

	// 타이머 정리
	StopAnimation();
}

FReply UPageScrollView::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (!InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
		return FReply::Unhandled();

	// 애니메이션 중지
	StopAnimation();

	// 드래그 시작
	bIsDragging = true;
	dragStartPosition = InMouseEvent.GetScreenSpacePosition();
	dragCurrentPosition = dragStartPosition;
	dragStartTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
	PrevPageIndex = CurPageIndex;

	// TakeWidget()으로 SWidget을 가져와서 Mouse Capture
	TSharedPtr<SWidget> CachedWidget = GetCachedWidget();
	if (CachedWidget.IsValid())
	{
		return FReply::Handled().CaptureMouse(CachedWidget.ToSharedRef());
	}

	return FReply::Handled();
}

FReply UPageScrollView::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (!bIsDragging)
		return FReply::Unhandled();

	bIsDragging = false;

	// 드래그 거리 계산
	FVector2D dragDelta = dragCurrentPosition - dragStartPosition;
	float dragDistance = dragDelta.Size();

	// 작은 드래그는 클릭으로 처리 (버튼 이벤트 전달)
	if (dragDistance <= 10.0f)
	{
		return FReply::Handled().ReleaseMouseCapture();
	}

	// 드래그 속도 계산 (픽셀/초)
	float dragTime = GetWorld() ? (GetWorld()->GetTimeSeconds() - dragStartTime) : 0.01f;
	float velocity = dragDistance / FMath::Max(dragTime, 0.01f);

	// 목표 페이지 계산
	int32 targetPage = CalculateTargetPage(dragDelta, velocity);

	// 목표 페이지로 이동
	MovePage(targetPage, true);

	return FReply::Handled().ReleaseMouseCapture();
}

FReply UPageScrollView::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (!bIsDragging)
		return FReply::Unhandled();

	dragCurrentPosition = InMouseEvent.GetScreenSpacePosition();

	// 드래그 중 실시간 컨테이너 위치 업데이트
	UpdateContainerPositionDuringDrag();

	return FReply::Handled();
}

void UPageScrollView::MovePage(int32 PageIndex, bool bAnimate)
{
	if (!pageItemList.IsValidIndex(PageIndex))
		return;

	int32 prevPage = CurPageIndex;
	CurPageIndex = PageIndex;

	FVector2D targetPosition = GetPagePosition(PageIndex);

	// Phase 1: 애니메이션 없이 즉시 이동
	if (!bAnimate || SnapAnimationSpeed <= 0.0f)
	{
		SetContainerPosition(targetPosition);

		// 페이지 변경 이벤트 발생
		if (prevPage != CurPageIndex)
		{
			OnPageChanged.Broadcast(prevPage, CurPageIndex);
		}

		// 인디케이터 업데이트
		UpdatePageDotIndicator();

		// 스케일 효과 업데이트
		if (bEnableScaleEffect)
		{
			UpdateItemScales();
		}
		return;
	}

	// Phase 2에서 구현: 애니메이션 이동
	UCanvasPanelSlot* containerSlot = Cast<UCanvasPanelSlot>(PageContainer->Slot);
	if (!containerSlot)
	{
		SetContainerPosition(targetPosition);
		return;
	}

	animStartPosition = containerSlot->GetPosition();
	animTargetPosition = targetPosition;
	animElapsedTime = 0.0f;
	bIsAnimating = true;

	// 타이머 시작
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(snapAnimTimerHandle);
		GetWorld()->GetTimerManager().SetTimer(
			snapAnimTimerHandle,
			this,
			&UPageScrollView::TickSnapAnimation,
			0.016f, // ~60 FPS
			true
		);
	}
}

void UPageScrollView::SetNumberOfPages(int32 ItemCount)
{
	if (!PageContainer || !PageItemClass)
		return;

	// 기존 페이지 제거
	pageItemList.Empty();
	PageContainer->ClearChildren();

	// 새 페이지 생성
	for (int32 i = 0; i < ItemCount; ++i)
	{
		UPageScrollItem* newItem = CreateWidget<UPageScrollItem>(GetWorld(), PageItemClass);
		if (!newItem)
			continue;

		// 페이지 초기화
		newItem->InitData(i, this);

		// PageContainer에 추가
		UCanvasPanelSlot* slot = PageContainer->AddChildToCanvas(newItem);
		if (slot)
		{
			// 페이지 아이템 위치 설정 (0부터 오른쪽으로 배치)
			float stride = GetPageStride();
			FVector2D itemPosition;
			if (ScrollDirection == EPageScrollDirection::Horizontal)
				itemPosition = FVector2D(stride * i, 0.0f);
			else
				itemPosition = FVector2D(0.0f, stride * i);

			slot->SetAnchors(FAnchors(0.0f, 0.0f, 0.0f, 0.0f)); // 왼쪽 위 고정
			slot->SetAlignment(FVector2D(0.0f, 0.0f));
			slot->SetPosition(itemPosition);
			slot->SetSize(PageSize);
		}

		pageItemList.Add(newItem);
	}

	// 인디케이터 업데이트
	if (PageDotIndicator)
	{
		PageDotIndicator->SetNumberOfPages(ItemCount);
	}

	// 기본 페이지로 이동
	if (pageItemList.Num() > 0 && pageItemList.IsValidIndex(DefaultPageIndex))
	{
		MovePage(DefaultPageIndex, false);
	}
}

void UPageScrollView::NextPage()
{
	int32 targetPage = FMath::Clamp(CurPageIndex + 1, 0, pageItemList.Num() - 1);
	MovePage(targetPage, true);
}

void UPageScrollView::PrevPage()
{
	int32 targetPage = FMath::Clamp(CurPageIndex - 1, 0, pageItemList.Num() - 1);
	MovePage(targetPage, true);
}

int32 UPageScrollView::CalculateTargetPage(FVector2D DragDelta, float Velocity) const
{
	float delta = (ScrollDirection == EPageScrollDirection::Horizontal)
		? DragDelta.X
		: DragDelta.Y;

	int32 targetPage = CurPageIndex;

	// 빠른 스와이프 감지
	if (Velocity >= FastSwipeThreshold)
	{
		// 양수: 오른쪽/아래로 드래그 → 이전 페이지
		// 음수: 왼쪽/위로 드래그 → 다음 페이지
		targetPage = (delta > 0) ? CurPageIndex - 1 : CurPageIndex + 1;
	}
	else
	{
		// 느린 드래그: 가장 가까운 페이지로 스냅
		float dragRatio = delta / GetPageStride();
		int32 dragPages = FMath::RoundToInt(dragRatio);
		targetPage = CurPageIndex - dragPages; // 음수 방향 (Unity와 동일)
	}

	// 범위 제한
	return FMath::Clamp(targetPage, 0, pageItemList.Num() - 1);
}

void UPageScrollView::TickSnapAnimation()
{
	// Canvas Slot 유효성 검사
	UCanvasPanelSlot* containerSlot = Cast<UCanvasPanelSlot>(PageContainer->Slot);
	if (!containerSlot)
	{
		StopAnimation();
		return;
	}

	// 애니메이션 진행
	animElapsedTime += 0.016f; // ~60 FPS 기준
	const float Alpha = FMath::Clamp(animElapsedTime / SnapAnimationSpeed, 0.0f, 1.0f);

	// Ease-out curve 적용
	float easedAlpha = FMath::InterpEaseOut(0.0f, 1.0f, Alpha, 2.0f);

	// Lerp를 사용한 부드러운 이동
	FVector2D currentPosition = FMath::Lerp(animStartPosition, animTargetPosition, easedAlpha);
	containerSlot->SetPosition(currentPosition);

	// 스케일 효과 업데이트
	if (bEnableScaleEffect)
	{
		UpdateItemScales();
	}

	// 애니메이션 완료 확인
	if (Alpha >= 1.0f)
	{
		StopAnimation();

		// 페이지 변경 이벤트 발생
		if (PrevPageIndex != CurPageIndex)
		{
			OnPageChanged.Broadcast(PrevPageIndex, CurPageIndex);
		}

		// 인디케이터 업데이트
		UpdatePageDotIndicator();
	}
}

void UPageScrollView::StopAnimation()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(snapAnimTimerHandle);
	}

	bIsAnimating = false;
}

FVector2D UPageScrollView::GetPagePosition(int32 PageIndex) const
{
	float stride = GetPageStride();
	float offset = -stride * PageIndex; // 음수: 컨테이너를 왼쪽/위로 이동

	return ScrollDirection == EPageScrollDirection::Horizontal ?  FVector2D(offset, 0.0f) : FVector2D(0.0f, offset);
}

void UPageScrollView::SetContainerPosition(FVector2D Position)
{
	if (UCanvasPanelSlot* containerSlot = Cast<UCanvasPanelSlot>(PageContainer->Slot))
	{
		containerSlot->SetPosition(Position);
	}
}

void UPageScrollView::UpdatePageDotIndicator()
{
	if (PageDotIndicator)
	{
		PageDotIndicator->SetCurrentPage(CurPageIndex);
	}
}

void UPageScrollView::UpdateItemScales()
{
	if (!bEnableScaleEffect)
		return;

	UCanvasPanelSlot* containerSlot = Cast<UCanvasPanelSlot>(PageContainer->Slot);
	if (!containerSlot)
		return;

	FVector2D containerPos = containerSlot->GetPosition();
	// ViewportSize 대신 PageSize 사용 (ViewportSize 계산이 실패할 수 있음)
	float viewportCenter = (ScrollDirection == EPageScrollDirection::Horizontal)
		? PageSize.X * 0.5f
		: PageSize.Y * 0.5f;

	for (int32 i = 0; i < pageItemList.Num(); ++i)
	{
		UPageScrollItem* item = pageItemList[i];
		if (!item)
			continue;

		// 페이지 아이템의 실제 위치 (0부터 오른쪽으로 배치됨)
		float stride = GetPageStride();
		FVector2D pagePos;
		if (ScrollDirection == EPageScrollDirection::Horizontal)
			pagePos = FVector2D(stride * i, 0.0f);
		else
			pagePos = FVector2D(0.0f, stride * i);

		float pageCenter = (ScrollDirection == EPageScrollDirection::Horizontal)
			? (containerPos.X + pagePos.X + PageSize.X * 0.5f)
			: (containerPos.Y + pagePos.Y + PageSize.Y * 0.5f);

		float distance = FMath::Abs(pageCenter - viewportCenter);
		float maxDistance = GetPageStride();
		float normalizedDistance = FMath::Clamp(distance / maxDistance, 0.0f, 1.0f);

		// Lerp from focus scale to side scale
		float scale = FMath::Lerp(FocusScale, SideScale, normalizedDistance);

		item->SetRenderScale(FVector2D(scale, scale));
		item->UpdateFocusState(normalizedDistance < 0.1f, scale);
	}
}

void UPageScrollView::UpdateContainerPositionDuringDrag()
{
	if (!bIsDragging)
		return;

	UCanvasPanelSlot* containerSlot = Cast<UCanvasPanelSlot>(PageContainer->Slot);
	if (!containerSlot)
		return;

	// 드래그 델타 계산
	FVector2D dragDelta = dragCurrentPosition - dragStartPosition;

	// 이전 위치에서 드래그 델타만큼 이동
	FVector2D basePosition = GetPagePosition(PrevPageIndex);
	FVector2D newPosition = basePosition;

	if (ScrollDirection == EPageScrollDirection::Horizontal)
		newPosition.X += dragDelta.X;
	else
		newPosition.Y += dragDelta.Y;

	containerSlot->SetPosition(newPosition);

	// 스케일 효과 업데이트
	if (bEnableScaleEffect)
	{
		UpdateItemScales();
	}
}