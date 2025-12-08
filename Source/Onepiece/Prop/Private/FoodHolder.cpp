// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "FoodHolder.h"

#include "ALingoGameState.h"
#include "Food.h"
#include "GameLogging.h"
#include "ANetworkBroadcastActor.h"
#include "Onepiece/Onepiece.h"
#include "Components/BoxComponent.h"

// Sets default values
AFoodHolder::AFoodHolder()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AFoodHolder::BeginPlay()
{
	Super::BeginPlay();

	// Unbind parent's OnBoxOverlapBegin and bind our own
	if (BoxCollision)
	{
		BoxCollision->OnComponentBeginOverlap.Clear();
		BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AFoodHolder::OnFoodBoxOverlapBegin);
	}
}

void AFoodHolder::SetAnswerFoodIndex(int32 InAnswerFoodIndex)
{
	this->AnswerFoodIndex = InAnswerFoodIndex;
}

void AFoodHolder::OnFoodBoxOverlapBegin(
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

	if (bIsActivated)
	{
		// 이미 Activate 되었으면 작동 안 함
		return;
	}

	// Food인지 확인
	if (AFood* Food = Cast<AFood>(OtherActor))
	{
		bool bSuccess = CheckFood(Food);

		// 블루프린트 이벤트 호출 (부모 클래스의 OnActivate)
		OnActivate(bSuccess);

		// NetworkBroadcastActor를 통해 모든 클라이언트에 메시지 브로드캐스트
		if (auto DM = ANetworkBroadcastActor::Get(this))
		{
			DM->SendTutorMessage(FText::FromString(bSuccess ? GameMessage::Holder_Success : GameMessage::Holder_Fail), this);
		}
	}
}

bool AFoodHolder::CheckFood(AFood* TargetFood)
{
	if (!TargetFood)
		return false;

	// Food Index 비교
	bool bSuccess = false;
	
	ALingoGameState* GS = Cast<ALingoGameState>(GetWorld()->GetGameState());
	if (GS)
	{
		const int32 CorrectIdx = GS->GetListenScenarioData().correct_answer_index;

		if (TargetFood->GetFoodIndex() == CorrectIdx)
			bSuccess = true;
	}

	PRINTLOG(TEXT("FoodIndex: %d (Answer: %d), Result: %s"),
		TargetFood->GetFoodIndex(), AnswerFoodIndex,
		bSuccess ? TEXT("Success") : TEXT("Fail"));

	if (bSuccess)
	{
		// Success: Food를 HoldPos 위치보다 살짝 위에 배치
		if (HoldPos)
		{
			FVector ActivatedLocation = HoldPos->GetComponentLocation();
			ActivatedLocation.Z += ActivatedHeightOffset;
			TargetFood->SetActorLocation(ActivatedLocation);
			TargetFood->SetActorRotation(HoldPos->GetComponentRotation());
		}

		// Food의 물리 및 충돌 비활성화
		if (TargetFood->Mesh)
		{
			TargetFood->Mesh->SetSimulatePhysics(false);
			TargetFood->Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}

		// Activate 상태로 전환
		bIsActivated = true;
		CurTarget = TargetFood;

		// 서버에서도 머티리얼 업데이트 (클라이언트는 OnRep_IsActivated에서 호출됨)
		UpdateActivateState(true);
	}
	else
	{
		// Fail: Food 제거
		bIsActivated = false;
		TargetFood->Destroy();

		// 서버에서 머티리얼 업데이트 (오답)
		UpdateActivateState(false);
	}

	return bSuccess;
}
