// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "APredestalSwitch.h"

#include "GameLogging.h"
#include "UTweenAnimInstance.h"
#include "InteractableComponent.h"

#define SWITCH_COLLISION_PATH		TEXT("Collision")

APredestalSwitch::APredestalSwitch()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	SwitchBody = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SwitchBody"));
	SwitchBody->SetupAttachment(RootComponent);
	SwitchBody->SetRelativeLocation(FVector::ZeroVector);

	// InteractableComponent 생성
	InteractableComp = CreateDefaultSubobject<UInteractableComponent>(TEXT("Interactable"));
	InteractableComp->InteractionType = EInteractionType::Button;
	InteractableComp->InteractionPrompt = TEXT("Press E to Activate");
}

void APredestalSwitch::BeginPlay()
{
	Super::BeginPlay();

	// InitSwitch 호출
	InitSwitch();

	// 델리게이트 바인딩
	InteractableComp->OnInteractionTriggered.AddDynamic(this, &APredestalSwitch::OnActivated);
}

void APredestalSwitch::InitSwitch()
{
	UAnimInstance* AnimInstance = SwitchBody->GetAnimInstance();
	if (!AnimInstance)
	{
		PRINTLOG( TEXT("AnimInstance is null"));
		return;
	}

	// 7. 캐스팅 및 변수 설정
	UTweenAnimInstance* AnimBP = Cast<UTweenAnimInstance>(AnimInstance);
	if (AnimBP)
	{
		AnimBlueprint = AnimBP;
	}
	else
	{
		PRINTLOG( TEXT("Failed to cast to AnimBP"));
	}
}

void APredestalSwitch::OnActivated(AActor* Interactor)
{
	if (AnimBlueprint)
	{
		// 버튼 눌림 애니메이션
		AnimBlueprint->ChangeState(true);
		
		// Duration 후에 버튼 리셋
		GetWorld()->GetTimerManager().SetTimer(
			ResetTimerHandle,
			this,
			&APredestalSwitch::ResetButton,
			Duration,
			false
		);
	}
	else
	{
		PRINTLOG( TEXT("AnimBlueprint is null"));
	}

	PRINT_STRING(TEXT("PredestalSwitch ACTIVATED"));
	// 블루프린트에서 추가 기능 구현 가능
}

void APredestalSwitch::ResetButton()
{
	if (AnimBlueprint)
	{
		// 버튼 원래 상태로 복귀
		AnimBlueprint->ChangeState(false);
		PRINT_STRING(TEXT("PredestalSwitch RESET"));
	}
}