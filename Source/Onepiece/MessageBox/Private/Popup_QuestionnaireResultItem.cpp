// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "Popup_QuestionnaireResultItem.h"

#include "GameLogging.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"

void UPopup_QuestionnaireResultItem::InitItem(int32 index, FString questionKor, const FResponseWriteData& data)
{
	// 질문 인덱스 설정 (예: "Question.01")
	if (Text_Index)
	{
		FString IndexText = FString::Printf(TEXT("Question.%02d"), index);
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
		float aspectRatio = LoadedTexture->GetSizeX() / LoadedTexture->GetSizeY();
		SizeBox_Answer->SetMinAspectRatio(aspectRatio);
		SizeBox_Answer->SetMaxAspectRatio(aspectRatio);
		// Image X 사이즈가 1080보다 크면 1080에, 아니면 Image X 사이즈에 1/ratio를 곱한다.
		SizeBox_Answer->SetHeightOverride(Image_Answer->GetBrush().GetImageSize().X > SizeBox_Answer->GetMaxDesiredWidth() ? SizeBox_Answer->GetMaxDesiredWidth() / aspectRatio : Image_Answer->GetBrush().GetImageSize().X / aspectRatio);
		Image_Answer->SetBrushFromTexture(LoadedTexture);
	}
	
	// 피드백 내용 설정
	if (Text_Message)
	{
		FString text = data.display.message;
		Text_Message->SetText(FText::FromString(text));
	}
	if (Text_Correction)
	{
		FString text = data.display.correction;
		Text_Correction->SetText(FText::FromString(text));
	}
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
