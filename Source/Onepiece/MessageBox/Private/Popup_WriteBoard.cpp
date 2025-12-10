// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "Popup_WriteBoard.h"

#include "GameLogging.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "UImageButton.h"
#include "UPopupManager.h"
#include "UTextureButton.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Engine/Canvas.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "WriteBoard.h"

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
	
	writeBoardObject = NewObject<UWriteBoard>();
	
	// Button Event
	Button_Clear->OnButtonClickedEvent.AddDynamic(this, &UPopup_WriteBoard::ClearCanvas);
	Button_Save->OnButtonClickedEvent.AddDynamic(this, &UPopup_WriteBoard::SaveCanvas);
	Button_Close->OnButtonClickedEvent.AddDynamic(this, &UPopup_WriteBoard::CloseDrawWindow);
	Button_Right->OnClicked.AddDynamic(this, &UPopup_WriteBoard::OnButtonRightClicked);
	Button_Left->OnClicked.AddDynamic(this, &UPopup_WriteBoard::OnButtonLeftClicked);
	
	Button_Left->SetVisibility(ESlateVisibility::Hidden);
	Button_Left->SetVisibility(ESlateVisibility::Hidden);
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

void UPopup_WriteBoard::OnButtonRightClicked()
{
	SaveCanvas();
	if (AnswerIdx < AnswerKr.Num() - 1)
	{
		AnswerIdx++;
	}
	RefreshArrowButton();
	AdjustLength();
}

void UPopup_WriteBoard::OnButtonLeftClicked()
{
	SaveCanvas();
	if (0 < AnswerIdx)
	{
		AnswerIdx--;
	}
	RefreshArrowButton();
	AdjustLength();
}

void UPopup_WriteBoard::RefreshArrowButton()
{
	Button_Right->SetVisibility(AnswerIdx < AnswerKr.Num() - 1 ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	Button_Left->SetVisibility(0 < AnswerIdx ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void UPopup_WriteBoard::AdjustLength()
{
	// TODO: 그냥 렌더 타겟 2d 새로 만들고 배열로 유지
	// TODO: 예상 답변에 따른 WriteBoard 길이 조절
	float letterNum = AnswerKr[AnswerIdx].Len();
	
	// 1. Render Target 길이 늘리기
	RT_Canvas->ResizeTarget(stepLength * letterNum, stepLength);
	
	// 2. Image의 길이 늘리기
	Image_Canvas->SetDesiredSizeOverride(FVector2D(stepLength * letterNum, stepLength));
	SizeBox_Border->SetWidthOverride(borderMinWidth + stepLength * letterNum);
	
	SizeBox_Canvas->SetWidthOverride(stepLength * letterNum);
	
	// TODO: 예상 답변 힌트 생성
}

void UPopup_WriteBoard::InitPopup(int32 InQid, const TArray<FString>& InTokens)
{
	this->Qid = InQid;
	this->AnswerKr = InTokens;
	
	if (AnswerKr.Num() > 1)
		Button_Right->SetVisibility(ESlateVisibility::Visible);
	
	AdjustLength();
}

void UPopup_WriteBoard::CloseDrawWindow()
{
	if (const auto PopupMgr = UPopupManager::Get(GetWorld()))
	{
		SaveCanvas();
		
		ClearCanvas();
		PopupMgr->HideCurrentPopup(false);
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
		float thickness = (drawColor == FLinearColor::Black) ? 12 : 30;
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

void UPopup_WriteBoard::SaveCanvas()
{
	writeBoardObject->SaveCanvas(Qid, RT_Canvas);
}

void UPopup_WriteBoard::ClearCanvas()
{
	writeBoardObject->ClearCanvas(RT_Canvas);
}
