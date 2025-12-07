// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ATeleportTrigger.generated.h"

UCLASS()
class ONEPIECE_API ATeleportTrigger : public AActor
{
	GENERATED_BODY()

public:
	ATeleportTrigger();
	
protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
	void OnActivate();
	virtual void OnActivate_Implementation();

	
protected:
	/// @brief 트리거 영역을 정의하는 박스 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Trigger")
	TObjectPtr<class UBoxComponent> TriggerBox;

	/// @brief 텔레포트 목표 위치를 나타내는 TeleportOut Actor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Teleport")
	TObjectPtr<class ATeleportOut> TeleportOut;

	/// @brief 원샷 모드 활성화 여부 (true = 한 번만 작동, false = 반복 작동 가능)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Trigger")
	bool bIsOneShot;

	/// @brief 트리거 활성화 상태 (false = 활성화, true = 비활성화)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Trigger")
	bool bIsTriggered;

	/// @brief 디버그 드로우 표시 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Debug")
	bool bShowDebugBox;

	/// @brief 디버그 박스 색상
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Debug")
	FColor DebugBoxColor;

protected:
	/// @brief 트리거 박스 Overlap 시작 이벤트 핸들러
	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
