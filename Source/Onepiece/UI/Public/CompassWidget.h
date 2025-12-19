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
	
	UImage* AddCompassMarker();
	void SetMarkerPosition(UImage* InMarker, float TargetRotation, bool bSideLock);

	void UpdateCompassMarkers(TArray<float>& CompassMarkers);
};
