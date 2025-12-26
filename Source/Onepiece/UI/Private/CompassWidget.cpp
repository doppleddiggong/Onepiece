// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "CompassWidget.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"

void UCompassWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (MarkerTextureMap.Num() == 0)
	{
		MarkerTextureMap.Add(ECompassMarkerType::QuestStart,
			LoadObject<UTexture2D>(nullptr, TEXT("/Game/CustomContents/UI/Texture/Resource/Icon_Listen.Icon_Listen")));
		MarkerTextureMap.Add(ECompassMarkerType::QuestEnd,
			LoadObject<UTexture2D>(nullptr, TEXT("/Game/StarterBundle/Image/mark1.mark1")));
		MarkerTextureMap.Add(ECompassMarkerType::Wheatly,
			LoadObject<UTexture2D>(nullptr, TEXT("/Game/CustomContents/UI/Texture/Resource/Icon_Whitney.Icon_Whitney")));
		MarkerTextureMap.Add(ECompassMarkerType::Teleporter,
			LoadObject<UTexture2D>(nullptr, TEXT("/Game/CustomContents/UI/Texture/Resource/resource_score.resource_score")));
		MarkerTextureMap.Add(ECompassMarkerType::FinalResult,
			LoadObject<UTexture2D>(nullptr, TEXT("/Game/CustomContents/UI/Texture/Resource/Icon_Overall.Icon_Overall")));
		MarkerTextureMap.Add(ECompassMarkerType::OtherPlayer,
			LoadObject<UTexture2D>(nullptr, TEXT("/Game/CustomContents/UI/Texture/Resource/Icon_RedOwl.Icon_RedOwl")));
	}
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

UImage* UCompassWidget::AddCompassMarker(ECompassMarkerType MarkerType)
{
	UImage* NewMarker = NewObject<UImage>(this);
	if (NewMarker)
	{
		UTexture2D* CompassTex = GetTextureForMarkerType(MarkerType);
		if (CompassTex)
		{
			NewMarker->SetBrushFromTexture(CompassTex);
		}
		
		Pnl_Compass->AddChild(NewMarker);

		UCanvasPanelSlot* MarkerSlot = Cast<UCanvasPanelSlot>(NewMarker->Slot);
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

UTexture2D* UCompassWidget::GetTextureForMarkerType(ECompassMarkerType MarkerType)
{
	UTexture2D** FoundTexture = MarkerTextureMap.Find(MarkerType);
	if (FoundTexture && *FoundTexture)
	{
		return *FoundTexture;
	}

	return LoadObject<UTexture2D>(nullptr, TEXT("/Game/StarterBundle/Image/mark1.mark1"));
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

// void UCompassWidget::UpdateCompassMarkers(TArray<FCompassInfo>& CompassInfos)
// {
// 	for (int32 i=0; i<CompassMarkers.Num(); i++)
// 	{
// 		if (Markers[i])
// 		{
// 			SetMarkerPosition(Markers[i], CompassMarkers[i], false);
// 			Markers[i]->SetVisibility(ESlateVisibility::Visible);
// 		}
// 		else
// 		{
// 			UImage* NewImage = AddCompassMarker();
// 			SetMarkerPosition(NewImage, CompassMarkers[i], false);
// 		}
// 	}
//
// 	if (CompassMarkers.Num() < Markers.Num())
// 	{
// 		for (int32 i=CompassMarkers.Num(); i<Markers.Num(); i++)
// 		{
// 			Markers[i]->SetVisibility(ESlateVisibility::Hidden);
// 		}
// 	}
// }




