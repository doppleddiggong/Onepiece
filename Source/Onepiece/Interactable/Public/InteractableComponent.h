// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractableComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ONEPIECE_API UInteractableComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInteractableComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Initial State
	// 집어올린 상태인지 여부
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	bool bIsPickedUp;
	
	// PickUp 전의 원래 Physics simulation 상태
	bool bOriginalSimulatePhysics;

	// PickUp 전의 원래 Collision 설정
	TEnumAsByte<ECollisionEnabled::Type> OriginalCollisionType;

public:
	// Interaction
	UPROPERTY(ReplicatedUsing=OnRep_HoldingOwner)
	AActor* HoldingOwner;

	UFUNCTION()
	void OnRep_HoldingOwner();
	
	// 현재 들고 있는 상태인지 확인
	// BlueprintPure : 값만 반환, 상태 변경 없을 때 사용
	UFUNCTION(BlueprintPure, Category = "Interaction")
	FORCEINLINE bool IsPickedUp() const {return bIsPickedUp;}
	
	// 물체 집어올림
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void PickUp();

	UFUNCTION(Server, Reliable)
	void Server_PickUp();
	
	// 물체 놓음
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void Drop();

	UFUNCTION(Server, Reliable)
	void Server_Drop();

protected:
	// Owner Actor의 PrimitiveComponent 찾기
	// UPrimitiveComponent : 물리/충돌/렌더링을 가진 컴포넌트
	//						 USceneComponent의 하위에 존재
	// 컴포넌트가 붙는 액터의 Mesh를 가져오는 역할
	UPrimitiveComponent* GetOwnerPrimitiveComponent() const;
};
