// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ECompassMarkerType.h"
#include "CompassWidget.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FCompassInfo
{
	GENERATED_BODY()

	ECompassMarkerType MarkerType;
	float RotationAtTarget;
	
};

UCLASS()
class ONEPIECE_API UCompassWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	
public:
	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* Pnl_Compass;

	UPROPERTY(meta = (BindWidget))
	class UImage* Img_Compass;
	
	UFUNCTION(BlueprintCallable)
	void RotateCompass(float ZRotation);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<UImage*> Markers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Compass|Markers")
	TMap<ECompassMarkerType, UTexture2D*> MarkerTextureMap;

	/// @brief 마커별 거리 텍스트 매핑
	UPROPERTY()
	TMap<UImage*, class UTextBlock*> MarkerDistanceMap;

	// 마커 이미지 설정
	UImage* AddCompassMarker(ECompassMarkerType MarkerType);
	UTexture2D*  GetTextureForMarkerType(ECompassMarkerType MarkerType);
	// 마커 회전값
	void SetMarkerPosition(UImage* InMarker, float TargetRotation, bool bSideLock);
	/// @brief 마커 거리 설정 (미터 단위)
	void SetMarkerDistance(UImage* InMarker, float Distance);
	/// @brief 마커와 거리 텍스트의 Visibility 동시 설정
	void SetMarkerVisibility(UImage* InMarker, ESlateVisibility InVisibility);
	// 마커들 업데이트
	//void UpdateCompassMarkers(TArray<FCompassInfo>& CompassInfos);
};
