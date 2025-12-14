// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "ADropper.h"
#include "ListenAnswer.h"
#include "GameFramework/Actor.h"
#include "OrderKiosk.generated.h"

/*
 * 굴러오는 Food를 정지시켰다가 음식메쉬액터 또는 이름액터를 가져오면 진행
 * (정답판정X)
 */

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
	// Components
	// 음식 캡슐 감지
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UBoxComponent* FoodCollision;
	// 답 제출 감지
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UBoxComponent* SubmitCollision;
	
	UFUNCTION()
	void BeginFoodOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void BeginSubmitOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
protected:
	// 현재 감지중인 음식 큐브
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	class AFood* CurrentFoodContainer;
	
	UPROPERTY(EditAnywhere)
	EAnswerType InAnswerType = EAnswerType::None;
	
	bool IsOnceStopped = false;
	
	// 움직이거나 멈추게 할 컨베이어 리스트
	// (맵에서 직접 선택)
	UPROPERTY(EditAnywhere)
	TArray<AActor*> ConveyorsToControl;

	UFUNCTION(Server, Reliable)
	void Server_DestroyListenAnswer(AActor* ActorToDestroy);

	/** 이번 스폰에서 사용할 데이터 */
	//UPROPERTY(ReplicatedUsing=OnRep_FoodData)
	//FFoodData FoodData;
	//-----------------------------
// public:
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
// 	class UStaticMeshComponent* Mesh;
//
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
// 	class UBoxComponent* Collision;
//
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
// 	class UInteractableComponent* InteractableComp;
//
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
// 	class UWidgetComponent* InteractWidget;
//
// 	UFUNCTION()
// 	void OnInteractionTriggered(AActor* Interactor);
// 	
// public:
// 	// 자신의 인덱스
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
// 	int32 Index = -1;
// 	
// 	// 실행시킬 푸드코트 부스 인덱스 (-1이면 지정 안됨)
// 	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
// 	int32 FoodCourtIdx = -1;
//
// 	
//
// 	UFUNCTION()
// 	void OnRep_FoodData();
//
// 	// 사용 여부 플래그
// 	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
// 	bool bIsUsed = false;
//
// 	UPROPERTY(Replicated)
// 	bool IsOverlapping = false;
// 	
// 	class ADropper* FindDropperByIdx(int32 InIdx);
//
// 	void UpdateInteractableWidget(FString NewString);
};

