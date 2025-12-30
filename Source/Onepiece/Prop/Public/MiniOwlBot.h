// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MiniOwlBot.generated.h"

UCLASS()
class ONEPIECE_API AMiniOwlBot : public APawn
{
	GENERATED_BODY()

public:
	AMiniOwlBot();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaSeconds) override;
	
protected:
	UPROPERTY()
	TObjectPtr<class USceneComponent> rootSceneComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components|Widget")
	TObjectPtr<class UWidgetComponent> speechWidget;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components|Mesh")
	TObjectPtr<class UStaticMeshComponent> meshComp;
	
public:
	void UpdateLocation(float DeltaTime);
	void UpdateText(const FString& text);
	
private:
	UFUNCTION(Server, Reliable)
	void ServerRPC_UpdateText(const FString& text);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_UpdateText(const FString& text);
	
private:
	// owlbot position values
	float radius = 280.f;
	float angle = 28.f;
	float amplitude = 10.f;
	float frequency = 0.4f;
	float speed = 20.f;
	float time = 0.f;
	
	// parent
	UPROPERTY()
	TObjectPtr<class APlayerActor> parentObj;	
};
