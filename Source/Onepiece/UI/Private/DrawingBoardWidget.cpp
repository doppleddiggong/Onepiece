// Fill out your copyright notice in the Description page of Project Settings.


#include "DrawingBoardWidget.h"

#include "Components/Image.h"
#include "Engine/Canvas.h"
#include "Kismet/KismetRenderingLibrary.h"

UDrawingBoardWidget::UDrawingBoardWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ConstructorHelpers::FObjectFinder<UTextureRenderTarget2D> rtCanvasRef(TEXT("/Script/Engine.TextureRenderTarget2D'/Game/ThirdPerson/Blueprints/Canvas/RT_Canvas.RT_Canvas'"));
	if (rtCanvasRef.Succeeded())
	{
		RT_Canvas = rtCanvasRef.Object;
	}
	ConstructorHelpers::FObjectFinder<UMaterialInterface> mBrushRef(TEXT("/Script/Engine.Material'/Game/ThirdPerson/Blueprints/Canvas/M_Brush.M_Brush'"));
	if (mBrushRef.Succeeded())
	{
		M_Brush = mBrushRef.Object;
	}
}

FReply UDrawingBoardWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	prevMousePos = GetLocalMousePos(InMouseEvent.GetScreenSpacePosition());
	bIsDrawing = true;
	
	return FReply::Handled();
}

FReply UDrawingBoardWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	prevMousePos = GetLocalMousePos(FVector2D::ZeroVector);
	bIsDrawing = false;
	
	return FReply::Handled();
}

FReply UDrawingBoardWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (bIsDrawing)
	{
		DrawPoint(GetLocalMousePos(InMouseEvent.GetScreenSpacePosition()));
	}
	
	return FReply::Handled();
}

void UDrawingBoardWidget::DrawPoint(FVector2D mousePos)
{
	// Begin Draw Canvas To Render Target
	UCanvas* canvas = nullptr;
	FVector2D size;
	FDrawToRenderTargetContext context;
	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(GetWorld(), RT_Canvas, canvas, size, context);
	
	// Calculate Draw Positions
	// 10 : 적당, 20 : 너무 많음(렉)
	int32 div = 15;
	FVector2D currPos = prevMousePos;
	FVector2D drawOffset = (mousePos - currPos) / div;
	
	// Draw
	for (int32 i = 1; i < div; ++i)
	{
		currPos = prevMousePos + drawOffset * i;
		
		// Draw Material
		canvas->K2_DrawMaterial(M_Brush, currPos, FVector2D(20, 20), FVector2D::ZeroVector);
	}
	prevMousePos = mousePos;
	
	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(GetWorld(), context);
}

FVector2D UDrawingBoardWidget::GetLocalMousePos(FVector2D mousePos)
{
	// Get Absolute Local Pos
	const FGeometry& geometry = Image_Canvas->GetCachedGeometry();
	FVector2D localPos = geometry.AbsoluteToLocal(mousePos);
	return localPos;
}
