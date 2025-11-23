// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "APredestalSwitch.generated.h"

UCLASS()
class ONEPIECE_API APredestalSwitch : public AActor
{
	GENERATED_BODY()
	
public:	
	APredestalSwitch();

protected:
	virtual void BeginPlay() override;
	
public:
	void InitSwitch();
	
	/** 상호작용 콜백 (InteractableComponent 델리게이트용) */
	UFUNCTION()
	void OnActivated(AActor* Interactor);

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class USkeletalMeshComponent> SwitchBody;
	
	/** InteractableComponent - 상호작용 시스템 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UInteractableComponent> InteractableComp;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class UTweenAnimInstance> AnimBlueprint;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Switch")
	int ButtonIndex = 0;
	
	/** 버튼이 눌린 상태를 유지하는 시간 (초) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Switch")
	float Duration = 0.5f;

private:
	/** 버튼을 원래 상태로 되돌리는 타이머 */
	FTimerHandle ResetTimerHandle;
	
	/** 버튼을 원래 상태로 되돌림 */
	void ResetButton();
};
