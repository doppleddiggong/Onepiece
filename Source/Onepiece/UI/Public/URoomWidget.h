// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "URoomWidget.generated.h"

/**
 * 
 */
UCLASS()
class ONEPIECE_API URoomWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	void InitWidget();
	void UpdateLoadingSpinner(float DeltaTime);
	void UpdateRoomId(const int32 InRoomId);
	
private:
	UFUNCTION(BlueprintCallable, Category = "State|Network")
	void OnNetworkWaitCount(int NetworkWaitCount);

public:
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "State|NickName")
	TObjectPtr<class UBorder> Border_State;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "State|NickName")
	TObjectPtr<class UTextBlock> Txt_RoomId;
	
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "State|Network")
	TObjectPtr<class UImage> LoadingSpinner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State|Network")
	float SpinnerRotationSpeed = 90.0f;
};
