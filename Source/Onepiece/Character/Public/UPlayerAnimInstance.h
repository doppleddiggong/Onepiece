// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "UPlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ONEPIECE_API URobotAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Values")
	TObjectPtr<class ARobotPlayer> OwningRobot;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Values")
	bool bIsRunning;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Values")
	bool bIsJumpStart;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Values")
	FVector velocity;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Values")
	bool bIsAir;
	
	// Velocity
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Values")
	float horizontal;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Values")
	float vertical;
	
	UFUNCTION()
	void AnimNotify_OnJumpStart();
};
