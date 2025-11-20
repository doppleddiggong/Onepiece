// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DrawingBoardWidget.generated.h"

/**
 * 
 */
UCLASS()
class ONEPIECE_API UDrawingBoardWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UDrawingBoardWidget(const FObjectInitializer& ObjectInitializer);
	virtual void NativeConstruct() override;
	
protected:
	// Objects
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UTextureRenderTarget2D> RT_Canvas;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UMaterialInterface> M_Brush;
	
	// Widgets
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<class UImage> Image_Canvas;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<class UButton> Button_Clear;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<class UButton> Button_Save;
	
	// Values
	UPROPERTY(BlueprintReadOnly)
	FVector2D prevMousePos;
	UPROPERTY(BlueprintReadOnly)
	bool bIsDrawing;
	
	// Mouse Event
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
private:
	// Draw
	void DrawPoint(FVector2D mousePos);
	FVector2D GetLocalMousePos(FVector2D mousePos);
	
	// Clear Canvas
	UFUNCTION()
	void ClearCanvas();
	// Save Render Target Canvas
	UFUNCTION()
	void SaveCanvas();
};
