// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UHoverButton.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHoverButtonClickedEvent);

UCLASS()
class ONEPIECE_API UHoverButton : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetButtonEnabled(bool bInEnabled);
	
protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
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
	UPROPERTY(BlueprintAssignable, Category = "Event")
	FOnHoverButtonClickedEvent OnButtonClickedEvent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Style", meta=(ExposeOnSpawn=true))
	FText LabelText = FText::FromString("Button");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Style", meta=(ExposeOnSpawn=true))
	int32 FontSize = 48;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Style", meta=(ExposeOnSpawn=true))
	FLinearColor NormalTextColor = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Style", meta=(ExposeOnSpawn=true))
	FLinearColor HoverTextColor = FLinearColor(0.8f, 0.9f, 1.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Style", meta=(ExposeOnSpawn=true))
	FLinearColor PressTextColor = FLinearColor(0.6f, 0.7f, 1.f);
	
private:
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
};