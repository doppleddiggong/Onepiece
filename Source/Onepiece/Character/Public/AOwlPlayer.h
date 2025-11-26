// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "AOwlPlayer.generated.h"

UCLASS()
class ONEPIECE_API AOwlPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	AOwlPlayer();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
public:
	// Interact System
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components|System")
	TObjectPtr<class UInteractionSystem> InteractionSystem;
	
	// Hold Position
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components|Position")
	TObjectPtr<class USceneComponent> HoldPosition;
	
protected:
	// Camera
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components|Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> CameraBoom;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components|Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> FollowCamera;
		
	// Input
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<class UInputMappingContext> RobotIMC;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<class UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<class UInputAction> LookAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<class UInputAction> JumpAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<class UInputAction> RunAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<class UInputAction> InteractAction;
	
	// UPROPERTY(EditAnywhere, Category="Input")
	// TObjectPtr<class UInputAction> GrabAction;
	
	void OnMove(const FInputActionValue& Value);
	void OnStopMove();
	void OnLook(const FInputActionValue& Value);
	void OnJump();
	void OnRun();
	
	// Interact
	// void OnGrab(const FInputActionValue& Value);
	// void OnGrabRelease(const FInputActionValue& Value);
	void OnInteract(const FInputActionValue& Value);
	
private:
	// Move Value
	float WalkSpeed = 200.f;
	float RunSpeed = 500.f;
	bool bIsRunning;
	bool bIsJumpStart;
	
public:
	// Get Value
	bool GetIsRunning();
	bool GetIsJumpStart();
	
	// Jump
	void DoJump();
};
