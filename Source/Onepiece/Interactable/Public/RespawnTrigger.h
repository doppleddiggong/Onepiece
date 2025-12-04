// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "ALingoGameState.h"
#include "GameFramework/Actor.h"
#include "RespawnTrigger.generated.h"

/*
 * 닿으면 플레이어를 특정 위치로 옮겨주는 트리거박스
 */
UCLASS()
class ONEPIECE_API ARespawnTrigger : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARespawnTrigger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	// Components
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* Collision;

	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	// 되돌아갈 리스폰 지점
	UPROPERTY(EditAnywhere, Category = "Respawn")
	EQuestType RespawnQuest = EQuestType::None;

	UFUNCTION(Server, Reliable)
	void Server_RepawnPlayer(ACharacter* Player);
};
