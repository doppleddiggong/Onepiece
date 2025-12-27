// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "CompassWidget.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

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

		// 거리 텍스트 생성
		UTextBlock* DistanceText = NewObject<UTextBlock>(this);
		if (DistanceText)
		{
			Pnl_Compass->AddChild(DistanceText);

			UCanvasPanelSlot* TextSlot = Cast<UCanvasPanelSlot>(DistanceText->Slot);
			if (TextSlot)
			{
				TextSlot->SetAnchors(FAnchors(0.5, 0.5, 0.5, 0.5));
				TextSlot->SetSize(FVector2D(100.f, 20.f));
				TextSlot->SetAlignment(FVector2D(0.5f, -0.8f)); // 하단 중앙 정렬
				TextSlot->SetPosition(FVector2D(0.f, -5.f)); // 마커 위에 배치
				TextSlot->SetZOrder(100); // 최상위로 설정
			}

			// 텍스트 스타일 설정
			FSlateFontInfo FontInfo = DistanceText->GetFont();
			FontInfo.Size = 14;
			DistanceText->SetFont(FontInfo);
			DistanceText->SetJustification(ETextJustify::Center);
			DistanceText->SetColorAndOpacity(FLinearColor::White);
			DistanceText->SetText(FText::FromString(TEXT("0m")));
			DistanceText->SetVisibility(ESlateVisibility::Visible);

			// 텍스트에 그림자 추가 (가독성 향상)
			DistanceText->SetShadowOffset(FVector2D(1.f, 1.f));
			DistanceText->SetShadowColorAndOpacity(FLinearColor::Black);

			MarkerDistanceMap.Add(NewMarker, DistanceText);
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
		FVector2D FinalPosition = FVector2D::ZeroVector;

		if (!bSideLock)
		{
			FinalPosition = FVector2D(MarkerRotation, 0.f);
			MarkerSlot->SetPosition(FinalPosition);
		}
		else
		{
			UCanvasPanelSlot* CompassSlot = Cast<UCanvasPanelSlot>(Pnl_Compass->Slot);
			if (CompassSlot)
			{
				float PositionX = (CompassSlot->GetSize().X / 2);

				if (FMath::Abs(MarkerRotation) < PositionX)
				{
					FinalPosition = FVector2D(MarkerRotation, 0.f);
				}
				else if (MarkerRotation > PositionX)
				{
					FinalPosition = FVector2D(PositionX, 0.f);
				}
				else
				{
					FinalPosition = FVector2D(-PositionX, 0.f);
				}

				MarkerSlot->SetPosition(FinalPosition);
			}
		}

		// 거리 텍스트도 같이 이동
		if (UTextBlock** FoundText = MarkerDistanceMap.Find(InMarker))
		{
			UCanvasPanelSlot* TextSlot = Cast<UCanvasPanelSlot>((*FoundText)->Slot);
			if (TextSlot)
			{
				TextSlot->SetPosition(FVector2D(FinalPosition.X, -5.f)); // 마커 위에 배치
			}
		}
	}
}

void UCompassWidget::SetMarkerDistance(UImage* InMarker, float Distance)
{
	if (UTextBlock** FoundText = MarkerDistanceMap.Find(InMarker))
	{
		// 거리를 미터 단위로 변환 (언리얼은 센티미터 단위)
		float DistanceInMeters = Distance / 100.f;

		FString DistanceStr;
		if (DistanceInMeters >= 1000.f)
		{
			// 1km 이상이면 km 단위로 표시
			DistanceStr = FString::Printf(TEXT("%.1fkm"), DistanceInMeters / 1000.f);
		}
		else
		{
			// 1m 이상이면 m 단위로 표시
			DistanceStr = FString::Printf(TEXT("%.0fm"), DistanceInMeters);
		}

		(*FoundText)->SetText(FText::FromString(DistanceStr));
	}
}

void UCompassWidget::SetMarkerVisibility(UImage* InMarker, ESlateVisibility InVisibility)
{
	if (InMarker)
	{
		InMarker->SetVisibility(InVisibility);
	}

	// 거리 텍스트도 같은 Visibility 적용
	if (UTextBlock** FoundText = MarkerDistanceMap.Find(InMarker))
	{
		if (*FoundText)
		{
			(*FoundText)->SetVisibility(InVisibility);
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




