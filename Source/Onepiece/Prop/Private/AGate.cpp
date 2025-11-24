// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "AGate.h"

#include "UBroadcastManager.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "Onepiece/Onepiece.h"

/**
 * @file AGate.cpp
 * @brief AGate의 동작을 구현합니다.
 */

/** @brief 충돌 프리미티브와 초기 개방 상태를 구성합니다. */
AGate::AGate()
{
    RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = RootComp;
    
    BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
    BoxCollision->SetupAttachment(RootComponent);

    BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    BoxCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    BoxCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    BoxCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

    BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AGate::OnOverlapBegin);
    BoxCollision->OnComponentEndOverlap.AddDynamic(this, &AGate::OnOverlapEnd);

    Door_Left = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door_Left"));
    Door_Left->SetupAttachment(RootComponent);

    Door_Right = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door_Right"));
    Door_Right->SetupAttachment(RootComponent);

    bIsOpen = false;
    GateID = -1;

    PrimaryActorTick.bCanEverTick = false;
}

/** @brief 플레이 시작 시 브로드캐스트 도어 메시지를 구독합니다. */
void AGate::BeginPlay()
{
    Super::BeginPlay();

    if (auto EventManager = UBroadcastManager::Get(this))
    {
        EventManager->OnDoorMessage.AddDynamic(this, &AGate::OnDoorMessage);
    }
}

/**
 * @brief 이 게이트를 대상으로 한 브로드캐스트 도어 토글에 반응합니다.
 * @param InGateID 조작할 게이트 식별자입니다.
 * @param Open 게이트를 열지 닫을지 여부입니다.
 */
void AGate::OnDoorMessage(const int32 InGateID, const bool Open)
{
    if ( InGateID != this->GateID)
    {
        // 나한테 명령한거 아님
        return;
    }

    if ( Open == this->bIsOpen)
    {
        // 내가 또 해야하는거 아님
        return;
    }

    if (Open)
    {
        OpenDoor();
    }
    else
    {
        CloseDoor();
    }
}

/** @brief 게이트를 열고 대응되는 오디오 피드백을 재생합니다. */
void AGate::OpenDoor()
{
    if (bIsOpen == true)
        return;

    bIsOpen = true;
    PlayOpenDoorAnimation();
}

/** @brief 플레이어가 주변에 남지 않았을 때 게이트를 닫는 플레이스홀더입니다. */
void AGate::CloseDoor()
{
    if (bIsOpen == false)
        return;
    
    bIsOpen = false;
    PlayCloseDoorAnimation();
}

/**
 * @brief 겹친 액터가 플레이어 폰인지 검증하는 헬퍼입니다.
 * @param OtherActor 평가할 대상 액터입니다.
 * @return 액터가 게이트 동작을 트리거해야 하면 @c true를 반환합니다.
 */
bool AGate::IsPlayerActor(const AActor* OtherActor) const
{
    if ( OtherActor == nullptr)
        return false;

    if ( OtherActor == this)
        return false;

    if ( OtherActor->ActorHasTag(GameTags::Player) == false)
        return false;

    return true;
}

/** @brief 트리거 볼륨에 진입한 플레이어를 처리하여 게이트를 엽니다. */
void AGate::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (IsPlayerActor(OtherActor))
    {
        if ( auto Pawn = Cast<APawn>(OtherActor) )
        {
            if ( OverlappingPawns.Contains(Pawn) == false )
                OverlappingPawns.Add(Pawn);
       }
        
        OpenDoor();
    }
}

/** @brief 트리거를 떠나는 플레이어를 추적해 비었을 때 게이트를 닫습니다. */
void AGate::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (IsPlayerActor(OtherActor))
    {
        if ( auto Pawn = Cast<APawn>(OtherActor) )
            OverlappingPawns.Remove(Pawn);

        if (OverlappingPawns.Num() == 0)
            CloseDoor();
    }
}