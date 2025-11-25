// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UTextureButton.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTextureButtonClickedEvent);

/**
 * @brief 심플한 텍스쳐 기반 버튼 위젯
 * 
 * Button이 배경 텍스쳐를 가지고, 그 안에 Image가 심볼로 표시됩니다.
 * - Button: 배경 이미지 (ButtonTexture) + 크기 조정
 * - Image_Symbol: 버튼 내부의 아이콘/심볼 (SymbolTexture)
 */
UCLASS()
class ONEPIECE_API UTextureButton : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * @brief 버튼 배경 텍스쳐 설정
	 * @param InTexture 설정할 배경 텍스쳐
	 */
	UFUNCTION(BlueprintCallable, Category = "TextureButton")
	void SetButtonTexture(UTexture2D* InTexture);

	/**
	 * @brief 심볼 이미지 텍스쳐 설정
	 * @param InTexture 설정할 심볼 텍스쳐
	 */
	UFUNCTION(BlueprintCallable, Category = "TextureButton")
	void SetSymbolTexture(UTexture2D* InTexture);

	/**
	 * @brief 버튼 크기 설정
	 * @param InWidth 버튼 너비
	 * @param InHeight 버튼 높이
	 */
	UFUNCTION(BlueprintCallable, Category = "TextureButton")
	void SetButtonSize(float InWidth, float InHeight);

	/**
	 * @brief 버튼 활성화 상태 설정
	 * @param bInEnabled 활성화 여부
	 */
	UFUNCTION(BlueprintCallable, Category = "TextureButton")
	void SetButtonEnabled(bool bInEnabled);

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

private:
	void ApplyTextures();
	void ApplySymbolColor(const FLinearColor& Color);

	UFUNCTION()
	void HandleClicked();

	UFUNCTION()
	void HandleHovered();

	UFUNCTION()
	void HandleUnhovered();

	UFUNCTION()
	void HandlePressed();

	UFUNCTION()
	void HandleReleased();

public:
	/// 버튼 클릭 이벤트
	UPROPERTY(BlueprintAssignable, Category = "Event")
	FOnTextureButtonClickedEvent OnButtonClickedEvent;

	/// 버튼 배경 텍스쳐
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style", meta = (ExposeOnSpawn = true))
	UTexture2D* ButtonTexture = nullptr;

	/// 심볼 이미지 텍스쳐
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style", meta = (ExposeOnSpawn = true))
	UTexture2D* SymbolTexture = nullptr;

	/// 버튼 너비
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style", meta = (ExposeOnSpawn = true))
	float ButtonWidth = 100.0f;

	/// 버튼 높이
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style", meta = (ExposeOnSpawn = true))
	float ButtonHeight = 100.0f;

	/// Normal 상태 심볼 색상
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style", meta = (ExposeOnSpawn = true))
	FLinearColor NormalSymbolColor = FLinearColor::White;

	/// Hover 상태 심볼 색상
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style", meta = (ExposeOnSpawn = true))
	FLinearColor HoverSymbolColor = FLinearColor(1.2f, 1.2f, 1.2f, 1.0f);

	/// Press 상태 심볼 색상
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style", meta = (ExposeOnSpawn = true))
	FLinearColor PressSymbolColor = FLinearColor(0.8f, 0.8f, 0.8f, 1.0f);

	/// Disabled 상태 심볼 색상
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style", meta = (ExposeOnSpawn = true))
	FLinearColor DisabledSymbolColor = FLinearColor(0.5f, 0.5f, 0.5f, 1.0f);

private:
	UPROPERTY(meta = (BindWidget))
	class UButton* Button_Main;

	UPROPERTY(meta = (BindWidget))
	class UImage* Image_Symbol;

	bool bEnabled = true;
};
