// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "APedestalSwitch.generated.h"

UCLASS()
class ONEPIECE_API APedestalSwitch : public AActor
{
	GENERATED_BODY()
	
public:	
	APedestalSwitch();

protected:
	virtual void BeginPlay() override;
	
public:
	void InitSwitch();
	
	/** 상호작용 콜백 (InteractableComponent 델리게이트용) */
	UFUNCTION()
	void OnInteractionTriggered(AActor* Interactor);

	UFUNCTION()
	void OnOutlineStateChanged(bool bShouldShowOutline);

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
	void OnActivate();
	virtual void OnActivate_Implementation();
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class USkeletalMeshComponent> SwitchBody;
	
	/** InteractableComponent - 상호작용 시스템 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UInteractableComponent> InteractableComp;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<class UWidgetComponent> WidgetComp;
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class UTweenAnimInstance> AnimBlueprint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Switch")
	float RecoveryDelay = 0.5f;

private:
	/** 버튼을 원래 상태로 되돌리는 타이머 */
	FTimerHandle RecoveryTimerHandle;
	
	/** 버튼을 원래 상태로 되돌림 */
	void RecoveryButton();
};
