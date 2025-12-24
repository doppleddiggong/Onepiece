// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file UDialogWidget.cpp
 * @brief UDialogWidget의 동작을 구현합니다.
 */
#include "UDialogWidget.h"
#include "UDelayTaskManager.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"

void UDialogWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (DialogBorder)
	{
		DialogBorder->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UDialogWidget::NativeDestruct()
{
	if (UWorld* World = GetWorld())
	{
		if (HideTimerHandle.IsValid())
		{
			World->GetTimerManager().ClearTimer(HideTimerHandle);
		}
	}

	Super::NativeDestruct();
}

void UDialogWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bIsOpenAnim)
	{
		UpdateAnimation(InDeltaTime);
	}
}

void UDialogWidget::HandleHideTimerExpired()
{
	if (!DialogBorder || !DialogText)
		return;

	HideTimerHandle.Invalidate();
	
	DialogText->SetText(FText::GetEmpty());
	DialogBorder->SetBrushColor(DeactivateColor);
	DialogBorder->SetVisibility(ESlateVisibility::Hidden);
}

void UDialogWidget::OpenAnimation()
{
	// 블루프린트에서 정의한 애니메이션이 있으면 해당 애니메이션 재생
	if (BlueprintOpenAnimation)
	{
		PlayAnimation(BlueprintOpenAnimation);
		return;
	}

	// 블루프린트 애니메이션이 없으면 기본 스크립트 애니메이션 사용
	SetRenderTransformPivot(OpenPivot);
	SetRenderScale(FVector2D(OpenStartScale, OpenStartScale));

	bIsOpenAnim = OpenDuration > KINDA_SMALL_NUMBER;
	OpenElapsedTime = 0.0f;

	if (!bIsOpenAnim)
	{
		SetRenderScale(FVector2D(OpenTargetScale, OpenTargetScale));
	}
}

void UDialogWidget::UpdateAnimation(float InDeltaTime)
{
	if (!bIsOpenAnim)
	{
		return;
	}

	OpenElapsedTime += InDeltaTime;

	const float Alpha = FMath::Clamp(OpenElapsedTime / OpenDuration, 0.0f, 1.0f);
	const float EasedAlpha = FEaseHelper::Ease(Alpha, OpenEaseType);
	const float NewScale = FMath::Lerp(OpenStartScale, OpenTargetScale, EasedAlpha);

	SetRenderScale(FVector2D(NewScale, NewScale));

	if (Alpha >= 1.0f)
	{
		bIsOpenAnim = false;
	}
}

void UDialogWidget::ShowDialog(FString InString)
{
	if (!DialogBorder || !DialogText)
		return;

	// 이전 타이머가 있으면 취소
	if (UWorld* World = GetWorld())
	{
		if (HideTimerHandle.IsValid())
			World->GetTimerManager().ClearTimer(HideTimerHandle);
	}

	DialogBorder->SetVisibility(ESlateVisibility::Visible);
	DialogBorder->SetBrushColor(ActivateColor);
	DialogText->SetText(FText::FromString(InString));

	// 오픈 애니메이션 재생
	OpenAnimation();

	// 5초 후 숨기는 타이머 설정 (약한 참조 사용)
	if (UWorld* World = GetWorld())
	{
		TWeakObjectPtr<UDialogWidget> WeakThis(this);

		World->GetTimerManager().SetTimer(
			HideTimerHandle,
			FTimerDelegate::CreateLambda([WeakThis]()
			{
				if (WeakThis.IsValid())
				{
					WeakThis->HandleHideTimerExpired();
				}
			}),
			5.0f,
			false
		);
	}
}

void UDialogWidget::HideDialogImmediately()
{
	if (!DialogBorder || !DialogText)
		return;

	if (UWorld* World = GetWorld())
	{
		if (HideTimerHandle.IsValid())
			World->GetTimerManager().ClearTimer(HideTimerHandle);
	}

	HandleHideTimerExpired();
}