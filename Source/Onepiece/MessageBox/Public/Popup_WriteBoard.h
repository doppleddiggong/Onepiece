// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "UBasePopup.h"
#include "Popup_WriteBoard.generated.h"

/**
 * 
 */
UCLASS()
class ONEPIECE_API UPopup_WriteBoard : public UBasePopup
{
	GENERATED_BODY()
	
public:
	UPopup_WriteBoard(const FObjectInitializer& ObjectInitializer);
	virtual void NativeOnInitialized() override;
	
protected:
	// Objects
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UTextureRenderTarget2D> RT_Canvas;
	
	// Widgets
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<class UCanvasPanel> CanvasPanel_WriteBoard;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<class UImage> Image_Canvas;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<class UImageButton> Button_Clear;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<class UImageButton> Button_Save;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<class UTextureButton> Button_Close;
	
	// Values
	UPROPERTY(BlueprintReadOnly)
	FVector2D prevMousePos;
	UPROPERTY(BlueprintReadOnly)
	bool bIsDrawing;
	
	// Mouse Event
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
public:
	// Init Popup
	void InitPopup(int32 Qid);
	
private:
	// Question Id
	int32 Qid;
	
	// Close Draw Window
	UFUNCTION()
	void CloseDrawWindow();
	
	// Draw Functions
	void DrawLines(FVector2D mousePos, FLinearColor drawColor);
	void DrawPoint(FVector2D mousePos, FLinearColor drawColor);
	FVector2D GetLocalMousePos(FVector2D mousePos);
	
	// Clear Canvas
	UFUNCTION()
	void ClearCanvas();
	// Save Render Target Canvas
	UFUNCTION()
	void SaveCanvas();
	
	bool SaveRenderTargetToPNG(UTextureRenderTarget2D* RenderTarget, const FString& FullFilePath);
};
