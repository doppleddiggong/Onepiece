// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "Food.h"

#include "CityNameWidget.h"
#include "InteractableComponent.h"
#include "Components/WidgetComponent.h"


// Sets default values
AFood::AFood()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	FoodMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FoodMesh"));
	FoodMesh->SetupAttachment(GetRootComponent());

	FoodName = CreateDefaultSubobject<UWidgetComponent>(TEXT("FoodName"));
	FoodName->SetupAttachment(GetRootComponent());

	InteractableComp = CreateDefaultSubobject<UInteractableComponent>(TEXT("Interactable"));
	InteractableComp->InteractionType = EInteractionType::PickUp;
	InteractableComp->InteractionPrompt = TEXT("Pick Up");
	
	// Initial settings
	Mesh->SetSimulatePhysics(true);
	Mesh->SetEnableGravity(true);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetCollisionProfileName(TEXT("PhysicsActor"));

	// 무게 설정
	Mesh->SetMassOverrideInKg(NAME_None, 50.f, true);

	// 물리 복제 설정
	Mesh->SetIsReplicated(true);

	// Replication
	bReplicates = true;
	SetNetUpdateFrequency(100.0f);		// 높은 업데이트 빈도로 부드러운 네트워크 동기화
	SetMinNetUpdateFrequency(33.0f);	// 최소 30fps 업데이트 보장
	
}

// Called when the game starts or when spawned
void AFood::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFood::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
}

void AFood::SetFoodInfo(int32 InIndex, FString InName)
{
	Name = InName;
	Index = InIndex;

	UCityNameWidget* NameWidget = Cast<UCityNameWidget>(FoodName->GetWidget());
	if (NameWidget)
	{
		NameWidget->SetCityName(InName);
	}
}



// HACK, 해줘요. 모델링 찾았고, ListenData에서 모델링도 빼오는데, AddChildActor 까지도 되는데.
// 객체 생성쪽에서 뭔가 이상한데. 디버깅을 다 못했음
// void AFood::SetFoodInfo(int32 InIndex, FString InName)
// {
// 	Name = InName;
// 	Index = InIndex;
//
// 	UCityNameWidget* NameWidget = Cast<UCityNameWidget>(FoodName->GetWidget());
// 	if (NameWidget)
// 	{
// 		NameWidget->SetCityName(InName);
// 	}
// }

// void AFood::SetFoodInfo(const FFoodData& InFoodData)
// {
// 	Name = FString::Printf(TEXT("%s / %s"), *InFoodData.word1.name, *InFoodData.word2.name);
// 	Index = InFoodData.SpawnIndex;
//
// 	UCityNameWidget* NameWidget = Cast<UCityNameWidget>(FoodName->GetWidget());
// 	if (NameWidget)
// 	{
// 		NameWidget->SetCityName(Name);
// 	}
//
// 	int32 PatternIdx = FCString::Atoi(*InFoodData.word2.code);
// 	FListenData Listen02Data;
// 	if (UGameDataManager::Get(GetWorld())->GetListenData(PatternIdx, Listen02Data))
// 	{
// 		SetFoodModel(Listen02Data.FoodPath);
// 	}
// }