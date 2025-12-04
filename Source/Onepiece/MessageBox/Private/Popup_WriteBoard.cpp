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
	Button_Right->OnClicked.AddDynamic(this, &UPopup_WriteBoard::OnButtonRightClicked);
	Button_Left->OnClicked.AddDynamic(this, &UPopup_WriteBoard::OnButtonLeftClicked);
	
	Button_Left->SetVisibility(ESlateVisibility::Hidden);
	Button_Left->SetVisibility(ESlateVisibility::Hidden);
}

/*
Q. popup_writeboard 클래스를 봐봐. 버튼에 마우스를 올리면 버튼이 깜빡이는데, 왜 그래? 클릭도 안 되는 것 같아.
A.
- Source/Onepiece/MessageBox/Private/Popup_WriteBoard.cpp:42-67를 보면, 커서가 Image_Canvas의 지오메트리 안에 들어오면 어떤 위젯이 위에 겹쳐 있건 무조건 bIsDrawing = true로 바꾸고 FReply::Handled()를 돌려 버립니다. Clear/Save/
Close 같은 버튼들이 같은 보드 영역 위에 배치되어 있어서 커서를 버튼 위로 옮겨도 여전히 Image_Canvas 영역 안으로 판정되고, 부모가 이벤트를 먼저 소화해 버리기 때문에 UImageButton 내부의 UButton까지 마우스 다운 이벤트가 전달되지
않습니다. 그래서 클릭이 안 먹는 것처럼 보입니다.
- 한 번 그 상태가 되면 NativeOnMouseMove(동일 파일 80-94행)도 bIsDrawing을 근거로 계속 Handled()를 반환합니다. Slate 입장에선 마우스 이동 이벤트가 캔버스에서만 소비되었다가 간헐적으로 버튼으로 넘어오기를 반복하므로 버튼이
Hover/Unhover 상태를 매 프레임 번갈아 타게 되고, 이것이 화면에서 “깜빡이는” 현상으로 보입니다. NativeOnMouseButtonUp(70-77행)이 호출되기 전까지 계속 이런 상태라 클릭 시도도 실패하게 됩니다.
- 버튼을 정상적으로 쓰려면 실제로 캔버스를 클릭했는지/버튼을 클릭했는지를 구분할 수 있는 히트 테스트 로직이 필요합니다. 예를 들어 Image_Canvas->IsHovered() 같은 위젯 레벨 체크를 추가하거나, 버튼이 올라가 있는 패널을
SelfHitTestInvisible이 아닌 HitTestInvisible로 바꿔서 캔버스가 버튼 위쪽 픽셀을 잡지 못하게 해야 합니다. 현재처럼 단순히 “좌표가 캔버스 영역 안인가?”만 보고 바로 Handled()를 반환하는 구조에서는 겹치는 모든 버튼에서 같은 문제가
반복될 수밖에 없습니다
*/

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
	if (AnswerIdx < AnswerKr.Num() - 1)
	{
		AnswerIdx++;
		Button_Left->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		Button_Right->SetVisibility(ESlateVisibility::Hidden);
	}
	AdjustLength();
}

void UPopup_WriteBoard::OnButtonLeftClicked()
{
	if (0 < AnswerIdx)
	{
		AnswerIdx--;
		Button_Right->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		Button_Left->SetVisibility(ESlateVisibility::Hidden);
	}
	AdjustLength();
}

void UPopup_WriteBoard::AdjustLength()
{
	// TODO: 예상 답변에 따른 WriteBoard 길이 조절
	// 하다 말았음
	float letterNum = AnswerKr[AnswerIdx].Len();
	
	// 1. Render Target 길이 늘리기
	RT_Canvas->ResizeTarget(stepLength * letterNum, stepLength);
	
	// 2. Image의 길이 늘리기
	Image_Canvas->SetDesiredSizeOverride(FVector2D(stepLength * letterNum, stepLength));
	SizeBox_Border->SetWidthOverride(borderMinWidth + stepLength * letterNum);
	PRINTLOG(TEXT("Border width %f"), SizeBox_Border->GetWidthOverride());
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
	
	// TODO: 이제 화살표 만들어서 단어 여러 개 작성할 수 있게 하기
}

void UPopup_WriteBoard::CloseDrawWindow()
{
	if (const auto PopupMgr = UPopupManager::Get(GetWorld()))
	{
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
	// FString fileName = FDateTime::Now().ToString(TEXT("%Y_%m_%d_%H_%M_%S.png"));
	FString fileName = FString::Printf(TEXT("Answer%d.PNG"), Qid);
	
	// Export Render Target to png
	UKismetRenderingLibrary::ExportRenderTarget(this, RT_Canvas, filePath, fileName);
	// UE_LOG(LogTemp, Warning, TEXT("%s | %s"), *filePath, *fileName);
	
	SaveRenderTargetToPNG(RT_Canvas, filePath / fileName);
	
	CloseDrawWindow();
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