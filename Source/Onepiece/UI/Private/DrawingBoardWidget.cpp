// Fill out your copyright notice in the Description page of Project Settings.


#include "DrawingBoardWidget.h"

#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/SceneCaptureComponent2D.h"
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
	// File Path
	const FString FilePath = FPaths::ProjectSavedDir() / TEXT("WriteImage/");
	IFileManager::Get().MakeDirectory(*FilePath, true);
	// File Name
	FString FileName = FDateTime::Now().ToString(TEXT("%Y_%m_%d_%H_%M_%S.png"));
	
	// Export Render Target to png
	UKismetRenderingLibrary::ExportRenderTarget(this, RT_Canvas, FilePath, FileName);
	UE_LOG(LogTemp, Warning, TEXT("%s | %s"), *FilePath, *FileName);
	
	SaveRenderTargetToPNG(RT_Canvas, FilePath / FileName);
}

bool UDrawingBoardWidget::SaveRenderTargetToPNG(UTextureRenderTarget2D* RenderTarget, const FString& FullFilePath)
{
	FTextureRenderTargetResource* RTResource = RenderTarget->GameThread_GetRenderTargetResource();
	if (!RTResource)
	{
		return false;
	}

	const int32 Width  = RenderTarget->SizeX;
	const int32 Height = RenderTarget->SizeY;

	TArray<FColor> Bitmap;
	Bitmap.AddUninitialized(Width * Height);

	// Read BGRA8 pixels in RenderTarget
	RTResource->ReadPixels(Bitmap);

	// PNG Encoder
	IImageWrapperModule& ImageWrapperModule =
		FModuleManager::LoadModuleChecked<IImageWrapperModule>("ImageWrapper");

	TSharedPtr<IImageWrapper> ImageWrapper =
		ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);

	if (!ImageWrapper.IsValid())
	{
		return false;
	}

	ImageWrapper->SetRaw(
		Bitmap.GetData(),
		Bitmap.GetAllocatedSize(),
		Width,
		Height,
		ERGBFormat::BGRA,
		8
	);

	const TArray64<uint8>& PNGData = ImageWrapper->GetCompressed(100);

	// Make Directory
	const FString Directory = FPaths::GetPath(FullFilePath);
	IFileManager::Get().MakeDirectory(*Directory, true);

	return FFileHelper::SaveArrayToFile(PNGData, *FullFilePath);
}