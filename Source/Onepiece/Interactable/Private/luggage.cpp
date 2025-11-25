// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "luggage.h"
#include "InteractableComponent.h"
#include "ALingoGameMode.h"
#include "ALingoPlayerState.h"
#include "GameLogging.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

Aluggage::Aluggage()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

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

	PRINTLOG(TEXT("[Luggage] OnInteract - Player selected luggage with Symbol: %s, Color: %s"), *Symbol, *Color);

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