// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ADropper.generated.h"

UCLASS()
class ONEPIECE_API ADropper : public AActor
{
	GENERATED_BODY()

public:
	ADropper();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:
	/// @brief 클라이언트/서버 모두에서 호출 가능한 스폰 요청 함수
	/// @param SpawnActorClass 스폰할 액터 클래스
	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void RequestSpawnActor(TSubclassOf<class AActor> SpawnActorClass);

protected:
	UFUNCTION(Server, Reliable)
	void Server_SpawnActor(TSubclassOf<class AActor> SpawnActorClass);

private:
	/// @brief 실제 스폰 로직 (서버에서만 실행)
	class AActor* SpawnActor( TSubclassOf<class AActor> SpawnActorClass ); 
	
private:
	void OnDelayCompleted();
	void OnRestoreDelayCompleted();
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayAnimation();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_DisableCollision();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_RestoreCollision();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	TObjectPtr<class USceneComponent> SpawnPos;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<class USkeletalMeshComponent> SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<class UStaticMeshComponent> BoxCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TObjectPtr<class UAnimationAsset> AnimToPlay;

private:
	FTimerHandle DelayTimerHandle;
	FTimerHandle RestoreTimerHandle;
};