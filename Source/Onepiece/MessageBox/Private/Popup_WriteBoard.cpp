// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "Popup_WriteBoard.h"

#include "GameLogging.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "UImageButton.h"
#include "UPopupManager.h"
#include "UTextureButton.h"
#include "Components/Image.h"
#include "Engine/Canvas.h"
#include "Kismet/KismetRenderingLibrary.h"

UPopup_WriteBoard::UPopup_WriteBoard(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ConstructorHelpers::FObjectFinder<UTextureRenderTarget2D> rtCanvasRef(TEXT("/Script/Engine.TextureRenderTarget2D'/Game/CustomContents/UI/DrawingBoard/RT_Canvas.RT_Canvas'"));
	if (rtCanvasRef.Succeeded())
	{
		RT_Canvas = rtCanvasRef.Object;
	}
}

void UPopup_WriteBoard::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	// Button Event
	Button_Clear->OnButtonClickedEvent.AddDynamic(this, &UPopup_WriteBoard::ClearCanvas);
	Button_Save->OnButtonClickedEvent.AddDynamic(this, &UPopup_WriteBoard::SaveCanvas);
	Button_Close->OnButtonClickedEvent.AddDynamic(this, &UPopup_WriteBoard::CloseDrawWindow);
}

FReply UPopup_WriteBoard::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// Check Mouse is in Canvas
	const FGeometry CanvasGeometry = Image_Canvas->GetCachedGeometry();
	if (!CanvasGeometry.IsUnderLocation(InMouseEvent.GetScreenSpacePosition()))
	{
		return FReply::Unhandled();
	}
	
	// Get Mouse Position in Local Image Coordinate System
	// Save Current MousePos to prevMousePos
	prevMousePos = GetLocalMousePos(InMouseEvent.GetScreenSpacePosition());
	bIsDrawing = true;
	
	// Draw Point Once
	// PRINT_STRING(TEXT("%s"), *InMouseEvent.GetEffectingButton().GetFName().ToString());
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))			// Draw
	{
		DrawPoint(GetLocalMousePos(InMouseEvent.GetScreenSpacePosition()), FLinearColor::Black);
	}
	else if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))	// Erase
	{
		DrawPoint(GetLocalMousePos(InMouseEvent.GetScreenSpacePosition()), FLinearColor::White);		
	}
	
	return FReply::Handled();
}

FReply UPopup_WriteBoard::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// Get Mouse Position in Local Image Coordinate System
	// Save ZeroVector to prevMousePos
	prevMousePos = GetLocalMousePos(FVector2D::ZeroVector);
	bIsDrawing = false;
	
	return FReply::Handled();
}

FReply UPopup_WriteBoard::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// return When Mouse Not Pressed
	if (!bIsDrawing) return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
	
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))			// Draw
	{
		DrawLines(GetLocalMousePos(InMouseEvent.GetScreenSpacePosition()), FLinearColor::Black);
	}
	else if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))	// Erase
	{
		DrawLines(GetLocalMousePos(InMouseEvent.GetScreenSpacePosition()), FLinearColor::White);
	}
	
	return FReply::Handled();
}

void UPopup_WriteBoard::CloseDrawWindow()
{
	if (const auto PopupMgr = UPopupManager::Get(GetWorld()))
	{
		PopupMgr->HideCurrentPopup(false);
		PRINT_STRING(TEXT("WriteBoard!!!!!"));
	}
}

void UPopup_WriteBoard::DrawPoint(FVector2D mousePos, FLinearColor drawColor)
{
	// Begin Draw Canvas To Render Target
	UCanvas* canvas = nullptr;
	FVector2D size;
	FDrawToRenderTargetContext context;
	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(this, RT_Canvas, canvas, size, context);
	
	// Set thickness Whether now in Draw or Erase
	float thickness = (drawColor == FLinearColor::Black) ? 10 : 30;
	// Draw Box
	canvas->K2_DrawBox(mousePos, FVector2D(1, 1), thickness, drawColor);
	
	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(this, context);
}

void UPopup_WriteBoard::DrawLines(FVector2D mousePos, FLinearColor drawColor)
{
	// Begin Draw Canvas To Render Target
	UCanvas* canvas = nullptr;
	FVector2D size;
	FDrawToRenderTargetContext context;
	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(this, RT_Canvas, canvas, size, context);
	
	// Calculate Draw Positions
	FVector2D currPos = prevMousePos;
	int32 div = 64;
	FVector2D drawOffset = (mousePos - currPos) / div;
	
	// Draw
	for (int32 i = 1; i < div; ++i)
	{
		currPos = prevMousePos + drawOffset * i;
		
		// Set thickness Whether now in Draw or Erase
		float thickness = (drawColor == FLinearColor::Black) ? 10 : 30;
		// Draw Line
		canvas->K2_DrawLine(prevMousePos, currPos, thickness, drawColor);
	}
	prevMousePos = mousePos;
	
	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(this, context);
}

FVector2D UPopup_WriteBoard::GetLocalMousePos(FVector2D mousePos)
{
	// Get Absolute Local Pos
	const FGeometry& geometry = Image_Canvas->GetCachedGeometry();
	FVector2D localPos = geometry.AbsoluteToLocal(mousePos);
	
	// Get Canvas Size
	const FVector2D canvasSize = geometry.GetLocalSize();
	// Transform localPos(in Image_Canvas Coord) to RT_Canvas Coord && Clamp upto RT_Canvas' Border
	localPos.X = FMath::Clamp((localPos.X / canvasSize.X * RT_Canvas->SizeX), 0.f, RT_Canvas->SizeX);
	localPos.Y = FMath::Clamp((localPos.Y / canvasSize.Y * RT_Canvas->SizeY), 0.f, RT_Canvas->SizeY);
	return localPos;
}

void UPopup_WriteBoard::ClearCanvas()
{
	UKismetRenderingLibrary::ClearRenderTarget2D(this, RT_Canvas, RT_Canvas->ClearColor);
}

void UPopup_WriteBoard::SaveCanvas()
{
	// File Path
	const FString filePath = FPaths::ProjectSavedDir() / TEXT("WriteImage/");
	IFileManager::Get().MakeDirectory(*filePath, true);
	// File Name
	FString fileName = FDateTime::Now().ToString(TEXT("%Y_%m_%d_%H_%M_%S.png"));
	
	// Export Render Target to png
	UKismetRenderingLibrary::ExportRenderTarget(this, RT_Canvas, filePath, fileName);
	// UE_LOG(LogTemp, Warning, TEXT("%s | %s"), *filePath, *fileName);
	
	SaveRenderTargetToPNG(RT_Canvas, filePath / fileName);
}

bool UPopup_WriteBoard::SaveRenderTargetToPNG(UTextureRenderTarget2D* RenderTarget, const FString& FullFilePath)
{
	FTextureRenderTargetResource* RTResource = RenderTarget->GameThread_GetRenderTargetResource();
	if (!RTResource)
	{
		return false;
	}

	// Get Width & Height
	const int32 Width  = RenderTarget->SizeX;
	const int32 Height = RenderTarget->SizeY;

	// Set Bitmap array
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

	// Compress to PNG
	const TArray64<uint8>& PNGData = ImageWrapper->GetCompressed(100);

	// Make Directory
	const FString Directory = FPaths::GetPath(FullFilePath);
	IFileManager::Get().MakeDirectory(*Directory, true);

	return FFileHelper::SaveArrayToFile(PNGData, *FullFilePath);
}