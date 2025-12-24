// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "Popup_QuestionnaireResultItem.h"

#include "GameLogging.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Onepiece/Onepiece.h"

void UPopup_QuestionnaireResultItem::InitItem(int32 index, FString questionKor, const FResponseWriteData& data)
{
	// 질문 인덱스 설정 (예: "Question.01")
	if (Text_Index)
	{
		FString IndexText = FString::Printf(TEXT("Q%d"), index);
		Text_Index->SetText(FText::FromString(IndexText));
	}
	
	if (Text_Question)
	{
		PRINTLOG(TEXT("Question: %s"), *questionKor);
		Text_Question->SetText(FText::FromString(questionKor));
	}
	
	// 이미지 불러와서 적용
	FString FileName = FString::Printf(TEXT("Answer%d.PNG"), index);
	UTexture2D* LoadedTexture = LoadTextureFromFile(WriteImagePath / FileName);
	
	if (LoadedTexture)
	{
		// SizeBox 비율 이미지에 맞게 수정
		const float MaxWidthLimit = 1080.0f;
		const float TargetHeight = 360.0f;

		const float TextureWidth = static_cast<float>(LoadedTexture->GetSizeX());
		const float TextureHeight = static_cast<float>(LoadedTexture->GetSizeY());
		const float AspectRatio = TextureWidth / TextureHeight;

		// 1. 이미지 위젯의 브러시 정보를 가져와서 텍스처와 사이즈를 일치시킴
		// 이 작업이 빠지면 이미지는 이전 브러시 사이즈(360x360)에 텍스처를 맞춥니다.
		FSlateBrush NewBrush = Image_Answer->GetBrush();
		NewBrush.SetResourceObject(LoadedTexture);
		NewBrush.ImageSize = FVector2D(TextureWidth, TextureHeight); // 브러시 자체 크기를 원본으로 설정
		Image_Answer->SetBrush(NewBrush);
		
		float NewWidth = TargetHeight * AspectRatio;
		float NewHeight = TargetHeight;
		
		// 가로 제한을 넘을 경우 비율 유지하며 축소
		if (NewWidth > MaxWidthLimit)
		{
			NewWidth = MaxWidthLimit;
			NewHeight = NewWidth / AspectRatio;
		}

		// 3. SizeBox의 가로/세로를 모두 명시적으로 고정
		SizeBox_Answer->SetWidthOverride(NewWidth);
		SizeBox_Answer->SetHeightOverride(NewHeight);

		Image_Answer->SetBrushFromTexture(LoadedTexture);
	}
	
	// 피드백 내용 설정
	FString Description = FString::Printf(TEXT("%s\n\n[Correction]\n%s"),
		*data.display.message,
		*data.display.correction);

	Text_Feedback->SetText(FText::FromString(Description));
	Text_Feedback->SetLineHeightPercentage( DefineData::LineHeightPercentage );
}

UTexture2D* UPopup_QuestionnaireResultItem::LoadTextureFromFile(const FString& filePath)
{
	TArray<uint8> RawFileData;
	if (!FFileHelper::LoadFileToArray(RawFileData, *filePath))
	{
		PRINTLOG(TEXT("Failed to load file: %s"), *filePath);
		return nullptr;
	}

	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);

	if (!ImageWrapper.IsValid() || !ImageWrapper->SetCompressed(RawFileData.GetData(), RawFileData.Num()))
	{
		PRINTLOG(TEXT("Failed to parse PNG: %s"), *filePath);
		return nullptr;
	}

	TArray<uint8> UncompressedBGRA;
	if (!ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, UncompressedBGRA))
	{
		PRINTLOG(TEXT("Failed to decompress PNG: %s"), *filePath);
		return nullptr;
	}

	UTexture2D* Texture = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_B8G8R8A8);
	if (!Texture)
	{
		return nullptr;
	}

	void* TextureData = Texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(TextureData, UncompressedBGRA.GetData(), UncompressedBGRA.Num());
	Texture->GetPlatformData()->Mips[0].BulkData.Unlock();
	Texture->UpdateResource();

	return Texture;
}
