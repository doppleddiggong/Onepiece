// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "OrderKiosk.h"

#include "ADropper.h"
#include "ALingoGameState.h"
#include "APlayerActor.h"
#include "ConveyorBelt.h"
#include "Food.h"
#include "InteractableComponent.h"
#include "ListenAnswer.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


// Sets default values
AOrderKiosk::AOrderKiosk()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	
	FoodCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("FoodCollision"));
	SetRootComponent(FoodCollision);

	SubmitCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("SubmitCollision"));
	SubmitCollision->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AOrderKiosk::BeginPlay()
{
	Super::BeginPlay();

	FoodCollision->OnComponentBeginOverlap.AddDynamic(this, &AOrderKiosk::BeginFoodOverlap);
	SubmitCollision->OnComponentBeginOverlap.AddDynamic(this, &AOrderKiosk::BeginSubmitOverlap);
}

void AOrderKiosk::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// DOREPLIFETIME(AOrderKiosk, IsOverlapping);
	// DOREPLIFETIME(AOrderKiosk, FoodData);
}

// Called every frame
void AOrderKiosk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AOrderKiosk::BeginFoodOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsOnceStopped) return;
	
	if (AFood* FoodContainer = Cast<AFood>(OtherActor))
	{
		if (ConveyorsToControl.Num() <= 0) return;

		UE_LOG(LogTemp, Warning, TEXT("FoodOverlap"));
		
		// 만약 음식 큐브일 경우, 컨베이어 멈추기
		for (auto Conveyor : ConveyorsToControl)
		{
			if (AConveyorBelt* CB = Cast<AConveyorBelt>(Conveyor))
			{
				CB->ChangeConveyorSpeed(0.f);
			}
		}
	}
}

void AOrderKiosk::BeginSubmitOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 답 제출하면 컨베이어 재개
	if (AListenAnswer* Answer = Cast<AListenAnswer>(OtherActor))
	{
		if (ConveyorsToControl.Num() <= 0) return;

		UE_LOG(LogTemp, Warning, TEXT("SubmitOverlap"));
		
		// 만약 음식 큐브일 경우, 컨베이어 멈추기
		for (auto Conveyor : ConveyorsToControl)
		{
			if (AConveyorBelt* CB = Cast<AConveyorBelt>(Conveyor))
			{
				CB->ChangeConveyorSpeed(200.f);
			}
		}

		IsOnceStopped = true;
	}
}

// void AOrderKiosk::OnInteractionTriggered(AActor* Interactor)
// {
// 	if (bIsUsed) return;
//
// 	// 사용됨 표시
// 	bIsUsed = true;
// 	
// 	// InteractableComp 비활성화 (재사용 방지)
// 	/*if (InteractableComp)
// 	{
// 		InteractableComp->bCanInteract = false;
// 	}
// 	*/
// 	ALingoGameState* GS = Cast<ALingoGameState>(GetWorld()->GetGameState());
// 	if (GS)
// 	{
// 		// Food 스폰
// 		ADropper* Dropper = FindDropperByIdx(FoodCourtIdx);
// 		if (Dropper)
// 		{
// 			FFoodData tmpData;
// 			tmpData.word1 = FoodData.word1;
// 			tmpData.word2 = FoodData.word2;
// 			tmpData.SpawnIndex = FoodCourtIdx;
// 			
// 			Dropper->SetFoodSpawnData(tmpData);
// 			Dropper->SetSpawnClass( LoadClass<AActor>(nullptr, TEXT("/Game/CustomContents/Blueprints/Interactables/BP_Food.BP_Food_C")));
// 			Dropper->RequestSpawn();
// 		}
// 	}
// }
//
// void AOrderKiosk::OnRep_FoodData()
// {
// 	UpdateInteractableWidget(FoodData.word2.name);
// }
//
// void AOrderKiosk::UpdateInteractableWidget(FString NewString)
// {
// 	FString PromptText = FString::Printf(TEXT("Order %s"), *NewString);
// 	InteractableComp->UpdateInteractPrompt(PromptText);
// }
//
// class ADropper* AOrderKiosk::FindDropperByIdx(int32 InIdx)
// {
// 	TArray<AActor*> Droppers;
// 	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADropper::StaticClass(), Droppers);
//
// 	for (auto Dropper : Droppers)
// 	{
// 		if (ADropper* Dpp = Cast<ADropper>(Dropper))
// 		{
// 			if (Dpp->DropperIndex == InIdx)
// 			{
// 				return Dpp;
// 			}
// 		}
// 	}
// 	return nullptr;
// }

