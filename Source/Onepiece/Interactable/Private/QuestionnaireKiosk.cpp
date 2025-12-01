// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "QuestionnaireKiosk.h"

#include "DrawingBoardWidget.h"
#include "InteractableComponent.h"
#include "Components/BoxComponent.h"


// Sets default values
AQuestionnaireKiosk::AQuestionnaireKiosk()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	RootSceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComp"));
	SetRootComponent(RootSceneComp);
	
	KioskMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("KioskMeshComp"));
	KioskMeshComp->SetupAttachment(RootComponent);
	
	InteractableComp = CreateDefaultSubobject<UInteractableComponent>(TEXT("InteractableComp"));
	
	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	BoxComp->SetupAttachment(GetRootComponent());
	BoxComp->SetRelativeLocation(FVector(0, 36, 75));
	BoxComp->SetBoxExtent(FVector(60, 65, 92));
	
	ConstructorHelpers::FClassFinder<UPopup_Interview> popupQuestionnaireWidgetRef(TEXT("/Game/CustomContents/UI/Widgets/WBP_PopupWriteKiosk.WBP_PopupWriteKiosk_C"));
	if (popupQuestionnaireWidgetRef.Succeeded())
	{
		PopupQuestionnaireWidgetClass = popupQuestionnaireWidgetRef.Class;
	}
}

void AQuestionnaireKiosk::BeginPlay()
{
	Super::BeginPlay();
	
	
}

void AQuestionnaireKiosk::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AQuestionnaireKiosk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AQuestionnaireKiosk::OnInteractionTriggered(AActor* Interactor)
{
}

void AQuestionnaireKiosk::ServerRPC_OnInteractionTriggered_Implementation(AActor* Interactor)
{
	
}

void AQuestionnaireKiosk::MultiCastRPC_OnInteractionTriggered_Implementation(AActor* Interactor)
{
}
