// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UImageButton.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnImageButtonClickedEvent);

/**
 * UImageButton
 *
 * UHoverButton과 동일한 호버/프레스 애니메이션 기능을 제공하며,
 * 추가로 Border 이미지 설정 및 고정 크기 설정이 가능한 버튼 위젯입니다.
 */
UCLASS()
class ONEPIECE_API UImageButton : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * 버튼의 활성화/비활성화 상태를 설정합니다.
	 *
	 * @param bInEnabled true면 활성화, false면 비활성화
	 */
	UFUNCTION(BlueprintCallable)
	void SetButtonEnabled(bool bInEnabled);

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	/**
	 * 설정된 스타일 속성을 위젯에 적용합니다.
	 */
	void ApplyStyle();

	UFUNCTION()
	void HandleHovered();

	UFUNCTION()
	void HandleUnhovered();

	UFUNCTION()
	void HandlePressed();

	UFUNCTION()
	void HandleReleased();

	UFUNCTION()
	void HandleClicked();

public:
	/// 버튼 클릭 시 발생하는 이벤트
	UPROPERTY(BlueprintAssignable, Category = "Event")
	FOnImageButtonClickedEvent OnButtonClickedEvent;

	// ========== 텍스트 스타일 ==========

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Style|Text", meta=(ExposeOnSpawn=true))
	FText LabelText = FText::FromString("Button");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Style|Text", meta=(ExposeOnSpawn=true))
	int32 FontSize = 48;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Style|Text", meta=(ExposeOnSpawn=true))
	int32 FontOutlineSize = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Style|Text", meta=(ExposeOnSpawn=true))
	FLinearColor NormalTextColor = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Style|Text", meta=(ExposeOnSpawn=true))
	FLinearColor HoverTextColor = FLinearColor(0.8f, 0.9f, 1.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Style|Text", meta=(ExposeOnSpawn=true))
	FLinearColor PressTextColor = FLinearColor(0.6f, 0.7f, 1.f);

	// ========== 버튼 배경 색상 ==========

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Style|Background", meta=(ExposeOnSpawn=true))
	FLinearColor NormalButtonColor = FLinearColor(0.1f, 0.1f, 0.1f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Style|Background", meta=(ExposeOnSpawn=true))
	FLinearColor HoverButtonColor = FLinearColor(0.2f, 0.2f, 0.2f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Style|Background", meta=(ExposeOnSpawn=true))
	FLinearColor PressButtonColor = FLinearColor(0.05f, 0.05f, 0.05f);

	// ========== Border 이미지 ==========

	/// Border에 적용할 이미지 (null이면 색상만 사용)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Style|Border", meta=(ExposeOnSpawn=true))
	UTexture2D* BorderImage = nullptr;

	// ========== 크기 설정 ==========

	/// 고정 크기 사용 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Style|Size", meta=(ExposeOnSpawn=true))
	bool bUseFixedSize = false;

	/// 고정 너비 (bUseFixedSize가 true일 때 적용)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Style|Size", meta=(ExposeOnSpawn=true, EditCondition="bUseFixedSize"))
	float FixedWidth = 200.0f;

	/// 고정 높이 (bUseFixedSize가 true일 때 적용)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Style|Size", meta=(ExposeOnSpawn=true, EditCondition="bUseFixedSize"))
	float FixedHeight = 100.0f;

private:
	UPROPERTY(meta = (BindWidget))
	class USizeBox* SizeBox_Root;

	UPROPERTY(meta = (BindWidget))
	class UButton* Button_Main;

	UPROPERTY(meta = (BindWidget))
	class UBorder* Border_BG;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_Label;

private:
	// 현재 값
	FVector2D CurrentScale = FVector2D(1.f, 1.f);
	float CurrentBrightness = 1.0f;

	// 목표값
	FVector2D TargetScale = FVector2D(1.f, 1.f);
	float TargetBrightness = 1.f;

	// 애니메이션 속도
	float LerpSpeed = 20.f;

	bool bEnabled = true;

	// Border 이미지용 캐시된 Brush (GetBrush()가 없으므로 캐싱 필요)
	FSlateBrush CachedBorderBrush;
	bool bBorderBrushInitialized = false;
};
