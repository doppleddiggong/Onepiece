// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "Popup_WriteBoard.h"

#include "GameLogging.h"
#include "NetworkData.h"
#include "NetworkMessage.h"
#include "UImageButton.h"
#include "UPopupManager.h"
#include "UTextureButton.h"
#include "Components/Image.h"
#include "Engine/Canvas.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "WriteBoard.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"

UPopup_WriteBoard::UPopup_WriteBoard(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ConstructorHelpers::FObjectFinder<UTextureRenderTarget2D> rtCanvasRef(TEXT("/Script/Engine.TextureRenderTarget2D'/Game/CustomContents/UI/DrawingBoard/RT_Canvas.RT_Canvas'"));
	if (rtCanvasRef.Succeeded())
	{
		RT_Canvas = rtCanvasRef.Object;
	}
	
	ConstructorHelpers::FObjectFinder<UTexture2D> dotLineImageRef(TEXT("/Script/Engine.Texture2D'/Game/CustomContents/UI/DrawingBoard/writepanel.writepanel'"));
	if (dotLineImageRef.Succeeded())
	{
		dotLineTexture = dotLineImageRef.Object;
	}
	
	ConstructorHelpers::FObjectFinder<UFont> guideTextFontRef(TEXT("/Script/Engine.Font'/Engine/EngineFonts/Roboto.Roboto'"));
	if (guideTextFontRef.Succeeded())
	{
		guideTextFont = guideTextFontRef.Object;
	}
}

void UPopup_WriteBoard::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UPopup_WriteBoard::NativeConstruct()
{
	Super::NativeConstruct();
	writeBoardObject = NewObject<UWriteBoard>();
}

void UPopup_WriteBoard::InitPopup(int32 InQid, const FWriteQuestionData& InQuestionData)
{
	// Button Event
	Button_Save->OnButtonClickedEvent.RemoveDynamic(this, &UPopup_WriteBoard::SaveCanvas);
	Button_Save->OnButtonClickedEvent.AddDynamic(this, &UPopup_WriteBoard::SaveCanvas);

	if (Text_Guide != nullptr)
	{
		tempFontInfo = Text_Guide->GetFont();
		Text_Guide->RemoveFromParent();
	}
	
	this->Qid = InQid;
	this->AnswerKr = InQuestionData.answer_kor;
	this->Text_Question->SetText(FText::FromString(InQuestionData.word_data.eng));
	
	ClearWriteBoard();
	AdjustLength();
}

FVector2D UPopup_WriteBoard::GetPrevMousePos()
{
	return prevMousePos;
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

void UPopup_WriteBoard::AdjustLength()
{
	// 예상 답변에 따른 WriteBoard 길이 조절
	float letterNum = AnswerKr.Len();
	
	// 1. Render Target 길이 늘리기
	RT_Canvas->ResizeTarget(stepLength * letterNum, stepLength);
	
	// 2. Image의 길이 늘리기
	Image_Canvas->SetDesiredSizeOverride(FVector2D(stepLength * letterNum, stepLength));
	
	// 예상 답변 힌트 생성
	for (const TCHAR letter : AnswerKr)
	{
		// 십자 점선 칸 이미지 추가
		UImage* tempImage = NewObject<UImage>(this, UImage::StaticClass());
		tempImage->SetBrushFromTexture(dotLineTexture);
		
		// 이미지 크기 설정
		FSlateBrush tempImageBrush = tempImage->GetBrush();
		tempImageBrush.SetImageSize(FVector2D(360));
		tempImage->SetBrush(tempImageBrush);
		
		HorizontalBox_DotLine->AddChildToHorizontalBox(tempImage);
		DotLineImages.Add(tempImage);
		
		// 글자 힌트 텍스트 추가
		UTextBlock* tempText = NewObject<UTextBlock>(this, UTextBlock::StaticClass());
		tempText->SetText(FText::FromString(FString::Printf(TEXT("%c"), letter)));
		tempText->SetColorAndOpacity(FLinearColor(0.4f, 0.4f, 0.4f, 0.5f));
		tempText->SetMinDesiredWidth(360.f);
		tempText->SetJustification(ETextJustify::Type::Center);
		
		// 글자 폰트 크기 설정
		tempText->SetFont(tempFontInfo);
		
		HorizontalBox_Guide->AddChildToHorizontalBox(tempText);
		GuideTexts.Add(tempText);
	}
}

void UPopup_WriteBoard::ClearWriteBoard()
{
	HorizontalBox_DotLine->ClearChildren();
	HorizontalBox_Guide->ClearChildren();
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
		float thickness = (drawColor == FLinearColor::Black) ? 10 : 25;
		// Draw Line
		canvas->K2_DrawLine(prevMousePos, currPos, thickness, drawColor);
	}
	prevMousePos = mousePos;
	
	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(this, context);
}

FVector2D UPopup_WriteBoard::GetLocalMousePos(FVector2D mousePos)
{
	return writeBoardObject->GetLocalMousePos(Image_Canvas, RT_Canvas, mousePos);
}

void UPopup_WriteBoard::SaveCanvas()
{
	if (const auto PopupMgr = UPopupManager::Get(GetWorld()))
	{
		if (writeBoardObject != nullptr)
			writeBoardObject->SaveCanvas(Qid, RT_Canvas);

		// 캔버스 저장 완료 델리게이트 브로드캐스트
		OnCanvasSaved.Broadcast();

		ClearCanvas();
		PopupMgr->HideCurrentPopup(false);
	}
}

void UPopup_WriteBoard::ClearCanvas()
{
	UKismetRenderingLibrary::ClearRenderTarget2D(this, RT_Canvas, RT_Canvas->ClearColor);
}
