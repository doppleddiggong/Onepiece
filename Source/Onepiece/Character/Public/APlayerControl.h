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
	UPROPERTY(EditDefaultsOnly, Category="Input") TObjectPtr<class UInputAction> IA_AltitudeUp;
	UPROPERTY(EditDefaultsOnly, Category="Input") TObjectPtr<class UInputAction> IA_AltitudeDown;
	UPROPERTY(EditDefaultsOnly, Category="Input") TObjectPtr<class UInputAction> IA_Jump;
	UPROPERTY(EditDefaultsOnly, Category="Input") TObjectPtr<class UInputAction> IA_Landing;

	// --- Handlers ---
	void OnMove(const FInputActionValue& Value);
	void OnLook(const FInputActionValue& Value);
	void OnAltitudeUp(const FInputActionValue& Value);
	void OnAltitudeDown(const FInputActionValue& Value);
	void OnAltitudeReleased(const FInputActionValue& Value);
	
	void OnJump(const FInputActionValue& Value);

	void OnLanding(const FInputActionValue& Value);
	
private:
    class IControllable* GetControllable() const;
};
