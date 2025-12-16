// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "UTabButtonGroup.h"
#include "UTabButton.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetTree.h"
#include "TimerManager.h"

void UTabButtonGroup::NativePreConstruct()
{
	Super::NativePreConstruct();

	// 에디터 프리뷰를 위해 탭을 재구성
	RebuildTabs();
}

void UTabButtonGroup::NativeConstruct()
{
	Super::NativeConstruct();

	// 탭 구성
	RebuildTabs();

	// 기본 탭 선택 (이벤트는 발생시키지 않음)
	OnSelectTab(DefaultTabIndex, false);
}

void UTabButtonGroup::RebuildTabs()
{
	// TabButtonClass 유효성 검사
	if (!TabButtonClass)
		return;

	// TabContainer 유효성 검사
	if (!TabContainer)
		return;

	// 기존 탭 버튼들 제거
	TabContainer->ClearChildren();
	TabButtonList.Empty();

	// TabLabels 배열을 순회하며 탭 버튼 생성
	for (int32 i = 0; i < TabLabels.Num(); ++i)
	{
		// UTabButton 인스턴스 생성
		UTabButton* newButton = CreateWidget<UTabButton>(this, TabButtonClass);
		if (!newButton)
			continue;

		// 버튼 초기화
		newButton->InitData(i, this);
		newButton->SetLabel(TabLabels[i]);

		// 클릭 이벤트 바인딩
		newButton->OnTabButtonClicked.RemoveDynamic(this, &UTabButtonGroup::OnTabClicked);
		newButton->OnTabButtonClicked.AddDynamic(this, &UTabButtonGroup::OnTabClicked);

		// HorizontalBox에 추가
		UHorizontalBoxSlot* slot = TabContainer->AddChildToHorizontalBox(newButton);
		if (slot)
		{
			// 슬롯 설정 (균등 분배)
			slot->SetHorizontalAlignment(HAlign_Fill);
			slot->SetVerticalAlignment(VAlign_Fill);
			slot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
		}

		// 인스턴스 배열에 추가
		TabButtonList.Add(newButton);
	}
}

void UTabButtonGroup::OnTabClicked(int32 TabIndex)
{
	OnSelectTab(TabIndex, true);
}

void UTabButtonGroup::OnSelectTab(int32 TabIndex, bool bBroadcastEvent)
{
	// 인덱스 유효성 검사
	if (!TabButtonList.IsValidIndex(TabIndex))
		return;

	// 이미 선택된 탭이면 무시
	if (CurTabIndex == TabIndex)
		return;

	// 현재 탭 인덱스 업데이트
	CurTabIndex = TabIndex;

	// 모든 탭 버튼의 선택 상태 업데이트
	for (int32 i = 0; i < TabButtonList.Num(); ++i)
	{
		if (TabButtonList[i])
		{
			TabButtonList[i]->SetSelected(i == CurTabIndex);
		}
	}

	// 인디케이터 위치 업데이트
	UpdateIndicatorPosition();

	// 이벤트 브로드캐스트
	if (bBroadcastEvent)
	{
		OnTabSelected.Broadcast(CurTabIndex);
	}
}

void UTabButtonGroup::UpdateIndicatorPosition()
{
	// 인디케이터와 현재 탭 버튼 유효성 검사
	if (!Image_SelectedIndicator || !TabButtonList.IsValidIndex(CurTabIndex))
		return;

	UTabButton* CurTabBtn = TabButtonList[CurTabIndex];
	if (!CurTabBtn)
		return;

	// 현재 탭 버튼의 지오메트리 가져오기
	FGeometry BtnGeometry = CurTabBtn->GetCachedGeometry();
	FVector2D BtnSize = BtnGeometry.GetLocalSize();
	FVector2D BtnPosition = BtnGeometry.GetAbsolutePosition();

	// 인디케이터의 CanvasPanelSlot 가져오기
	UCanvasPanelSlot* indicatorSlot = Cast<UCanvasPanelSlot>(Image_SelectedIndicator->Slot);
	if (!indicatorSlot)
		return;

	// TabContainer의 지오메트리 가져오기 (상대 위치 계산용)
	FGeometry containerGeometry = TabContainer->GetCachedGeometry();
	FVector2D containerPosition = containerGeometry.GetAbsolutePosition();

	// 목표 위치 계산 (TabContainer 기준 상대 위치)
	FVector2D targetPosition = BtnPosition - containerPosition;

	// 애니메이션 활성화 여부에 따라 처리
	if (bAnimateIndicator && IndicatorAnimationSpeed > 0.0f)
	{
		// 애니메이션 시작 위치 설정
		animStartPosition = indicatorSlot->GetPosition();
		animTargetPosition = targetPosition;
		animElapsedTime = 0.0f;

		// 타이머 시작 (기존 타이머가 있다면 제거)
		if (GetWorld())
		{
			GetWorld()->GetTimerManager().ClearTimer(indicatorAnimTimerHandle);
			GetWorld()->GetTimerManager().SetTimer(
				indicatorAnimTimerHandle,
				this,
				&UTabButtonGroup::TickIndicatorAnimation,
				0.016f, // ~60 FPS
				true
			);
		}
	}
	else
	{
		// 애니메이션 없이 즉시 이동
		indicatorSlot->SetPosition(targetPosition);
		indicatorSlot->SetSize(BtnSize);
	}
}

void UTabButtonGroup::TickIndicatorAnimation()
{
	// 인디케이터 유효성 검사
	if (!Image_SelectedIndicator)
	{
		if (GetWorld())
			GetWorld()->GetTimerManager().ClearTimer(indicatorAnimTimerHandle);
		return;
	}

	UCanvasPanelSlot* indicatorSlot = Cast<UCanvasPanelSlot>(Image_SelectedIndicator->Slot);
	if (!indicatorSlot)
	{
		if (GetWorld())
			GetWorld()->GetTimerManager().ClearTimer(indicatorAnimTimerHandle);
		return;
	}

	// 애니메이션 진행
	animElapsedTime += 0.016f; // ~60 FPS 기준
	float alpha = FMath::Clamp(animElapsedTime / IndicatorAnimationSpeed, 0.0f, 1.0f);

	// Lerp를 사용한 부드러운 이동
	FVector2D currentPosition = FMath::Lerp(animStartPosition, animTargetPosition, alpha);
	indicatorSlot->SetPosition(currentPosition);

	// 애니메이션 완료 확인
	if (alpha >= 1.0f)
	{
		// 타이머 정지
		if (GetWorld())
		{
			GetWorld()->GetTimerManager().ClearTimer(indicatorAnimTimerHandle);
		}
	}
}