// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "APlatformSwitch.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FChangeSwitchState, bool, bIsActive);

UCLASS()
class ONEPIECE_API APlatformSwitch : public AActor
{
	GENERATED_BODY()
	
public:	
	APlatformSwitch();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

private:
	void AddElapsedTime();
	float LerpAlpha() const;
	bool ActivateTrigger();
	
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
						UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
						bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
					  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


public:
	float ElapsedTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Switch")
	bool DetectTarget = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Switch")
	float Duration = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Switch")
	float TriggerDelay = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Switch")
	FName ColorParam = FName("Color");
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Switch")
	FLinearColor IdleColor = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Switch")
	FLinearColor WarningColor = FLinearColor::Red;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Switch")
	FVector OriginVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Switch")
	FVector EndVector;

	UPROPERTY()
	class UStaticMeshComponent* SwitchButton;
	UPROPERTY()
	class UMaterialInstanceDynamic* MaterialButton;
	UPROPERTY()
	class UPrimitiveComponent* SwitchCollision;
	UPROPERTY()
	class AActor* InOtherActor;	

#pragma region SwitchEvent
	UFUNCTION(BlueprintCallable, Category="SwitchEvent")
	void ChangeActivateState(bool State);
	
	UPROPERTY(BlueprintAssignable, Category="SwitchEvent")
	FChangeSwitchState OnChangeSwitchState;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SwitchEvent")
	bool bActivateState = false;
#pragma endregion
};
