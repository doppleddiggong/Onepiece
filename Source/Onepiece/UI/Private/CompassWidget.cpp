// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "CompassWidget.h"

#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"

void UCompassWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UCompassWidget::RotateCompass(float ZRotation)
{
	FVector2D DesiredSize = Img_Compass->GetDesiredSize();
	
	UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Img_Compass->Slot);
	if (CanvasSlot)
	{
		float PositionX = -ZRotation * (DesiredSize.X / 360.f);
		CanvasSlot->SetPosition(FVector2D(PositionX, 0.f));
	}
}


