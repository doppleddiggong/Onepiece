// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "ADropper.h"
#include "CompassTargetInterface.h"
#include "ListenAnswer.h"
#include "GameFramework/Actor.h"
#include "OrderKiosk.generated.h"

/*
 * 굴러오는 Food를 정지시켰다가 음식메쉬액터 또는 이름액터를 가져오면 진행
 * (정답판정X)
 */

UCLASS()
class ONEPIECE_API AOrderKiosk : public AActor, public ICompassTargetInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AOrderKiosk();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Teleport")
	TObjectPtr<class ATeleportOut> TeleportOut;
	
public:
	// 한번 제출했었는지 여부
	bool IsOnceStopped = false;
	
protected:
	// 현재 감지중인 음식 큐브
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	class AFood* CurrentFoodContainer;
	
	UPROPERTY(EditAnywhere)
	EAnswerType InAnswerType = EAnswerType::None;
	
	// 움직이거나 멈추게 할 컨베이어 리스트
	// (맵에서 직접 선택)
	UPROPERTY(EditAnywhere)
	TArray<AActor*> ConveyorsToControl;

	UFUNCTION(Server, Reliable)
	void Server_DestroyListenAnswer(AActor* ActorToDestroy);

	UFUNCTION(Server, Reliable)
	void Server_MoveFoodContainer(AActor* ActorToMove);

protected:
	// Compass Interface
	virtual void SetCompassMarkerInto(ECompassMarkerType InMarkerType) override;
};

