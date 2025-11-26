// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "InteractableComponent.h"

#include "APlayerActor.h"
#include "GameLogging.h"
#include "luggage.h"
#include "Net/UnrealNetwork.h"
#include "Components/BoxComponent.h"
#include "UInteractionSystem.h"


UInteractableComponent::UInteractableComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	bIsPickedUp = false;
	bOriginalSimulatePhysics = false;
	OriginalCollisionType = ECollisionEnabled::NoCollision;

	// 생성자에서 생성해서 자동 복제
	DetectionRange = CreateDefaultSubobject<UBoxComponent>(TEXT("DetectionRange"));
	if (DetectionRange)
	{
		DetectionRange->SetBoxExtent(FVector(150.0f)); // DetectionDistance 기본값                                                                 
		DetectionRange->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		DetectionRange->SetCollisionResponseToAllChannels(ECR_Ignore);
		DetectionRange->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	}
	
	SetIsReplicatedByDefault(true);
}

void UInteractableComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		PRINTLOG( TEXT("UInteractableComponent::BeginPlay - GetOwner() returned nullptr!"));
		return;
	}

	this->InitDetectionRange();
}

void UInteractableComponent::InitDetectionRange()
{
	// 1. 컴포넌트 등록                                                                                                                                         
	if (!DetectionRange->IsRegistered())
	{
		DetectionRange->RegisterComponent();
	}

	AActor* Owner = GetOwner();
	
	// 2. Owner의 RootComponent에 부착                                                                                                                                         
	if (!DetectionRange->IsAttachedTo(Owner->GetRootComponent()))
	{
		DetectionRange->AttachToComponent(
			Owner->GetRootComponent(),
			FAttachmentTransformRules::KeepRelativeTransform
		);
	}
	
	// BoxExtent 설정 업데이트                                                                                                         
	FVector BoxExtent(DetectionDistance, DetectionDistance, DetectionDistance);
	DetectionRange->SetBoxExtent(BoxExtent);

	// Overlap 콜백 바인딩                                                                                                                     
	DetectionRange->OnComponentBeginOverlap.AddDynamic(this, &UInteractableComponent::OnDetectionBeginOverlap);
	DetectionRange->OnComponentEndOverlap.AddDynamic(this, &UInteractableComponent::OnDetectionEndOverlap);
}

void UInteractableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 디버그 표시
	if (bShowDetectionDebug && DetectionRange)
	{
		AActor* Owner = GetOwner();
		if (Owner && IsValid(Owner))
		{
			FVector Center = Owner->GetActorLocation();
			FVector BoxExtent = DetectionRange->GetScaledBoxExtent();
			FQuat Rotation = Owner->GetActorQuat();

			// DetectionRange 박스 그리기
			DrawDebugBox(
				GetWorld(),
				Center,
				BoxExtent,
				Rotation,
				bCanInteract ? FColor::Green : FColor::Yellow,
				false,
				0.0f,
				0,
				2.0f
			);

			// 거리 텍스트 표시
			FString DebugText = FString::Printf(
				TEXT("Detection: %.0f cm\n%s"),
				DetectionDistance,
				*InteractionPrompt
			);

			DrawDebugString(
				GetWorld(),
				Center + FVector(0, 0, BoxExtent.Z + 20.f),
				DebugText,
				nullptr,
				FColor::White,
				0.0f,
				true,
				1.0f
			);
		}
	}
}

void UInteractableComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInteractableComponent, HoldingOwner);
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

void UInteractableComponent::PickUp(AActor* NewHoldingOwner)
{
	if (!NewHoldingOwner)
	{
		PRINTLOG( TEXT("InteractableComponent::PickUp - NewHoldingOwner is null"));
		return;
	}

	Server_PickUp(NewHoldingOwner);
}

void UInteractableComponent::Server_PickUp_Implementation(AActor* NewHoldingOwner)
{
	if (bIsPickedUp) return;

	HoldingOwner = NewHoldingOwner;
	if (!HoldingOwner)
	{
		PRINTLOG( TEXT("InteractableComponent::PickUp - HoldingOwner is null"));
		return;
	}
	
	// Owner actor의 PrimitiveComponent 찾기
	UPrimitiveComponent* PrimComp = GetOwnerPrimitiveComponent();
	if (!PrimComp)
		return;

	// 원래 상태 저장
	// Drop 시 원래 상태로 되돌리기 위함
	bOriginalSimulatePhysics = PrimComp->IsSimulatingPhysics();
	OriginalCollisionType = PrimComp->GetCollisionEnabled();

	// pick up
	PrimComp->SetSimulatePhysics(false);
	
	OnRep_HoldingOwner();

	bIsPickedUp = true;
	PRINTLOG( TEXT("InteractableComponent::PickUp - %s picked up"), *GetOwner()->GetName());
}

void UInteractableComponent::Drop()
{
	Server_Drop();
}

void UInteractableComponent::Server_Drop_Implementation()
{
	if (!bIsPickedUp) return;

	// detach
	GetOwner()->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	UPrimitiveComponent* PrimComp = GetOwnerPrimitiveComponent();
	if (PrimComp)
	{
		PrimComp->SetSimulatePhysics(bOriginalSimulatePhysics);
		PrimComp->SetCollisionEnabled(OriginalCollisionType);
	}

	bIsPickedUp = false;

	PRINTLOG( TEXT("InteractableComponent::Drop - %s dropped"), *GetOwner()->GetName());
}


UPrimitiveComponent* UInteractableComponent::GetOwnerPrimitiveComponent() const
{
	AActor* Owner = GetOwner();
	if (!Owner)
		return nullptr;

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

void UInteractableComponent::ShowDebugInfo(AActor* ViewerActor)
{
	if (!GetOwner() || !ViewerActor) return;

	// 객체 위치 (약간 위쪽에 표시)
	FVector DebugLocation = GetOwner()->GetActorLocation() + FVector(0.f, 0.f, 100.f);
	
	// InteractionPrompt 사용
	FString DebugMessage = InteractionPrompt;
	
	// 3D 공간에 디버그 문자열 표시
	DrawDebugString(
		GetWorld(),
		DebugLocation,
		DebugMessage,
		nullptr,
		bIsPickedUp ? FColor::Yellow : FColor::Green,
		0.0f,
		true,
		1.2f
	);
}

void UInteractableComponent::TriggerInteraction(AActor* Interactor)
{
	if (!bCanInteract || !Interactor)
		return;

	// 델리게이트 브로드캐스트
	OnInteractionTriggered.Broadcast(Interactor);

	PRINTLOG( TEXT("InteractableComponent::TriggerInteraction - %s triggered by %s"), *GetOwner()->GetName(), *Interactor->GetName());
}

void UInteractableComponent::OnDetectionBeginOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor)
		return;

	// Player 캐릭터인지 확인
	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (Character && Character->IsPlayerControlled())
	{
		// InteractionSystem 찾기 및 등록
		UInteractionSystem* InteractionSystem = Character->FindComponentByClass<UInteractionSystem>();
		if (InteractionSystem)
		{
			InteractionSystem->RegisterInteractable(this);
		}

		// Outline 켜기
		Aluggage* luggage = Cast<Aluggage>(GetOwner());
		if (luggage && !bIsPickedUp)
		{
			luggage->OutlineOn();
		}
		
		PRINTLOG( TEXT("InteractableComponent: Player entered detection range - %s"), *GetOwner()->GetName());
	}
}

void UInteractableComponent::OnDetectionEndOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor)
		return;

	// Player 캐릭터인지 확인
	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (Character && Character->IsPlayerControlled())
	{
		// InteractionSystem에서 등록 해제
		UInteractionSystem* InteractionSystem = Character->FindComponentByClass<UInteractionSystem>();
		if (InteractionSystem)
		{
			InteractionSystem->UnregisterInteractable(this);
		}

		PRINTLOG( TEXT("InteractableComponent: Player left detection range - %s"), *GetOwner()->GetName());

		// Outline 끄기
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this]
		{
			Aluggage* luggage = Cast<Aluggage>(GetOwner());
			if (luggage && !bIsPickedUp)
			{
				luggage->OutlineOff();
			}
			
		}), 0.1f, false);
	}
}