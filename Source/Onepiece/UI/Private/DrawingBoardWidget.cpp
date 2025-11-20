// Fill out your copyright notice in the Description page of Project Settings.


#include "DrawingBoardWidget.h"

#include "Components/Button.h"
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

void UDrawingBoardWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	Button_Clear->OnClicked.AddDynamic(this, &UDrawingBoardWidget::ClearCanvas);
	Button_Save->OnClicked.AddDynamic(this, &UDrawingBoardWidget::SaveCanvas);
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
	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(this, RT_Canvas, canvas, size, context);
	
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
	
	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(this, context);
}

FVector2D UDrawingBoardWidget::GetLocalMousePos(FVector2D mousePos)
{
	// Get Absolute Local Pos
	const FGeometry& geometry = Image_Canvas->GetCachedGeometry();
	FVector2D localPos = geometry.AbsoluteToLocal(mousePos);
	return localPos;
}

void UDrawingBoardWidget::ClearCanvas()
{
	UKismetRenderingLibrary::ClearRenderTarget2D(this, RT_Canvas, RT_Canvas->ClearColor);
}

void UDrawingBoardWidget::SaveCanvas()
{
	// 저장할 경로 (예: 프로젝트/Saved/화면캡쳐.png)
	const FString FilePath = FPaths::ProjectSavedDir() / TEXT("WriteImage/");
	FString FileName = FDateTime::Now().ToString(TEXT("%Y-%m-%d-%H-%M-%S"));
	FileName += FString(TEXT(".png"));
	
	IFileManager::Get().MakeDirectory(*FilePath, true);
	UKismetRenderingLibrary::ExportRenderTarget(this, RT_Canvas, FilePath, FileName);
}
