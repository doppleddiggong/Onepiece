// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "luggage.h"
#include "InteractableComponent.h"
#include "UGameDataManager.h"
#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"

Aluggage::Aluggage()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	Mesh1Comp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh1Comp"));
	Mesh1Comp->SetupAttachment(GetRootComponent());

	InteractableComp = CreateDefaultSubobject<UInteractableComponent>(TEXT("Interactable"));
	InteractableComp->InteractionType = EInteractionType::PickUp;
	InteractableComp->InteractionPrompt = TEXT("Press E to Grap");
	
	// Initial settings
	Mesh->SetSimulatePhysics(true);
	Mesh->SetEnableGravity(true);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetCollisionProfileName(TEXT("PhysicsActor"));

	// 무게 설정
	Mesh->SetMassOverrideInKg(NAME_None, 5.f, true);

	// Replication
	bReplicates = true;
	SetReplicateMovement(true);
}

void Aluggage::BeginPlay()
{
	Super::BeginPlay();

}

void Aluggage::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Pattern 이름을 luggage 위에 표시
	if (!PatternName.IsEmpty())
	{
		FVector TextLocation = GetActorLocation() + FVector(0, 0, 100);
		DrawDebugString(GetWorld(), TextLocation, PatternName, nullptr, FColor::White, 0.f, true);
	}
}

void Aluggage::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(Aluggage, ColorIndex);
}

void Aluggage::OnRep_ColorIndex()
{
	ApplyColorToMesh(ColorIndex);
}

void Aluggage::ApplyColorToMesh(int32 InColorIdx)
{
	ColorIndex = InColorIdx;

	FColorData ColorData;
	if (UGameDataManager::Get(GetWorld())->GetColorData(InColorIdx, ColorData))
	{
		FLinearColor Color = ColorData.GetLinearColor();

		UMaterialInterface* OriginalMaterial = Mesh->GetMaterial(0);
		if (OriginalMaterial)
		{
			UMaterialInstanceDynamic* NewMaterial = UMaterialInstanceDynamic::Create(OriginalMaterial, this);
			if (NewMaterial && Mesh1Comp)
			{
				// BaseColorFactor로 변경!                                                                                                                                                                          
				NewMaterial->SetVectorParameterValue(FName("BaseColorFactor"), Color);
				Mesh1Comp->SetMaterial(0, NewMaterial);
			}
		}
	}
}

void Aluggage::ApplyPatternToMesh(FString InPattern)
{
	PatternName = InPattern;
}
