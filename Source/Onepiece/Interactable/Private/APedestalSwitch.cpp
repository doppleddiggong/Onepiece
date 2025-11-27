// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "APedestalSwitch.h"

#include "GameLogging.h"
#include "UTweenAnimInstance.h"
#include "InteractableComponent.h"
#include "UInteractWidget.h"
#include "Components/WidgetComponent.h"

#define INTERACT_WIDGET_PATH TEXT("/Game/CustomContents/UI/Widgets/WBP_InteractWidget.WBP_InteractWidget_C")

APedestalSwitch::APedestalSwitch()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	SwitchBody = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SwitchBody"));
	SwitchBody->SetupAttachment(RootComponent);
	SwitchBody->SetRelativeLocation(FVector::ZeroVector);

	InteractableComp = CreateDefaultSubobject<UInteractableComponent>(TEXT("Interactable"));
	InteractableComp->InteractionType = EInteractionType::Button;
	InteractableComp->InteractionPrompt = TEXT("Activate");

	WidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComp"));
	ConstructorHelpers::FClassFinder<UInteractWidget> WidgetRef(INTERACT_WIDGET_PATH);
	if (WidgetRef.Succeeded())
	{
		WidgetComp->SetWidgetClass(WidgetRef.Class);
		WidgetComp->SetupAttachment(GetRootComponent());
	}
}

void APedestalSwitch::BeginPlay()
{
	Super::BeginPlay();

	// InitSwitch 호출
	InitSwitch();

	// 델리게이트 바인딩
	InteractableComp->InitWidget(WidgetComp);
	InteractableComp->OnInteractionTriggered.AddDynamic(this, &APedestalSwitch::OnInteractionTriggered);
}

void APedestalSwitch::InitSwitch()
{
	UAnimInstance* AnimInstance = SwitchBody->GetAnimInstance();
	if (!AnimInstance)
	{
		PRINTLOG( TEXT("AnimInstance is null"));
		return;
	}

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

void APedestalSwitch::OnInteractionTriggered(AActor* Interactor)
{
	if (AnimBlueprint)
	{
		// 버튼 눌림 애니메이션
		AnimBlueprint->ChangeState(true);
		
		// 딜레이 후 버튼 리커버리
		GetWorld()->GetTimerManager().SetTimer(
			RecoveryTimerHandle,
			this,
			&APedestalSwitch::RecoveryButton,
			RecoveryDelay,
			false
		);
	}
	else
	{
		PRINTLOG( TEXT("AnimBlueprint is null"));
	}

	OnActivate();
}

void APedestalSwitch::RecoveryButton()
{
	AnimBlueprint->ChangeState(false);
}

void APedestalSwitch::OnActivate_Implementation()
{
	PRINT_STRING(TEXT("Pedestal Switch Activated"));
}