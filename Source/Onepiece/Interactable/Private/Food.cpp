// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "Food.h"

#include "CityNameWidget.h"
#include "FListenData.h"
#include "InteractableComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"


// Sets default values
AFood::AFood()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	FoodMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FoodMesh"));
	FoodMesh->SetupAttachment(GetRootComponent());

	CityName = CreateDefaultSubobject<UWidgetComponent>(TEXT("FoodName"));

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
	
	CityName->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	CityName->SetRelativeLocation(FVector(0, 0, 0));
}

void AFood::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// FoodMesh는 Component이므로 자동 복제됨
	DOREPLIFETIME(AFood, FoodMesh);
	DOREPLIFETIME(AFood, CurrentFoodData);
}

// Called every frame
void AFood::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void AFood::SetCityName(FWordInfo InWord)
{
	CurrentFoodData.word2 = InWord;
	// 서버에서도 Widget 업데이트 (클라이언트는 OnRep_FoodName에서 호출됨)
	if (HasAuthority())
	{
		UpdateFoodWidget();
	}
}

void AFood::OnRep_CurrentFoodData()
{
	UpdateMesh();
	UpdateFoodWidget();
}

void AFood::UpdateFoodWidget()
{
	// Widget이 아직 초기화되지 않았을 수 있으므로 체크
	if (!CityName || !CityName->GetWidget())
		return;

	UCityNameWidget* NameWidget = Cast<UCityNameWidget>(CityName->GetWidget());
	if (NameWidget)
	{
		NameWidget->SetCityName(CurrentFoodData.word2.name);
	}
}

void AFood::UpdateMesh()
{
	if (!ListenDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("[AFood::UpdateMesh] ListenDataTable is null!"));
		return;
	}

	if (!FoodMesh)
	{
		UE_LOG(LogTemp, Error, TEXT("[AFood::UpdateMesh] FoodMesh component is null!"));
		return;
	}

	TArray<FListenData*> AllRows;
	ListenDataTable->GetAllRows<FListenData>(TEXT("UpdateMesh"), AllRows);

	for (FListenData* Row : AllRows)
	{
		if (Row && Row->Word == CurrentFoodData.word1.name)
		{
			if (Row->FoodPath)
			{
				FoodMesh->SetStaticMesh(Row->FoodPath);
				UE_LOG(LogTemp, Warning, TEXT("[AFood::UpdateMesh] Mesh updated to: %s for word: %s"),
					*Row->FoodPath->GetName(), *CurrentFoodData.word1.name);
				return;
			}
		}
	}

	// 일치하는 데이터를 찾지 못함
	UE_LOG(LogTemp, Warning, TEXT("[AFood::UpdateMesh] No matching ListenData found for: %s"),
		*CurrentFoodData.word1.name);
}

void AFood::SetFoodMesh(FWordInfo InWord, UStaticMesh* InMesh)
{
	UE_LOG(LogTemp, Warning, TEXT("[AFood::SetFoodMesh] Called - Word: %s, HasAuthority: %s"),
		*InWord.name, HasAuthority() ? TEXT("TRUE") : TEXT("FALSE"));

	CurrentFoodData.word1 = InWord;

	// 서버에서도 UpdateMesh 호출 (클라이언트는 OnRep_CurrentFoodData에서 호출됨)
	if (HasAuthority())
	{
		UpdateMesh();
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