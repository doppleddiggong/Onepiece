// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "WriteBoard.h"

#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetRenderingLibrary.h"


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

void UWriteBoard::ClearCanvas(UTextureRenderTarget2D* RT_Canvas)
{
	UKismetRenderingLibrary::ClearRenderTarget2D(this, RT_Canvas, RT_Canvas->ClearColor);
}