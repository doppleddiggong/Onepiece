// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "QuestionnaireKiosk.generated.h"

UCLASS()
class ONEPIECE_API AQuestionnaireKiosk : public AActor
{
	GENERATED_BODY()

public:
	AQuestionnaireKiosk();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:
	virtual void Tick(float DeltaTime) override;
	
protected:
	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class USceneComponent> RootSceneComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UStaticMeshComponent> KioskMeshComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UInteractableComponent> InteractableComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UBoxComponent> BoxComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UWidgetComponent> WidgetComp;
	
	// Interaction
	UFUNCTION()
	void OnInteractionTriggered(AActor* Interactor);
	
	UFUNCTION(Server, Reliable)
	void ServerRPC_OnInteractionTriggered(AActor* Interactor);
	
	UFUNCTION(Client, Reliable)
	void ClientRPC_OnInteractionTriggered(AActor* Interactor);
};
