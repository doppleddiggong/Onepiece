// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "CompassTargetInterface.h"
#include "GameFramework/Actor.h"
#include "ALuggageHolder.generated.h"

UCLASS()
class ONEPIECE_API ALuggageHolder : public AActor, public ICompassTargetInterface
{
	GENERATED_BODY()

public:
	ALuggageHolder();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:
	// Events
	UFUNCTION(BlueprintImplementableEvent, Category = "Holder")
	void OnActivate(bool bSuccess);

	/**
	 * @brief [Multicast RPC] 모든 클라이언트에 정답 결과 팝업 표시
	 * @param CorrectAnswerIndex 정답 인덱스 (WrongAnswerList 마지막에 추가)
	 */
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ShowResultPopup(int32 CorrectAnswerIndex);

	/**
	 * @brief [Multicast RPC] 모든 클라이언트에 오답 메시지 표시
	 * @param LuggageColor 선택한 Luggage 색상
	 * @param LuggagePattern 선택한 Luggage 무늬
	 */
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ShowWrongPopup(const FString& LuggageColor, const FString& LuggagePattern);
	
private:
	UFUNCTION()
	void OnRep_IsActivated();

	UFUNCTION()
	void OnRep_CurTarget();

	UFUNCTION()
	void OnBoxOverlapBegin(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	bool CheckLuggage(class Aluggage* TargetLuggage, int CorrectIndex);

	void UpdateActivateState(bool State);
	
	/** 상호작용 위젯 빌보드화 (카메라를 향하도록) */
	void BillboardInteractWidget();
	
public:
	// Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<class UBoxComponent> BoxCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<class USkeletalMeshComponent> MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	TObjectPtr<class USceneComponent> HoldPos;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TObjectPtr<class UWidgetComponent> WidgetGuideComp;

protected:
	// 현재 올라가 있는 액터
	UPROPERTY(ReplicatedUsing=OnRep_CurTarget)
	TObjectPtr<class AActor> CurTarget;
	
	// State
	UPROPERTY(ReplicatedUsing=OnRep_IsActivated, VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsActivated = false;

	// Visual Settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	float ActivatedHeightOffset = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	float RotationSpeed = 90.0f;
	
protected:
	// Compass Interface
	virtual void SetCompassMarkerInto(ECompassMarkerType InMarkerType) override;
	
private:
	// Luggage Destroy Value
	FTimerHandle DestroyTimerHandle;
};