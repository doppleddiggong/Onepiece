// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "Popup_QuestionnaireResultItem.h"

#include "GameLogging.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Components/TextBlock.h"

UPopup_QuestionnaireResultItem::UPopup_QuestionnaireResultItem(const FObjectInitializer& ObjectInitializer)
{
	ConstructorHelpers::FObjectFinder<UMaterialInterface> materialResultRef(TEXT("/Script/Engine.Material'/Game/CustomContents/UI/DrawingBoard/M_Result.M_Result'"));
	if (materialResultRef.Succeeded())
	{
		M_Result = UMaterialInstanceDynamic::Create(materialResultRef.Object, this);
	}
}

void UPopup_QuestionnaireResultItem::InitItem(int32 index, const FResponseWriteData& data)
{
	// 질문 인덱스 설정 (예: "Question.01")
	if (Txt_Index)
	{
		FString IndexText = FString::Printf(TEXT("Question.%02d"), index);
		Txt_Index->SetText(FText::FromString(IndexText));
	}
	
	// 이미지 불러와서 적용
	FString FileName = FString::Printf(TEXT("Answer%d.PNG"), index);
	UTexture2D* LoadedTexture = LoadTextureFromFile(WriteImagePath / FileName);
	if (LoadedTexture)
	{
		// Image_Purpose->SetBrushFromTexture(LoadedTexture);
		if (M_Result)
			M_Result->SetTextureParameterValue(FName("Texture"), LoadedTexture);
	}
	
	// 피드백 내용 설정
	if (Text_Display)
	{
		FString QuestionText = data.display.message;
		Text_Display->SetText(FText::FromString(QuestionText));
	}
	if (Text_Record)
	{
		FString QuestionText = data.display.correction;
		Text_Record->SetText(FText::FromString(QuestionText));
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
