// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "OrderKiosk.h"

#include "ADropper.h"
#include "ALingoGameState.h"
#include "APlayerActor.h"
#include "InteractableComponent.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


// Sets default values
AOrderKiosk::AOrderKiosk()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	Collision->SetupAttachment(GetRootComponent());

	InteractableComp = CreateDefaultSubobject<UInteractableComponent>(TEXT("InteractableComp"));
	InteractableComp->InteractionType = EInteractionType::Button;
	InteractableComp->InteractionPrompt = TEXT("Order Ticket");

	InteractWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractWidget"));
	InteractWidget->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AOrderKiosk::BeginPlay()
{
	Super::BeginPlay();

	// InteractableComp에 위젯 연결
	if (InteractableComp && InteractWidget)
	{
		InteractableComp->InitWidget(InteractWidget);
	}

	// 상호작용 델리게이트 바인딩
	if (InteractableComp)
	{
		InteractableComp->OnInteractionTriggered.AddDynamic(this, &AOrderKiosk::OnInteractionTriggered);
	}
	
	InteractWidget->SetVisibility(false);
}

void AOrderKiosk::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AOrderKiosk, IsOverlapping);
}

// Called every frame
void AOrderKiosk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AOrderKiosk::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (APlayerActor* Player = Cast<APlayerActor>(OtherActor))
	{
		InteractWidget->SetVisibility(true);
		IsOverlapping = true;
	}
}

void AOrderKiosk::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (APlayerActor* Player = Cast<APlayerActor>(OtherActor))
	{
		InteractWidget->SetVisibility(false);
		IsOverlapping = false;
	}
}

void AOrderKiosk::OnInteractionTriggered(AActor* Interactor)
{
	if (bIsUsed) return;

	// 사용됨 표시
	bIsUsed = true;
	
	// InteractableComp 비활성화 (재사용 방지)
	/*if (InteractableComp)
	{
		InteractableComp->bCanInteract = false;
	}
	*/
	ALingoGameState* GS = Cast<ALingoGameState>(GetWorld()->GetGameState());
	if (GS)
	{
		// Food 스폰
		ADropper* Dropper = FindDropperByIdx(FoodCourtIdx);
		if (Dropper)
		{
			FFoodData tmpData;
			tmpData.word = FoodData.word;
			tmpData.SpawnIndex = FoodCourtIdx;
			
			Dropper->SetFoodSpawnData(tmpData);
			Dropper->SetSpawnClass( LoadClass<AActor>(nullptr, TEXT("/Game/CustomContents/Blueprints/Interactables/BP_Food.BP_Food_C")));
			Dropper->RequestSpawn();
		}
	}
}

class ADropper* AOrderKiosk::FindDropperByIdx(int32 InIdx)
{
	TArray<AActor*> Droppers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADropper::StaticClass(), Droppers);

	for (auto Dropper : Droppers)
	{
		if (ADropper* Dpp = Cast<ADropper>(Dropper))
		{
			if (Dpp->DropperIndex == InIdx)
			{
				return Dpp;
			}
		}
	}
	return nullptr;
}

