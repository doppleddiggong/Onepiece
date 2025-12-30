// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "OrderKiosk.h"

#include "ATeleportOut.h"
#include "CityName.h"
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

	MarkerType = ECompassMarkerType::QuestEnd;
}

// Called when the game starts or when spawned
void AOrderKiosk::BeginPlay()
{
	Super::BeginPlay();

	FoodCollision->OnComponentBeginOverlap.AddDynamic(this, &AOrderKiosk::BeginFoodOverlap);
	SubmitCollision->OnComponentBeginOverlap.AddDynamic(this, &AOrderKiosk::BeginSubmitOverlap);

	TArray<AActor*> CityNames;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACityName::StaticClass(), CityNames);
	for (AActor* Actor : CityNames)
	{
		if (ACityName* CN = Cast<ACityName>(Actor))
		{
			if (CN->Index == 0) FoodDisplay = CN;
			else if (CN->Index == 1) CityDisplay = CN;
		}
	}
}

// Called every frame
void AOrderKiosk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AOrderKiosk::BeginFoodOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("[OrderKiosk::BeginFoodOverlap] IsOnceStopped: %d"), IsOnceStopped);

	if (IsOnceStopped) return;

	if (AFood* Temp = Cast<AFood>(OtherActor))
	{
		CurrentFoodContainer = Temp;
		FFoodCapsuleData CurrentData = CurrentFoodContainer->CurrentFoodData;

		UE_LOG(LogTemp, Warning, TEXT("[OrderKiosk::BeginFoodOverlap] Food detected. InAnswerType: %d, City=%s, Food=%s"),
			InAnswerType, *CurrentData.word1.name, *CurrentData.word2.name);

		// 이미 정답 데이터가 있으면 컨베이어 계속 움직임
		if (InAnswerType == EAnswerType::Food && CurrentData.word2.name != TEXT("")) return;

		// City가 이미 있는 경우 (부분정답), 바로 3초 후 이동 타이머 설정
		if (InAnswerType == EAnswerType::City && CurrentData.word1.name != TEXT(""))
		{
			UE_LOG(LogTemp, Warning, TEXT("[OrderKiosk::BeginFoodOverlap] City already set - Setting timer for teleport"));

			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this]
			{
				UE_LOG(LogTemp, Warning, TEXT("[OrderKiosk] Timer triggered - Moving FoodContainer (from BeginFoodOverlap)"));
				Server_MoveFoodContainer(CurrentFoodContainer);
			}), 3.f, false);

			return;
		}

		// 아직 설정 안 됐으면 컨베이어 멈추기
		if (ConveyorsToControl.Num() <= 0) return;

		UE_LOG(LogTemp, Warning, TEXT("[OrderKiosk::BeginFoodOverlap] Stopping conveyor"));

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
		UE_LOG(LogTemp, Warning, TEXT("[OrderKiosk::BeginSubmitOverlap] ListenAnswer detected. AnswerType: %d, Index: %d"),
			Answer->AnswerData.AnswerType, InAnswerType);

		if (ConveyorsToControl.Num() <= 0) return;

		for (auto Conveyor : ConveyorsToControl)
		{
			if (AConveyorBelt* CB = Cast<AConveyorBelt>(Conveyor))
			{
				if (!CurrentFoodContainer) return;

				// 인덱스 번호와 정답 타입이 일치하면 데이터 전달 후 컨베이어 움직이기
				if (Answer->AnswerData.AnswerType == InAnswerType)
				{
					UE_LOG(LogTemp, Warning, TEXT("[OrderKiosk::BeginSubmitOverlap] AnswerType matched. InAnswerType: %d"), InAnswerType);

					switch (InAnswerType)
					{
					case EAnswerType::Food:
						CurrentFoodContainer->SetFoodMesh(Answer->AnswerData.word1, Answer->Mesh->GetStaticMesh());
						CurrentFoodContainer->UpdateFoodWidget();
						FoodDisplay->SetChecked();
						break;

					case EAnswerType::City:
						{
							UE_LOG(LogTemp, Warning, TEXT("[OrderKiosk::BeginSubmitOverlap] City case - Setting timer for teleport"));

							CurrentFoodContainer->SetCityName(Answer->AnswerData.word1);
							CityDisplay->SetChecked();

							// 3초 뒤 정답 구간으로 이동
							FTimerHandle TimerHandle;
							GetWorldTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this]
							{
								UE_LOG(LogTemp, Warning, TEXT("[OrderKiosk] Timer triggered - Moving FoodContainer"));
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

void AOrderKiosk::SetCompassMarkerInto(ECompassMarkerType InMarkerType)
{
	MarkerType = InMarkerType;
}

void AOrderKiosk::Server_MoveFoodContainer_Implementation(AActor* ActorToMove)
{
	ActorToMove->SetActorLocation(TeleportOut->GetActorLocation());
}

void AOrderKiosk::Server_DestroyListenAnswer_Implementation(AActor* ActorToDestroy)
{
	ActorToDestroy->Destroy();
}



