// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "NetworkData.h"
#include "GameFramework/Actor.h"
#include "ConveyorButton.generated.h"

UCLASS()
class ONEPIECE_API AConveyorButton : public AActor
{
	GENERATED_BODY()

public:
	AConveyorButton();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
	void InitConveyorButton(const FResponseReadResult& result);
	
	// Get Func
	bool GetIsButtonOn();
	
protected:
	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class USceneComponent> RootSceneComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class USkeletalMeshComponent> ButtonMeshComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UInteractableComponent> InteractableComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<class UWidgetComponent> WidgetComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UBoxComponent> BoxComp;
	
	// Conveyor Belt Actors
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	TArray<class AActor*> ConveyorBeltActors;
	
	// Interaction
	UFUNCTION()
	void OnInteractionTriggered(AActor* Interactor);

	UFUNCTION()
	void OnOutlineStateChanged(bool bShouldShowOutline);
	
	UPROPERTY(Replicated)
	bool bIsButtonOn = false;
};
