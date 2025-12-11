// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "ConveyorBelt.h"

#include "ANPCBase.h"
#include "APlayerActor.h"
#include "Food.h"
#include "GameLogging.h"
#include "InteractableComponent.h"
#include "luggage.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"


// Sets default values
AConveyorBelt::AConveyorBelt()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	RootSceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComp"));
	SetRootComponent(RootSceneComp);
	
	BeltComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BeltComp"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> beltMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/LevelPrototyping/Meshes/SM_ChamferCube.SM_ChamferCube'"));
	if (beltMeshRef.Succeeded())
	{
		BeltComp->SetStaticMesh(beltMeshRef.Object);
		BeltComp->SetRelativeScale3D(FVector(1.5,5.5,0.05));
		BeltComp->SetupAttachment(GetRootComponent());
	}
	
	BeltBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BeltBoxComp"));
	BeltBoxComp->SetupAttachment(GetRootComponent());
	BeltBoxComp->SetBoxExtent(FVector(75,275,5));
	
	MoveDirArrowComp = CreateDefaultSubobject<UArrowComponent>(TEXT("MoveDirArrowComp"));
	MoveDirArrowComp->SetupAttachment(GetRootComponent());
	MoveDirArrowComp->SetRelativeRotation(FRotator(0, 90, 0));
	
	// Set Collision Preset
	BeltComp->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	BeltBoxComp->SetCollisionProfileName(TEXT("ConveyorBelt"));
}

// Called when the game starts or when spawned
void AConveyorBelt::BeginPlay()
{
	Super::BeginPlay();
	
}

/**
 * @brief 컨베이어 벨트 Tick
 * @details [문제] 기존에는 서버/클라이언트 구분 없이 이동 처리하여 물리 복제와 충돌
 *          [해결] 서버에서만 이동 처리, 클라이언트는 복제로 동기화
 */
void AConveyorBelt::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// [개선] 서버에서만 이동 처리
	if (!HasAuthority())
		return;

	MoveOverlappedSkeletals(MoveSpeed * DeltaTime);
	MoveOverlappedStatics(MoveSpeed * DeltaTime);
}

void AConveyorBelt::ChangeConveyorMovement()
{
	ServerRPC_ChangeConveyorMovement();
}

void AConveyorBelt::ServerRPC_ChangeConveyorMovement_Implementation()
{
	MulticastRPC_ChangeConveyorMovement();
}

void AConveyorBelt::MulticastRPC_ChangeConveyorMovement_Implementation()
{
	bIsForward = !bIsForward;
	PRINT_STRING(TEXT("%d"), bIsForward);
}

void AConveyorBelt::MoveOverlappedSkeletals(float deltaDistance)
{
	TArray<AActor*> overlappedActors;
	BeltBoxComp->GetOverlappingActors(overlappedActors);
	for (const auto& actor : overlappedActors)
	{
		if (Cast<APlayerActor>(actor) || Cast<ANPCBase>(actor))
		{
			FVector deltaLoc = MoveDirArrowComp->GetForwardVector() * deltaDistance;
			deltaLoc = bIsForward ? deltaLoc : -deltaLoc;
			actor->AddActorWorldOffset(deltaLoc);
		}
	}
}

/**
 * @brief 정적 오브젝트 (Luggage) 이동 처리
 * @details [문제 1] 기존에는 클라이언트/서버 모두 물리 이동 처리하여 충돌 발생
 *          [해결 1] 서버 전용 Tick으로 변경하여 물리 복제 충돌 제거
 *
 *          [문제 2] AddForce 방식은 매 프레임 누적되어 무한 가속됨
 *          [해결 2] SetPhysicsLinearVelocity로 일정 속도 유지
 *                  - 서버 전용 실행이므로 네트워크 충돌 없음
 *                  - Z축 속도는 유지하여 중력 영향 보존
 */
void AConveyorBelt::MoveOverlappedStatics(float deltaDistance)
{
	TArray<UPrimitiveComponent*> overlappedComponents;
	BeltBoxComp->GetOverlappingComponents(overlappedComponents);

	for (const auto& comp : overlappedComponents)
	{
		AActor* owner = comp->GetOwner();
		Aluggage* luggage = Cast<Aluggage>(owner);
		AFood* Food = Cast<AFood>(owner);

		if (!Food && !luggage) continue;  // 둘 다 아니면 스킵
		if (!Cast<UStaticMeshComponent>(comp)) continue;

		// [개선] Luggage의 경우 Hook이나 PickUp 중이면 컨베이어 이동 무시
		// bIsPickedUp은 이제 복제되므로 서버에서 정확한 상태 확인 가능
		if (luggage)
		{
			if (luggage->InteractableComp && luggage->InteractableComp->IsPickedUp())
			{
				continue;
			}

			// [개선] Hook 중이면 컨베이어 이동 무시
			if (luggage->bIsBeingHooked)
			{
				continue;
			}
		}

		FVector moveDirection = MoveDirArrowComp->GetForwardVector();
		moveDirection = bIsForward ? moveDirection : -moveDirection;

		// 물리 객체인지 확인
		UPrimitiveComponent* primComp = Cast<UPrimitiveComponent>(comp);
		if (primComp && primComp->IsSimulatingPhysics())
		{
			/**
			 * [개선] SetPhysicsLinearVelocity로 일정 속도 유지
			 * - AddForce는 매 프레임 누적되어 무한 가속되므로 부적합
			 * - 서버 전용 실행이므로 클라이언트와 충돌 없음
			 * - Z축 속도는 유지하여 중력/점프 등 유지
			 */
			FVector targetVelocity = moveDirection * MoveSpeed;
			FVector currentVelocity = primComp->GetPhysicsLinearVelocity();

			// Z축 속도는 현재 속도 유지 (중력, 점프 등)
			FVector newVelocity = targetVelocity;
			newVelocity.Z = currentVelocity.Z;

			primComp->SetPhysicsLinearVelocity(newVelocity, false);
		}
		else
		{
			// 물리가 아닌 경우 (Hook/PickUp 중) - 이동하지 않음
			// Hook 중에는 물리가 꺼져있으므로 여기서 멈춤
		}
	}
}
