// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ADoor.generated.h"

UCLASS()
class ONEPIECE_API ADoor : public AActor
{
	GENERATED_BODY()

public:	
	ADoor();

protected:
	virtual void BeginPlay() override;
	
public:
	UFUNCTION()
	void OnDoorMessage(int32 InDoorIndex, bool bInOpen);

	UFUNCTION(BlueprintCallable)
	void UpdateDoor(float InAlpha);
	
	UFUNCTION(BlueprintNativeEvent)
	void OpenDoor();
	virtual void OpenDoor_Implementation();

	UFUNCTION(BlueprintNativeEvent)
	void CloseDoor();
	virtual void CloseDoor_Implementation();

public:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> SM_Frame;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> SM_Left;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> SM_Right;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DoorIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ReqCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool OneShot = false;

	/// @brief 맵 배치 시 문이 열린 상태로 시작할지 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	bool bStartOpened = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector LeftOpen = FVector(-120,0,0);
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector RightOpen = FVector(120,0,0);
	
private:
	int32 CurCount = 0;
	float Alpha = 0.0f;
};
