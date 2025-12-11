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
	
	// Init Popup
	void InitPopup(int32 InQid, const FString& InAnswerKr);
	
	FVector2D GetPrevMousePos();
	
protected:
	// Objects
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UTextureRenderTarget2D> RT_Canvas;
	
	// Widgets
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<class UOverlay> Overlay_WriteBoard;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<class USizeBox> SizeBox_Border;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<class USizeBox> SizeBox_Canvas;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<class UImageButton> Button_Save;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<class UTextureButton> Button_Close;
	
	// WriteBoard Widgets
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<class UHorizontalBox> HorizontalBox_DotLine;
	UPROPERTY()
	TArray<class UImage*> DotLineImages;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<class UHorizontalBox> HorizontalBox_Guide;
	UPROPERTY()
	TArray<class UTextBlock*> GuideTexts;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<class UTextBlock> Text_Guide;
	FSlateFontInfo tempFontInfo;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<class UImage> Image_Canvas;
	
	// Values
	UPROPERTY(BlueprintReadOnly)
	FVector2D prevMousePos;
	UPROPERTY(BlueprintReadOnly)
	bool bIsDrawing;
	
	// Mouse Event
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
	// Adjust WriteBoard Length
	void AdjustLength();
	
	// Clear WriteBoard
	void ClearWriteBoard();
	
private:
	// WriteBoard
	UPROPERTY()
	TObjectPtr<class UWriteBoard> writeBoardObject;
	
	// Question Id
	int32 Qid;
	FString AnswerKr;
	
	// Now Answer Index
	int32 AnswerIdx = 0;
	// Canvas Step Length
	float stepLength = 360;
	// Border Min Width
	float borderMinWidth = 560;
	
	// DotLine Image
	UPROPERTY()
	TObjectPtr<class UTexture2D> dotLineTexture;
	UPROPERTY()
	TObjectPtr<class UFont> guideTextFont;
	
	// Close Draw Window
	UFUNCTION()
	void CloseDrawWindow();
	
	// Draw Functions
	void DrawLines(FVector2D mousePos, FLinearColor drawColor);
	void DrawPoint(FVector2D mousePos, FLinearColor drawColor);
	FVector2D GetLocalMousePos(FVector2D mousePos);
	
	UFUNCTION()
	void SaveCanvas();
	
	void ClearCanvas();
};
