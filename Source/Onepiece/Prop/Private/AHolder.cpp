// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "AHolder.h"
#include "luggage.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameLogging.h"
#include "InteractableComponent.h"
#include "UBroadcastManager.h"
#include "UHookComponent.h"
#include "Animation/AnimationAsset.h"
#include "Net/UnrealNetwork.h"

AHolder::AHolder()
{
	PrimaryActorTick.bCanEverTick = true;

	// Replication
	bReplicates = true;

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

	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AHolder::OnBoxOverlapBegin);

	// 머티리얼 파라미터 초기화 (비활성화 상태)
	if (MeshComponent && MeshComponent->GetNumMaterials() > 0)
	{
		UMaterialInstanceDynamic* DynamicMaterial = MeshComponent->CreateDynamicMaterialInstance(0);
		if (DynamicMaterial)
		{
			DynamicMaterial->SetScalarParameterValue(FName("Activate"), 0.0f);
		}
	}
}

void AHolder::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// CurrentLuggage가 유효하고 활성화된 상태라면 회전
	if (bIsActivated && CurTarget)
	{
		FRotator CurrentRotation = CurTarget->GetActorRotation();
		CurrentRotation.Yaw += RotationSpeed * DeltaTime;
		CurTarget->SetActorRotation(CurrentRotation);
	}
}

void AHolder::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHolder, bIsActivated);
	DOREPLIFETIME(AHolder, CurTarget);
}

void AHolder::SetAnswerData(const int32 InAnswerColorIdx, const int32 InAnswerPatternIdx)
{
	this->AnswerColorIdx = InAnswerColorIdx;
	this->AnswerPatternIdx = InAnswerPatternIdx;
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

	// 서버에서만 실행
	if (!HasAuthority())
		return;

	if ( bIsActivated )
	{
		// 이미 Activate 되었으면 작동 안 함
		return;
	}
	
	// Luggage인지 확인
	if (auto Luggage = Cast<Aluggage>(OtherActor))
	{
		bool bSuccess = CheckLuggage(Luggage);

		// 머티리얼 파라미터 설정 (활성화)
		UpdateActivateState(bSuccess);
		// 블루프린트 이벤트 호출
		OnActivate(bSuccess);

		if ( bSuccess )
			UBroadcastManager::Get(GetWorld())->SendTutorMessage( FText::FromString(TEXT("Perfect! You placed the block right. Let’s keep going!")));
		else
			UBroadcastManager::Get(GetWorld())->SendTutorMessage( FText::FromString(TEXT("Oops! That block doesn’t go there.")));
	}
}

bool AHolder::CheckLuggage(Aluggage* TargetLuggage)
{
	if (!TargetLuggage)
		return false;

	// ColorIdx와 PatternIdx 비교
	bool bColorMatch = (AnswerColorIdx == -1) || (TargetLuggage->ColorIdx == AnswerColorIdx);
	bool bPatternMatch = (AnswerPatternIdx == -1) || (TargetLuggage->PatternIdx == AnswerPatternIdx);
	bool bSuccess = bColorMatch && bPatternMatch;

	PRINTLOG(TEXT("ColorIdx: %d (Answer: %d), PatternIdx: %d (Answer: %d), Result: %s"),
		TargetLuggage->ColorIdx, AnswerColorIdx,
		TargetLuggage->PatternIdx, AnswerPatternIdx,
		bSuccess ? TEXT("Success") : TEXT("Fail"));

	if (bSuccess)
	{
		// Success: Luggage를 HoldPos 위치보다 살짝 위에 배치
		if (HoldPos)
		{
			FVector ActivatedLocation = HoldPos->GetComponentLocation();
			ActivatedLocation.Z += ActivatedHeightOffset;
			TargetLuggage->SetActorLocation(ActivatedLocation);
			TargetLuggage->SetActorRotation(HoldPos->GetComponentRotation());
		}

		// Luggage의 모든 충돌 비활성화 (pickup, hook 등 모든 상호작용 차단)
		TargetLuggage->SetAllCollision(false);

		// Activate 상태로 전환
		bIsActivated = true;
		CurTarget = TargetLuggage;
	}
	else
	{
		// Fail: Luggage 제거
		TargetLuggage->Destroy();
	}

	return bSuccess;
}

void AHolder::UpdateActivateState(bool State)
{
	// 머티리얼 파라미터 설정 (비활성화)
	if (MeshComponent && MeshComponent->GetNumMaterials() > 0)
	{
		UMaterialInstanceDynamic* DynamicMaterial = Cast<UMaterialInstanceDynamic>(MeshComponent->GetMaterial(0));
		if (!DynamicMaterial)
			DynamicMaterial = MeshComponent->CreateDynamicMaterialInstance(0);

		if (DynamicMaterial)
			DynamicMaterial->SetScalarParameterValue(FName("Activate"), State ? 1.0f : 0.0f);
	}
}

