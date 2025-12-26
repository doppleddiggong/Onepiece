// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "UPageScrollDot.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Blueprint/WidgetTree.h"

void UPageScrollDot::SetNumberOfPages(int32 PageCount)
{
	if (!DotContainer || !DotWidgetClass)
		return;

	// 기존 도트 제거
	dotList.Empty();
	DotContainer->ClearChildren();

	// 새 도트 생성
	for (int32 i = 0; i < PageCount; ++i)
	{
		UUserWidget* newDot = CreateWidget<UUserWidget>(GetWorld(), DotWidgetClass);
		if (!newDot)
			continue;

		// DotContainer에 추가
		DotContainer->AddChild(newDot);

		// 초기 색상/스케일 설정 (비선택 상태)
		UpdateDotAppearance(newDot, UnselectedColor, UnselectedScale);

		dotList.Add(newDot);
	}

	// 첫 번째 도트를 선택 상태로 설정
	SetCurrentPage(0);
}

void UPageScrollDot::SetCurrentPage(int32 PageIndex)
{
	if (!dotList.IsValidIndex(PageIndex))
		return;

	currentPageIndex = PageIndex;

	// 모든 도트 업데이트
	for (int32 i = 0; i < dotList.Num(); ++i)
	{
		UUserWidget* dot = dotList[i];
		if (!dot)
			continue;

		if (i == currentPageIndex)
		{
			// 선택된 도트
			UpdateDotAppearance(dot, SelectedColor, SelectedScale);
		}
		else
		{
			// 비선택된 도트
			UpdateDotAppearance(dot, UnselectedColor, UnselectedScale);
		}
	}
}

UImage* UPageScrollDot::GetDotImage(UUserWidget* DotWidget) const
{
	if (!DotWidget)
		return nullptr;

	// "Image_Dot" 이름으로 Image 컴포넌트 찾기
	UImage* dotImage = Cast<UImage>(DotWidget->GetWidgetFromName(TEXT("Image_Dot")));

	// 못 찾았다면 WidgetTree에서 첫 번째 Image 컴포넌트 사용
	if (!dotImage && DotWidget->WidgetTree)
	{
		TArray<UWidget*> allWidgets;
		DotWidget->WidgetTree->GetAllWidgets(allWidgets);

		for (UWidget* widget : allWidgets)
		{
			if (UImage* image = Cast<UImage>(widget))
			{
				dotImage = image;
				break;
			}
		}
	}

	return dotImage;
}

void UPageScrollDot::UpdateDotAppearance(UUserWidget* DotWidget, FLinearColor Color, float Scale)
{
	if (!DotWidget)
		return;

	// Image 컴포넌트 찾기
	UImage* dotImage = GetDotImage(DotWidget);
	if (dotImage)
	{
		// 색상 설정
		dotImage->SetColorAndOpacity(Color);
	}

	// 스케일 설정
	DotWidget->SetRenderScale(FVector2D(Scale, Scale));
}