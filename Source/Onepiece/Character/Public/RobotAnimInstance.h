// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "RobotAnimInstance.generated.h"

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
	
	// 이동 속도
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Values")
	FVector velocity;
	// 공중에 있는지 여부
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Values")
	bool isAir;
	// 수평 이동 속력
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Values")
	float groundSpeed;
	
	UFUNCTION()
	void AnimNotify_OnJumpStart();
};
