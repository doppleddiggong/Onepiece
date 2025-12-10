// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "URoomWidget.h"

#include "UBroadcastManager.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void URoomWidget::InitWidget()
{
	if (LoadingSpinner)
		LoadingSpinner->SetVisibility(ESlateVisibility::Hidden);

	Txt_RoomId->SetText(FText::FromString(""));

	if (auto EventManager = UBroadcastManager::Get(GetWorld()))
		EventManager->OnNetworkWaitCount.AddDynamic(this, &URoomWidget::OnNetworkWaitCount);
}

void URoomWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateLoadingSpinner(InDeltaTime);
}

void URoomWidget::UpdateRoomId(const int32 InRoomId)
{
	Txt_RoomId->SetText(FText::FromString(FString::Printf(TEXT("%d"), InRoomId )));
}

void URoomWidget::UpdateLoadingSpinner(float DeltaTime)
{
	if (!LoadingSpinner || !LoadingSpinner->IsVisible())
		return;

	const float NewAngle = LoadingSpinner->GetRenderTransformAngle() + (SpinnerRotationSpeed * DeltaTime);
	LoadingSpinner->SetRenderTransformAngle(NewAngle);
}

void URoomWidget::OnNetworkWaitCount(int NetworkWaitCount)
{
	if (!LoadingSpinner)
		return;

	LoadingSpinner->SetVisibility(NetworkWaitCount > 0 ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}