// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "NetworkData.h"
#include "GameFramework/Actor.h"
#include "AEvaluationTrigger.generated.h"

UCLASS()
class ONEPIECE_API AEvaluationTrigger : public AActor
{
	GENERATED_BODY()

public:
	AEvaluationTrigger();
	
protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
	void OnActivate();

	virtual void OnActivate_Implementation();
	void OnResponseEvaluationResult(FResponseEvaluationResult& ResponseData, bool bWasSuccessful);

protected:
	/// @brief 트리거 영역을 정의하는 박스 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Trigger")
	TObjectPtr<class UBoxComponent> TriggerBox;

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
