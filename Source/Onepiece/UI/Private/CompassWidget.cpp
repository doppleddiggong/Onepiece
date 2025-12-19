// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "CompassWidget.h"

#include "Components/CanvasPanel.h"
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

UImage* UCompassWidget::AddCompassMarker()
{
	UImage* NewMarker = NewObject<UImage>(this);
	if (NewMarker)
	{
		UTexture2D* CompassTex = LoadObject<UTexture2D>(nullptr,
			TEXT("/Game/StarterBundle/Image/mark1.mark1"));
		if (CompassTex)
		{
			NewMarker->SetBrushFromTexture(CompassTex);
		}
		
		Pnl_Compass->AddChild(NewMarker);

		UCanvasPanelSlot* MarkerSlot =
			Cast<UCanvasPanelSlot>(NewMarker->Slot);

		if (MarkerSlot)
		{
			MarkerSlot->SetAnchors(FAnchors(0.5, 0.5, 0.5, 0.5));
			MarkerSlot->SetSize(FVector2D(60.f, 60.f));
			MarkerSlot->SetAlignment(FVector2D(0.5f, 0.5f));
			MarkerSlot->SetPosition(FVector2D(0.f, 0.f));
		}

		Markers.Add(NewMarker);

		return NewMarker;
	}

	return nullptr;
}

void UCompassWidget::SetMarkerPosition(UImage* InMarker, float TargetRotation, bool bSideLock)
{
	float CompassSize = Img_Compass->GetDesiredSize().X;
	float MarkerRotation = TargetRotation * (CompassSize/360.f);

	UCanvasPanelSlot* MarkerSlot = Cast<UCanvasPanelSlot>(InMarker->Slot);
	if (MarkerSlot)
	{
		if (!bSideLock)
		{
			MarkerSlot->SetPosition(FVector2D(MarkerRotation, 0.f));
		}
		else
		{
			UCanvasPanelSlot* CompassSlot = Cast<UCanvasPanelSlot>(Pnl_Compass->Slot);
			if (CompassSlot)
			{
				float PositionX = (CompassSlot->GetSize().X / 2);
				
				if (FMath::Abs(MarkerRotation) < PositionX)
				{
					MarkerSlot->SetPosition(FVector2D(MarkerRotation, 0.f));
				}
				else if (MarkerRotation > PositionX)
				{
					MarkerSlot->SetPosition(FVector2D(PositionX, 0.f));
				}
				else
				{
					MarkerSlot->SetPosition(FVector2D(-PositionX, 0.f));
				}
			
			}
		}
	}
	
}

void UCompassWidget::UpdateCompassMarkers(TArray<float>& CompassMarkers)
{
	for (int32 i=0; i<CompassMarkers.Num(); i++)
	{
		if (Markers[i])
		{
			SetMarkerPosition(Markers[i], CompassMarkers[i], false);
			Markers[i]->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			UImage* NewImage = AddCompassMarker();
			SetMarkerPosition(NewImage, CompassMarkers[i], false);
		}
	}

	if (CompassMarkers.Num() < Markers.Num())
	{
		for (int32 i=CompassMarkers.Num(); i<Markers.Num(); i++)
		{
			Markers[i]->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}




