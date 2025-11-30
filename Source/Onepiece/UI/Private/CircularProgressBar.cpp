// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "CircularProgressBar.h"

#include "Components/Image.h"

void UCircularProgressBar::NativeConstruct()
{
	Super::NativeConstruct();
}

void UCircularProgressBar::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UCircularProgressBar::SetPercent(float Percent)
{
	auto DynamicMaterial = Img_CircularBar->GetDynamicMaterial();
	if (DynamicMaterial)
	{
		float ClampedPercent = FMath::Clamp(Percent, 0, 1);
		DynamicMaterial->SetScalarParameterValue(TEXT("Percent"), ClampedPercent);
		
	}
	
}

double UCircularProgressBar::easeOutSine(double x)
{
	return FMath::Sin((x * PI) / 2.0);
}
