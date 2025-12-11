// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "WriteBoard.h"

#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Popup_WriteBoard.h"
#include "Components/Image.h"
#include "Engine/Canvas.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetRenderingLibrary.h"


UWriteBoard::UWriteBoard(class UPopup_WriteBoard* parent)
{
	parentWritePopup = parent;
}
FVector2D UWriteBoard::GetLocalMousePos(UImage* Image_Canvas, UTextureRenderTarget2D* RT_Canvas, FVector2D mousePos)
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

bool UWriteBoard::SaveRenderTargetToPNG(UTextureRenderTarget2D* RenderTarget, const FString& FullFilePath)
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

void UWriteBoard::SaveCanvas(int32 Qid, UTextureRenderTarget2D* RT_Canvas)
{
	// File Path
	IFileManager::Get().MakeDirectory(*filePath, true);
	// File Name
	// FString fileName = FDateTime::Now().ToString(TEXT("%Y_%m_%d_%H_%M_%S.png"));
	FString fileName = FString::Printf(TEXT("Answer%d.PNG"), Qid);
	
	// Export Render Target to png
	SaveRenderTargetToPNG(RT_Canvas, filePath / fileName);
}