// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "AHolder.h"
#include "luggage.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameLogging.h"
#include "InteractableComponent.h"
#include "Animation/AnimationAsset.h"

AHolder::AHolder()
{
	PrimaryActorTick.bCanEverTick = false;

	// Root component
	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	RootComponent = Root;

	// Mesh component
	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);

	// Box collision component
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxCollision->SetupAttachment(MeshComponent);
	BoxCollision->SetGenerateOverlapEvents(true);
	BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxCollision->SetCollisionResponseToAllChannels(ECR_Overlap);

	// HoldPos component
	HoldPos = CreateDefaultSubobject<USceneComponent>(TEXT("HoldPos"));
	HoldPos->SetupAttachment(MeshComponent);
}

void AHolder::BeginPlay()
{
	Super::BeginPlay();

	// Overlap 이벤트 바인딩
	if (BoxCollision)
		BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AHolder::OnBoxOverlapBegin);
}

void AHolder::OnBoxOverlapBegin(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!OtherActor)
		return;

	// Luggage인지 확인
	Aluggage* Luggage = Cast<Aluggage>(OtherActor);
	
	if (Luggage && !CurrentLuggage)
	{
		CurrentLuggage = Luggage;
		CheckLuggage(Luggage);
	}
}

void AHolder::CheckLuggage(Aluggage* Luggage)
{
	if (!Luggage)
		return;

	// 이미 Activate 되었으면 작동 안 함
	if (bIsActivated)
	{
		PRINTLOG(TEXT("AHolder::CheckLuggage - Already activated, ignoring"));
		return;
	}

	// ColorIdx와 PatternIdx 비교
	bool bColorMatch = (AnswerColorIdx == -1) || (Luggage->ColorIdx == AnswerColorIdx);
	bool bPatternMatch = (AnswerPatternIdx == -1) || (Luggage->PatternIdx == AnswerPatternIdx);
	bool bSuccess = bColorMatch && bPatternMatch;

	PRINTLOG(TEXT("AHolder::CheckLuggage - ColorIdx: %d (Answer: %d), PatternIdx: %d (Answer: %d), Result: %s"),
		Luggage->ColorIdx, AnswerColorIdx,
		Luggage->PatternIdx, AnswerPatternIdx,
		bSuccess ? TEXT("Success") : TEXT("Fail"));

	if (bSuccess)
	{
		// Success: Luggage를 HoldPos 위치에 Lock
		if (HoldPos)
		{
			Luggage->SetActorLocation(HoldPos->GetComponentLocation());
			Luggage->SetActorRotation(HoldPos->GetComponentRotation());
		}

		// Luggage의 물리 비활성화 및 충돌 비활성화
		if (Luggage->BoxComp)
		{
			Luggage->BoxComp->SetSimulatePhysics(false);
			Luggage->BoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}

		// Interactable 컴포넌트 비활성화
		if (Luggage->InteractableComp)
		{
			Luggage->InteractableComp->SetActive(false);
		}

		// Activate 상태로 전환
		bIsActivated = true;

		// 애니메이션 재생
		if (MeshComponent && AnimToPlay)
		{
			MeshComponent->PlayAnimation(AnimToPlay, false);
			PRINTLOG(TEXT("AHolder::CheckLuggage - Playing animation"));
		}

		// 블루프린트 이벤트 호출
		OnActivate(true);
	}
	else
	{
		// Fail: Luggage 제거
		Luggage->Destroy();
		CurrentLuggage = nullptr;

		// 블루프린트 이벤트 호출
		OnActivate(false);
	}
}

