// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AWeightSwitch.generated.h"


UCLASS()
class ONEPIECE_API AWeightSwitch : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeightSwitch();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	void InitSwitch();
	void SetActivate(bool State);

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
	void OnActivate(const bool State);
	virtual void OnActivate_Implementation(const bool State);
	
private:
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
						UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
						bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
					  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	UFUNCTION(BlueprintCallable, Category="SwitchEvent")
	void OnWeightSwitch(int InButtonIndex, bool InActive);

	bool ActivateTrigger();

	/**
	 * @brief [Multicast RPC] 모든 클라이언트에 정답 결과 팝업 표시
	 * @details [문제] 서버에서만 팝업을 표시하여 클라이언트에서 보이지 않음
	 *          [해결] Multicast RPC로 모든 머신에 팝업 전달
	 */
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ShowResultPopup();

	/**
	 * @brief [Multicast RPC] 모든 클라이언트에 오답 메시지 표시
	 * @param LuggageColor 선택한 Luggage 색상
	 * @param LuggagePattern 선택한 Luggage 무늬
	 */
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ShowWrongPopup(const FString& LuggageColor, const FString& LuggagePattern);
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class USkeletalMeshComponent> SwitchBody;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UBoxComponent> SwitchCollision;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class UTweenAnimInstance> AnimBlueprint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UMaterialInstanceDynamic> EmissiveMaterial;

	UPROPERTY()
	TArray<class AActor*> OverlappingActors;	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Switch")
	int ButtonIndex = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Switch")
	float Duration = 0.25f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Switch")
	float TriggerDelay = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	FLinearColor ActivateColor = FLinearColor::Blue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	FLinearColor DeactivateColor = FLinearColor::Yellow;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName EmissiveParam = "EmissiveColor";

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsPlayerDetect = false;

	
private:
	float ElapsedTime = 0.0f;
	bool DetectTarget = false;
	bool bActivateState = false;

	bool AnswerFound = false;
};
