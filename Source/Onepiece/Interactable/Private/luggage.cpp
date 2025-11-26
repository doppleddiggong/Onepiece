// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "luggage.h"
#include "InteractableComponent.h"
#include "ALingoGameMode.h"
#include "ALingoPlayerState.h"
#include "GameLogging.h"
#include "UGameDataManager.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

Aluggage::Aluggage()
{
	PrimaryActorTick.bCanEverTick = true;


	Mesh1Comp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh1Comp"));
	SetRootComponent(Mesh1Comp);
	
	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	BoxComp->SetupAttachment(GetRootComponent());
	BoxComp->SetBoxExtent(FVector(55));
	
	Mesh2Comp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh2Comp"));
	Mesh2Comp->SetupAttachment(Mesh1Comp);
	
	Mesh3Comp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh3Comp"));
	Mesh3Comp->SetupAttachment(Mesh1Comp);

	InteractableComp = CreateDefaultSubobject<UInteractableComponent>(TEXT("Interactable"));
	InteractableComp->InteractionType = EInteractionType::PickUp;
	InteractableComp->InteractionPrompt = TEXT("Press E to Grap");
	
	// Initial settings
	Mesh1Comp->SetSimulatePhysics(true);
	Mesh1Comp->SetEnableGravity(true);
	Mesh1Comp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh1Comp->SetCollisionProfileName(TEXT("PhysicsActor"));

	// 무게 설정
	Mesh1Comp->SetMassOverrideInKg(NAME_None, 5.f, true);

	// Replication
	bReplicates = true;

}

void Aluggage::BeginPlay()
{
	Super::BeginPlay();

	SetReplicateMovement(true);
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

		UMaterialInterface* OriginalMaterial = Mesh3Comp->GetMaterial(0);
		if (OriginalMaterial)
		{
			UMaterialInstanceDynamic* NewMaterial = UMaterialInstanceDynamic::Create(OriginalMaterial, this);
			if (NewMaterial && Mesh3Comp)
			{
				// BaseColorFactor로 변경!                                                                                                                                                                          
				NewMaterial->SetVectorParameterValue(FName("BaseColorFactor"), Color);
				Mesh3Comp->SetMaterial(0, NewMaterial);
			}
		}
	}
}

void Aluggage::ApplyPatternToMesh(FString InPattern)
{
	PatternName = InPattern;
}


//--------------------------------------------------------------//
// Read Quest Interaction
//--------------------------------------------------------------//

void Aluggage::OnInteract(AActor* Interactor)
{
	if (!Interactor)
	{
		PRINTLOG(TEXT("[Luggage] OnInteract - Interactor is null"));
		return;
	}

	// 플레이어 컨트롤러와 PlayerState 가져오기
	APlayerController* PC = Cast<APlayerController>(Interactor->GetInstigatorController());
	if (!PC)
	{
		PRINTLOG(TEXT("[Luggage] OnInteract - PlayerController is null"));
		return;
	}

	APlayerState* PS = PC->GetPlayerState<APlayerState>();
	if (!PS)
	{
		PRINTLOG(TEXT("[Luggage] OnInteract - PlayerState is null"));
		return;
	}

	PRINTLOG(TEXT("[Luggage] OnInteract - Player selected luggage with Target1: %s, Target2: %s"), *Target1, *Target2);

	// 서버에 선택 알림
	ServerNotifySelection(PS);
}

void Aluggage::ServerNotifySelection_Implementation(APlayerState* Player)
{
	if (!Player)
	{
		PRINTLOG(TEXT("[Luggage] ServerNotifySelection - Player is null"));
		return;
	}

	PRINTLOG(TEXT("[Luggage] ServerNotifySelection - Processing selection for player"));

	// GameMode에 캐리어 선택 알림
	ALingoGameMode* GameMode = GetWorld()->GetAuthGameMode<ALingoGameMode>();
	if (GameMode)
	{
		GameMode->HandleCarrierSelection(Player, this);
	}
	else
	{
		PRINTLOG(TEXT("[Luggage] ServerNotifySelection - GameMode is null or not ALingoGameMode"));
	}
}

bool Aluggage::ServerNotifySelection_Validate(APlayerState* Player)
{
	// 기본적인 검증: Player가 유효한지 확인
	return Player != nullptr;
}
