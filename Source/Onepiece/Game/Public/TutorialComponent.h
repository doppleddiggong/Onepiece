// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ETutorialStep.h"
#include "TutorialComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ONEPIECE_API UTutorialComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UTutorialComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

public:
	// 현재 튜토리얼 단계
	UPROPERTY(BlueprintReadOnly, Category = "Tutorial")
	ETutorialStep CurrentStep;

	UFUNCTION(BlueprintCallable, Category = "Tutorial")
	void StartTutorial();

	UFUNCTION(BlueprintCallable, Category = "Tutorial")
	void AdvanceToNextStep();
	// 특정 단계로 이동
	UFUNCTION(BlueprintCallable, Category = "Tutorial")
	void SetStep(ETutorialStep NewStep);
	// 튜토리얼 완료됐는지 확인
	UFUNCTION(BlueprintCallable, Category = "Tutorial")
	bool IsTutorialCompleted() const;

private:
	// 현 단계의 입력 조건 확인
	void CheckInputConditions();
	// 다음 단계 반환
	ETutorialStep GetNextStep(ETutorialStep Step) const;
	
	/** 각 단계별 입력 조건 체크 함수들 */
	bool CheckMouseLookInput() const;
	bool CheckMovementInput() const;
	bool CheckSprintInput() const;
	bool CheckJumpInput() const;
	bool CheckPickUpInput() const;
	bool CheckGrabGunInput() const;
	bool CheckInteractionInput() const;

	// 단계별 메세지 반환
	FText GetTutorialMessage(ETutorialStep Step) const;
	
	// 소유 PlayerController 캐시
	APlayerController* OwnerController;

	// 이전 프레임의 카메라 회전값
	FRotator LastControlRotation;
};
