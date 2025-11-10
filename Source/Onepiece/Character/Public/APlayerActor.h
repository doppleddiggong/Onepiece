// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file APlayerActor.h
 * @brief Declares the player-controlled character actor.
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "IControllable.h"
#include "APlayerActor.generated.h"

/**
 * @brief Main character driven directly by the player.
 * @details Extends AGameCharacter with player-specific input and camera handling.
 * @ingroup Character
 */
UCLASS(Blueprintable, BlueprintType, ClassGroup=(Dopple))
class ONEPIECE_API APlayerActor : public ACharacter, public IControllable
{
	GENERATED_BODY()

public:
	APlayerActor();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	virtual void Landed(const FHitResult& Hit) override;

	UFUNCTION(BlueprintCallable, Category="Fly")
	void SetFlying();
	UFUNCTION(BlueprintCallable, Category="Fly")
	void SetFallingToWalk();

	UFUNCTION(BlueprintCallable, Category="Command")
	void RecoveryMovementMode(const EMovementMode InMovementMode);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Command")
	void OnFlyEnd();
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components|System")
	TObjectPtr<class UFlySystem> FlySystem;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess="true"))
	TObjectPtr<class USpringArmComponent> SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess="true"))
	TObjectPtr<class UCameraComponent> FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="RushAttack|Owner")
	TObjectPtr<class UCharacterMovementComponent> MoveComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TEnumAsByte<EMovementMode> PrevMoveMode;
	
public: // Control Interface
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Command")
	void Cmd_Move(const FVector2D& Axis) override;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Command")
	void Cmd_Look(const FVector2D& Axis) override;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Command")
	void Cmd_AltitudeUp() override;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Command")
	void Cmd_AltitudeDown() override;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Command")
	void Cmd_AltitudeReleased() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Command")
	void Cmd_Jump() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Command")
	void Cmd_Landing() override;
};
