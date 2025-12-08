// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "ADropper.h"
#include "GameFramework/Actor.h"
#include "OrderKiosk.generated.h"

UCLASS()
class ONEPIECE_API AOrderKiosk : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AOrderKiosk();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* Collision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UInteractableComponent* InteractableComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UWidgetComponent* InteractWidget;

	UFUNCTION()
	void OnInteractionTriggered(AActor* Interactor);
	
public:
	// 실행시킬 푸드코트 부스 인덱스 (-1이면 지정 안됨)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 FoodCourtIdx = -1;

	/** 이번 스폰에서 사용할 데이터 */
	UPROPERTY(ReplicatedUsing=OnRep_FoodData)
	FFoodData FoodData;

	UFUNCTION()
	void OnRep_FoodData();

	// 사용 여부 플래그
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsUsed = false;

	UPROPERTY(Replicated)
	bool IsOverlapping = false;
	
	class ADropper* FindDropperByIdx(int32 InIdx);

	void UpdateInteractableWidget(FString NewString);
};
