// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "UToastWidget.h"

#include "UAutoDespawnItem.h"
#include "UBroadcastManager.h"
#include "UTutorMessage.h"
#include "Components/HorizontalBox.h"

void UToastWidget::NativeConstruct()
{
	if (auto BM = UBroadcastManager::Get(GetWorld()))
	{
		BM->OnTutorMessage.AddDynamic(this, &UToastWidget::OnTutorMessage);
		BM->OnAddItemToBoxList.AddDynamic(this, &UToastWidget::AddItemToBoxList);
	}

	// TutorMessage 초기화 및 애니메이션 콜백 바인딩
	InitTutorMessage();
}


void UToastWidget::InitTutorMessage()
{
	if (TutorMessage)
	{
		TutorMessage->SetVisibility(ESlateVisibility::HitTestInvisible);
		
		FWidgetAnimationDynamicEvent HideDelegate;
		HideDelegate.BindDynamic(this, &UToastWidget::OnTutorHideComplete);
		BindToAnimationFinished(TutorHideAnim, HideDelegate);

		PlayAnimation(TutorHideAnim);
		
		bIsTutorVisible = false;
	}
}


void UToastWidget::OnTutorMessage(const FText& NewMessage)
{
	if (!TutorMessage)
		return;

	// 기존 타이머가 있으면 클리어
	if (TutorHideTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(TutorHideTimerHandle);
	}

	if (bIsTutorVisible)
	{
		// 표시 중이면 Hide → SetText → Show
		PendingMessage = NewMessage;
		bHasPendingMessage = true;
		
		PlayAnimation(TutorHideAnim);
	}
	else
	{
		// 숨김 상태면 바로 SetText → Show
		TutorMessage->SetMessageText(NewMessage);
		
		if (TutorShowAnim)
		{
			PlayAnimation(TutorShowAnim);
			bIsTutorVisible = true;

			// 자동 Hide 타이머 시작
			StartTutorHideTimer();
		}
	}
}

void UToastWidget::OnTutorHideComplete()
{
	bIsTutorVisible = false;
	
	if (bHasPendingMessage && TutorMessage)
	{
		bHasPendingMessage = false;
		TutorMessage->SetMessageText(PendingMessage);
		
		if (TutorShowAnim)
		{
			PlayAnimation(TutorShowAnim);
			bIsTutorVisible = true;

			// 자동 Hide 타이머 시작
			StartTutorHideTimer();
		}
	}
}

void UToastWidget::StartTutorHideTimer()
{
	// 기존 타이머 클리어 (안전성을 위해)
	if (TutorHideTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(TutorHideTimerHandle);
	}

	// 지정된 시간 후 자동으로 Hide 애니메이션 재생
	GetWorld()->GetTimerManager().SetTimer(
		TutorHideTimerHandle,
		[this]()
		{
			if (bIsTutorVisible)
			{
				PlayAnimation(TutorHideAnim);
			}
		},
		TutorMessageDisplayDuration,
		false
	);
}

void UToastWidget::AddItemToBoxList(const TArray<FResultStatData>& InDataList)
{
	if (InDataList.Num() == 0)
		return;

	// 기존 타이머가 있으면 클리어
	if (ItemAddTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(ItemAddTimerHandle);
	}

	// 대기 리스트에 데이터 복사 및 인덱스 초기화
	PendingItemDataList = InDataList;
	CurItemIndex = 0;

	// 첫 번째 아이템은 즉시 추가
	ProcessNextItem();
}


void UToastWidget::AddItemToBoxItem(const FResultStatData& InData)
{
	if (!ItemHorizontalBox || !ItemWidgetClass )
		return;

	// 새 아이템 생성
	if (auto SpawnItem = CreateWidget<UAutoDespawnItem>(GetWorld(), ItemWidgetClass))
	{
		ItemHorizontalBox->AddChild(SpawnItem);

		SpawnItem->InitData(InData);
	}
}

void UToastWidget::ProcessNextItem()
{
	if (CurItemIndex >= PendingItemDataList.Num())
	{
		// 모든 아이템 추가 완료
		PendingItemDataList.Empty();
		CurItemIndex = 0;
		return;
	}

	// 현재 인덱스의 아이템 추가
	AddItemToBoxItem(PendingItemDataList[CurItemIndex]);
	CurItemIndex++;

	// 다음 아이템이 있으면 타이머 설정
	if (CurItemIndex < PendingItemDataList.Num())
	{
		GetWorld()->GetTimerManager().SetTimer(
			ItemAddTimerHandle,
			this,
			&UToastWidget::ProcessNextItem,
			ItemAddInterval,
			false
		);
	}
	else
	{
		// 모든 아이템 추가 완료
		PendingItemDataList.Empty();
		CurItemIndex = 0;
	}
}