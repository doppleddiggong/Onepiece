// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "UTabButtonGroup.h"
#include "UTabButton.h"
#include "UTabIndicator.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetTree.h"

void UTabButtonGroup::NativePreConstruct()
{
	Super::NativePreConstruct();

	// 에디터 프리뷰를 위해 탭을 재구성
	ApplyTab();
}

void UTabButtonGroup::NativeConstruct()
{
	Super::NativeConstruct();

	// 인디케이터 이동 완료 이벤트 바인딩 및 설정
	if (TabIndicator)
	{
		TabIndicator->OnMoveCompleted.RemoveDynamic(this, &UTabButtonGroup::OnMoveCompleted);
		TabIndicator->OnMoveCompleted.AddDynamic(this, &UTabButtonGroup::OnMoveCompleted);

		// 애니메이션 속도 설정
		TabIndicator->SetAnimationSpeed(IndicatorAnimationSpeed);

		// TabIndicator 크기를 TabSize로 자동 설정
		UCanvasPanelSlot* indicatorSlot = Cast<UCanvasPanelSlot>(TabIndicator->Slot);
		if (indicatorSlot)
		{
			indicatorSlot->SetSize(TabSize);
		}
	}

	// 탭 구성
	ApplyTab();

	// 기본 탭 선택 (이벤트는 발생시키지 않음)
	OnSelectTab(DefaultTabIndex, false);
}

void UTabButtonGroup::ApplyTab()
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
		OnTabSelected.Broadcast(CurTabIndex);
}

void UTabButtonGroup::UpdateIndicatorPosition()
{
	// 인디케이터 유효성 검사
	if (!TabIndicator)
		return;

	// 고정 폭 기반으로 X 위치 계산: X = TabSize.X * TabIndex
	FVector2D targetPosition(TabSize.X * CurTabIndex, 0.0f);

	// UTabIndicator에게 이동 요청
	TabIndicator->MoveTo(targetPosition, bAnimateIndicator);
}

void UTabButtonGroup::OnMoveCompleted()
{
	// 인디케이터 이동이 완료되었을 때 수행할 작업
	// 예: 사운드 재생, 추가 이벤트 발생 등
}