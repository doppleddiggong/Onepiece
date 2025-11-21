// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "Onepiece/Interactable/Public/InteractableComponent.h"

#include "APlayerActor.h"
#include "Net/UnrealNetwork.h"


UInteractableComponent::UInteractableComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	bIsPickedUp = false;
	bOriginalSimulatePhysics = false;
	OriginalCollisionType = ECollisionEnabled::NoCollision;

	SetIsReplicatedByDefault(true);
}


// Called when the game starts
void UInteractableComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UInteractableComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInteractableComponent, HoldingOwner);
}

// Called every frame
void UInteractableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UInteractableComponent::OnRep_HoldingOwner()
{
	if (HoldingOwner)
	{
		APlayerActor* MyPlayer = Cast<APlayerActor>(HoldingOwner);
		if (MyPlayer)
		{
			// 여기서 GetOwner() == 컴포넌트가 붙어있는 액터
			GetOwner()->AttachToComponent(MyPlayer->HoldPosition, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		}
	}
	else
	{
		GetOwner()->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	}
}

void UInteractableComponent::PickUp()
{
	GetOwner()->SetOwner(HoldingOwner);
	// 클라에서 이게 실행이 안되는데?
	Server_PickUp();
}

void UInteractableComponent::Server_PickUp_Implementation()
{
	if (bIsPickedUp) return;
	
	// Owner actor의 PrimitiveComponent 찾기
	UPrimitiveComponent* PrimComp = GetOwnerPrimitiveComponent();
	if (!PrimComp) return;

	// 원래 상태 저장
	// Drop 시 원래 상태로 되돌리기 위함
	bOriginalSimulatePhysics = PrimComp->IsSimulatingPhysics();
	OriginalCollisionType = PrimComp->GetCollisionEnabled();

	// pick up
	PrimComp->SetSimulatePhysics(false);
	
	if (HoldingOwner->HasAuthority())
	{
		OnRep_HoldingOwner();
	}

	bIsPickedUp = true;
	UE_LOG(LogTemp, Log, TEXT("InteractableComponent::PickUp - %s picked up"), *GetOwner()->GetName());
}

void UInteractableComponent::Drop()
{
	Server_Drop();

	GetOwner()->SetOwner(nullptr);
}

void UInteractableComponent::Server_Drop_Implementation()
{
	if (!bIsPickedUp) return;

	// detach
	if (HoldingOwner->HasAuthority())
	{
		GetOwner()->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	}

	UPrimitiveComponent* PrimComp = GetOwnerPrimitiveComponent();
	if (PrimComp)
	{
		PrimComp->SetSimulatePhysics(bOriginalSimulatePhysics);
		PrimComp->SetCollisionEnabled(OriginalCollisionType);
	}

	bIsPickedUp = false;

	UE_LOG(LogTemp, Log, TEXT("InteractableComponent::Drop - %s dropped"), *GetOwner()->GetName());
}

UPrimitiveComponent* UInteractableComponent::GetOwnerPrimitiveComponent() const
{
	AActor* Owner = GetOwner();
	if (!Owner) return nullptr;

	UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(Owner->GetRootComponent());

	// RootComponent가 PrimitiveComponent가 아니면, 자식 중에서 찾기
	if (!PrimComp)
	{
		TArray<UActorComponent*> Components;
		Owner->GetComponents(UPrimitiveComponent::StaticClass(), Components);

		if (Components.Num() > 0)
		{
			// 첫 번쩨 primitive component 반환
			PrimComp = Cast<UPrimitiveComponent>(Components[0]);
		}
	}

	return PrimComp;
}

