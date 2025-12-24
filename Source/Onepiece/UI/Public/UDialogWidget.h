// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FEaseHelper.h"
#include "UDialogWidget.generated.h"

/// @file UDialogWidget.h
/// @brief 텍스트 출력하는 다이얼로그 위젯을 선언합니다.
/**
 * @brief 텍스트 출력하는 다이얼로그 위젯입니다.
 * @details 화면에 잠시 나타났다가 사라지는 토스트 메시지나 간단한 알림을 표시하는 데 사용됩니다.
 */
UCLASS()
class ONEPIECE_API UDialogWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    /// @brief 위젯 초기화 및 기본 색상을 설정합니다.
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    /// @brief 대화 텍스트를 표시하고 강조 색상을 적용합니다.
    /// @param InString [in] 출력할 대화 문자열입니다.
    UFUNCTION(BlueprintCallable, Category ="Chat")
    void ShowDialog(FString InString);

    void HideDialogImmediately();

    /**
     * @brief 다이얼로그 오픈 애니메이션을 재생합니다
     */
    UFUNCTION(BlueprintCallable, Category = "Dialog|Animation")
    void OpenAnimation();

private:
    void HandleHideTimerExpired();

    /**
     * @brief 스크립트 오픈 애니메이션 업데이트
     */
    void UpdateAnimation(float InDeltaTime);
    
public:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UBorder> DialogBorder;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UTextBlock> DialogText;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="Chat")
    FLinearColor ActivateColor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="Chat")
    FLinearColor DeactivateColor;

    // ========================================
    // Blueprint Open Animation
    // ========================================

    /**
     * @brief 블루프린트에서 정의한 오픈 애니메이션
     * @details 설정된 경우 스크립트 애니메이션 대신 이 애니메이션을 재생합니다.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog|Animation", meta = (BindWidgetOptional))
    UWidgetAnimation* BlueprintOpenAnimation;

    // ========================================
    // Script Open Animation
    // ========================================

    /** 애니메이션 재생 중 여부 */
    bool bIsOpenAnim = false;

    /** 경과 시간 */
    float OpenElapsedTime = 0.0f;

    /** 애니메이션 지속 시간 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog|Animation")
    float OpenDuration = 0.2f;

    /** 시작 스케일 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog|Animation")
    float OpenStartScale = 0.8f;

    /** 목표 스케일 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog|Animation")
    float OpenTargetScale = 1.0f;

    /** 애니메이션 중심점 (0~1 범위) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog|Animation")
    FVector2D OpenPivot = FVector2D(0.5f, 0.5f);

    /**
     * @brief Easing 타입
     * @details 애니메이션의 보간 곡선을 선택합니다. 기본값은 EaseOutBack (탄성 효과)
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog|Animation")
    EEaseType OpenEaseType = EEaseType::EaseOutBack;

private:
    /// @brief Delay 태스크 핸들 (이전 태스크 취소용)
    FTimerHandle HideTimerHandle;
};
