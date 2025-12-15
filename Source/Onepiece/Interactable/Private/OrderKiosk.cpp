// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "OrderKiosk.h"

#include "ConveyorBelt.h"
#include "Food.h"
#include "ListenAnswer.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

#define TARGET_LOCATION FVector(5604.438473,-5691.762227,-3648.742048)

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

// Called every frame
void AOrderKiosk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AOrderKiosk::BeginFoodOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsOnceStopped) return;
	
	if (AFood* Temp = Cast<AFood>(OtherActor))
	{
		CurrentFoodContainer = Temp;
		
		if (ConveyorsToControl.Num() <= 0) return;
		
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
	UE_LOG(LogTemp, Warning, TEXT("[OrderKiosk::BeginSubmitOverlap] Called with Actor: %s"),
		OtherActor ? *OtherActor->GetName() : TEXT("NULL"));

	// 답 제출하면 컨베이어 재개
	if (AListenAnswer* Answer = Cast<AListenAnswer>(OtherActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("[OrderKiosk::BeginSubmitOverlap] ListenAnswer detected. AnswerType: %d, Index: %d"),
			Answer->AnswerData.AnswerType, InAnswerType);

		if (ConveyorsToControl.Num() <= 0)
		{
			UE_LOG(LogTemp, Error, TEXT("[OrderKiosk::BeginSubmitOverlap] ConveyorsToControl is empty!"));
			return;
		}

		for (auto Conveyor : ConveyorsToControl)
		{
			if (AConveyorBelt* CB = Cast<AConveyorBelt>(Conveyor))
			{
				if (!CurrentFoodContainer) return;

				// 인덱스 번호와 정답 타입이 일치하면 데이터 전달 후 컨베이어 움직이기
				if (Answer->AnswerData.AnswerType == InAnswerType)
				{
					switch (InAnswerType)
					{
					case EAnswerType::Food:
						CurrentFoodContainer->SetFoodMesh(Answer->AnswerData.word1, Answer->Mesh->GetStaticMesh());
						break;

					case EAnswerType::City:
						{
							CurrentFoodContainer->SetCityName(Answer->AnswerData.word1);

							FTimerHandle TimerHandle;
							GetWorldTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this]
							{

								Server_MoveFoodContainer(CurrentFoodContainer);
								
							}), 3.f, false);
							
							break;
						}

					default:
						break;
					}

					CB->ChangeConveyorSpeed(200.f);

					Server_DestroyListenAnswer(Answer);
					//CurrentFoodContainer = nullptr;
				}
			}
		}

		IsOnceStopped = true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[OrderKiosk::BeginSubmitOverlap] OtherActor is not a ListenAnswer"));
	}
}

void AOrderKiosk::Server_MoveFoodContainer_Implementation(AActor* ActorToMove)
{
	ActorToMove->SetActorLocation(TARGET_LOCATION);
}

void AOrderKiosk::Server_DestroyListenAnswer_Implementation(AActor* ActorToDestroy)
{
	ActorToDestroy->Destroy();
}



