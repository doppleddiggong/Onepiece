// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file APlayerControl.h
 * @brief APlayerControl 선언에 대한 Doxygen 주석을 제공합니다.
 */
#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"
#include "APlayerControl.generated.h"

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Dopple))
class ONEPIECE_API APlayerControl : public APlayerController
{
	GENERATED_BODY()

public:
	APlayerControl();

protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

	// --- Input Assets ---
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<class UInputMappingContext> IMC_Default;	

	UPROPERTY(EditDefaultsOnly, Category="Input") TObjectPtr<class UInputAction> IA_Move;
	UPROPERTY(EditDefaultsOnly, Category="Input") TObjectPtr<class UInputAction> IA_Look;
	UPROPERTY(EditDefaultsOnly, Category="Input") TObjectPtr<class UInputAction> IA_Jump;
	UPROPERTY(EditDefaultsOnly, Category="Input") TObjectPtr<class UInputAction> IA_Record;
	UPROPERTY(EditDefaultsOnly, Category="Input") TObjectPtr<class UInputAction> IA_Grab;
	UPROPERTY(EditDefaultsOnly, Category="Input") TObjectPtr<class UInputAction> IA_Interact;
	UPROPERTY(EditDefaultsOnly, Category="Input") TObjectPtr<class UInputAction> IA_Run;
	UPROPERTY(EditDefaultsOnly, Category="Input") TObjectPtr<class UInputAction> IA_Info;

	// --- Handlers ---
	void OnMove(const FInputActionValue& Value);
	void OnLook(const FInputActionValue& Value);
	
	void OnStopMove(const FInputActionValue& Value);
	void OnJump(const FInputActionValue& Value);
	void OnRun(const FInputActionValue& Value);

	void OnRecordPressed(const FInputActionValue& Value);
	void OnRecordReleased(const FInputActionValue& Value);

	void OnGrab(const FInputActionValue& Value);
	void OnGrabRelease(const FInputActionValue& Value);

	void OnInteract(const FInputActionValue& Value);

	void OnInfo(const FInputActionValue& Value);

	UFUNCTION(Server, Reliable)
	void Server_OnGrab();	

	UFUNCTION(Server, Reliable)
	void Server_OnGrabRelease();

	UFUNCTION(Server, Reliable)
	void Server_OnInteract();
	
private:
    class IControllable* GetControllable() const;
};
