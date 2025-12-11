// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file UFadeWidget.cpp
 * @brief 화면 페이드 인/아웃 효과 위젯 구현
 */

#include "UFadeWidget.h"
#include "Animation/WidgetAnimation.h"
#include "Components/Image.h"
#include "GameLogging.h"

void UFadeWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 초기 상태: 투명 (보이지 않음)
	if (FadeImage)
	{
		FadeImage->SetVisibility(ESlateVisibility::HitTestInvisible);
		FadeImage->SetOpacity(0.0f);
	}
}

void UFadeWidget::FadeOut(float Duration)
{
	if (!FadeOutAnim)
	{
		PRINTLOG(TEXT("UFadeWidget::FadeOut - FadeOutAnim is null"));

		// 애니메이션이 없으면 즉시 완료 처리
		if (FadeImage)
		{
			FadeImage->SetOpacity(1.0f);
		}
		OnFadeOutComplete.Broadcast();
		return;
	}

	// 애니메이션 완료 시 콜백 바인딩
	FWidgetAnimationDynamicEvent AnimationFinishedDelegate;
	AnimationFinishedDelegate.BindDynamic(this, &UFadeWidget::OnFadeOutAnimationFinished);

	// 기존 바인딩 제거 후 새로 바인딩
	BindToAnimationFinished(FadeOutAnim, AnimationFinishedDelegate);

	// 페이드 아웃 애니메이션 재생 (속도 조절로 Duration 적용)
	float AnimSpeed = FadeOutAnim->GetEndTime() / Duration;
	PlayAnimation(FadeOutAnim, 0.0f, 1, EUMGSequencePlayMode::Forward, AnimSpeed);

	PRINTLOG(TEXT("UFadeWidget::FadeOut - Animation started (Duration: %.2f)"), Duration);
}

void UFadeWidget::FadeIn(float Duration)
{
	if (!FadeInAnim)
	{
		PRINTLOG(TEXT("UFadeWidget::FadeIn - FadeInAnim is null"));

		// 애니메이션이 없으면 즉시 완료 처리
		if (FadeImage)
		{
			FadeImage->SetOpacity(0.0f);
		}
		OnFadeInComplete.Broadcast();
		return;
	}

	// 애니메이션 완료 시 콜백 바인딩
	FWidgetAnimationDynamicEvent AnimationFinishedDelegate;
	AnimationFinishedDelegate.BindDynamic(this, &UFadeWidget::OnFadeInAnimationFinished);

	// 기존 바인딩 제거 후 새로 바인딩
	BindToAnimationFinished(FadeInAnim, AnimationFinishedDelegate);

	// 페이드 인 애니메이션 재생 (속도 조절로 Duration 적용)
	float AnimSpeed = FadeInAnim->GetEndTime() / Duration;
	PlayAnimation(FadeInAnim, 0.0f, 1, EUMGSequencePlayMode::Forward, AnimSpeed);

	PRINTLOG(TEXT("UFadeWidget::FadeIn - Animation started (Duration: %.2f)"), Duration);
}

void UFadeWidget::OnFadeOutAnimationFinished()
{
	PRINTLOG(TEXT("UFadeWidget::OnFadeOutAnimationFinished - FadeOut completed"));
	OnFadeOutComplete.Broadcast();
}

void UFadeWidget::OnFadeInAnimationFinished()
{
	PRINTLOG(TEXT("UFadeWidget::OnFadeInAnimationFinished - FadeIn completed"));
	OnFadeInComplete.Broadcast();
}
