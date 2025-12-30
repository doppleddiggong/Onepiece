// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "ListenAnswer.h"

#include "CityNameWidget.h"
#include "FListenData.h"
#include "InteractableComponent.h"
#include "UGameDataManager.h"
#include "UHookComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"


// Sets default values
AListenAnswer::AListenAnswer()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	NameWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("NameWidget"));
	NameWidgetComp->SetupAttachment(GetRootComponent());

	InteractableComp = CreateDefaultSubobject<UInteractableComponent>(TEXT("Interactable"));
	InteractableComp->InteractionType = EInteractionType::PickUp;
	InteractableComp->InteractionPrompt = TEXT("Pick Up");

	// Initial settings (Physics 설정은 BeginPlay에서 수행)
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetCollisionProfileName(TEXT("PhysicsActor"));

	HookComp = CreateDefaultSubobject<UHookComponent>(TEXT("Hook"));

	// 물리 복제 설정
	Mesh->SetIsReplicated(true);

	// Replication
	bReplicates = true;
	SetNetUpdateFrequency(100.0f);		// 높은 업데이트 빈도로 부드러운 네트워크 동기화
	SetMinNetUpdateFrequency(33.0f);	// 최소 30fps 업데이트 보장
}

// Called when the game starts or when spawned
void AListenAnswer::BeginPlay()
{
	Super::BeginPlay();

	// Physics 설정 (생성자에서 이동)
	if (Mesh)
	{
		Mesh->SetSimulatePhysics(true);
		Mesh->SetEnableGravity(true);
		Mesh->SetMassOverrideInKg(NAME_None, 50.f, true);
	}

	NameWidgetComp->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	NameWidgetComp->SetRelativeLocation(FVector(0, 0, 10));

	// 델리게이트 바인딩
	if (InteractableComp)
	{
		InteractableComp->OnOutlineStateChanged.AddDynamic(this, &AListenAnswer::OnOutlineStateChanged);
	}
}

void AListenAnswer::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AListenAnswer, AnswerData);
}

// Called every frame
void AListenAnswer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AListenAnswer::OnRep_AnswerData()
{
	UpdateMesh();
	UpdateNameWidget();
}

void AListenAnswer::UpdateMesh()
{
	// 게임 스레드가 아니면 게임 스레드로 dispatch
	if (!IsInGameThread())
	{
		AsyncTask(ENamedThreads::GameThread, [this]()
		{
			UpdateMesh();
		});
		return;
	}

	if (!ListenDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("ListenDataTable is null!"));
		return;
	}

	if (!ListenDataTable) return;

	TArray<FListenData*> AllRows;
	ListenDataTable->GetAllRows<FListenData>(TEXT("UpdateMesh"), AllRows);

	for (FListenData* Row : AllRows)
	{
		if (Row && Row->Word == AnswerData.word1.name)
		{
			if (Row->FoodPath)
			{
				Mesh->SetStaticMesh(Row->FoodPath);
				return;
			}
		}
	}

	// 일치하는 데이터를 찾지 못함
	UE_LOG(LogTemp, Warning, TEXT("No matching ListenData found for: %s"), *AnswerData.word1.name);
}

void AListenAnswer::UpdateNameWidget()
{
	// 게임 스레드가 아니면 게임 스레드로 dispatch
	if (!IsInGameThread())
	{
		AsyncTask(ENamedThreads::GameThread, [this]()
		{
			UpdateNameWidget();
		});
		return;
	}

	// Widget이 아직 초기화되지 않았을 수 있으므로 체크
	if (!NameWidgetComp || !NameWidgetComp->GetWidget())
		return;

	UCityNameWidget* NameWidget = Cast<UCityNameWidget>(NameWidgetComp->GetWidget());
	if (NameWidget)
	{
		NameWidget->SetCityName(AnswerData.word1.name);
	}
}

void AListenAnswer::OnOutlineStateChanged(bool bShouldShowOutline)
{
	if (Mesh)
	{
		Mesh->SetRenderCustomDepth(bShouldShowOutline);
	}
}

