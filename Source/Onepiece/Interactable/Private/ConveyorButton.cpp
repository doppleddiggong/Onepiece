// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "ConveyorButton.h"

#include "ConveyorBelt.h"
#include "GameLogging.h"
#include "InteractableComponent.h"
#include "UInteractWidget.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

#define INTERACT_WIDGET_PATH TEXT("/Game/CustomContents/UI/Widgets/WBP_InteractWidget.WBP_InteractWidget_C")

// Sets default values
AConveyorButton::AConveyorButton()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	RootSceneComp = CreateDefaultSubobject<USceneComponent>("RootSceneComp");
	SetRootComponent(RootSceneComp);
	
	ButtonMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>("ButtonMeshComp");
	ButtonMeshComp->SetupAttachment(GetRootComponent());
	ConstructorHelpers::FObjectFinder<USkeletalMesh> buttonMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/CustomContents/Platfrom/Assets/ConveyorBelt_Button/button.button'"));
	if (buttonMeshRef.Succeeded())
	{
		ButtonMeshComp->SetSkeletalMesh(buttonMeshRef.Object);
		ButtonMeshComp->SetRelativeScale3D(FVector(0.4f));
	}
	
	InteractableComp = CreateDefaultSubobject<UInteractableComponent>(TEXT("InteractableComp"));
	InteractableComp->InteractionType = EInteractionType::Button;
	InteractableComp->InteractionPrompt = TEXT("Activate");

	WidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComp"));
	ConstructorHelpers::FClassFinder<UInteractWidget> WidgetRef(INTERACT_WIDGET_PATH);
	if (WidgetRef.Succeeded())
	{
		WidgetComp->SetWidgetClass(WidgetRef.Class);
		WidgetComp->SetupAttachment(GetRootComponent());
	}
	
	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	BoxComp->SetupAttachment(GetRootComponent());
	BoxComp->SetRelativeLocation(FVector(0.0f, 0.0f, 45.f));
	BoxComp->SetBoxExtent(FVector(32, 32, 45));
	BoxComp->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	BoxComp->SetCollisionObjectType(ECC_WorldStatic);
	BoxComp->SetCollisionResponseToAllChannels(ECR_Block);
	
	bReplicates = true;
}

// Called when the game starts or when spawned
void AConveyorButton::BeginPlay()
{
	Super::BeginPlay();
	
	// Find Conveyor Belt Actors
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AConveyorBelt::StaticClass(), ConveyorBeltActors);
	
	// Bind Delegate
	InteractableComp->InitWidget(WidgetComp);
	InteractableComp->OnInteractionTriggered.AddDynamic(this, &AConveyorButton::OnInteractionTriggered);
}

void AConveyorButton::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AConveyorButton, ConveyorBeltActors);
	DOREPLIFETIME(AConveyorButton, bIsButtonOn);
}

// Called every frame
void AConveyorButton::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AConveyorButton::GetIsButtonOn()
{
	return bIsButtonOn;
}

void AConveyorButton::OnInteractionTriggered(AActor* Interactor)
{
	if (ConveyorBeltActors.IsEmpty()) return;
	
	bIsButtonOn = !bIsButtonOn;
	PRINT_STRING(TEXT("%d"), bIsButtonOn);
	for (const auto& Belt : ConveyorBeltActors)
	{
		AConveyorBelt* ConveyorBeltActor = Cast<AConveyorBelt>(Belt);
		ConveyorBeltActor->ChangeConveyorMovement();
	}
	
}

void AConveyorButton::ServerRPC_OnInteractionTriggered_Implementation(AActor* Interactor)
{
	
}

void AConveyorButton::MultiCastRPC_OnInteractionTriggered_Implementation(AActor* Interactor)
{
	
}
