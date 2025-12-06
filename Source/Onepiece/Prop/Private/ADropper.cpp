// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "ADropper.h"

#include "GameLogging.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Animation/AnimationAsset.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"

ADropper::ADropper()
{
    PrimaryActorTick.bCanEverTick = false;

    // Replication
    bReplicates = true;

    // Root component
    USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
    RootComponent = Root;

    // SkeletalMesh component
    SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
    SkeletalMesh->SetupAttachment(RootComponent);

    // Cube (StaticMesh) component
    BoxCollision = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cube"));
    BoxCollision->SetupAttachment(SkeletalMesh);
    BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    BoxCollision->SetCollisionResponseToAllChannels(ECR_Block);

    // SpawnPos component
    SpawnPos = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnPos"));
    SpawnPos->SetupAttachment(SkeletalMesh);
}

void ADropper::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

AActor* ADropper::SpawnActor(TSubclassOf<AActor> SpawnActorClass)
{
    if (!SpawnActorClass || !SpawnPos)
    {
        PRINTLOG( TEXT("SpawnActorClass or SpawnPos is not set"));
        return nullptr;
    }

    // 서버에서만 실행
    if (!HasAuthority())
    {
        PRINTLOG(TEXT("ADropper::SpawnActor - Not authority, ignoring"));
        return nullptr;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.Instigator = GetInstigator();

    auto SpawnedActor = GetWorld()->SpawnActor<AActor>(SpawnActorClass,
        SpawnPos->GetComponentLocation(),
        SpawnPos->GetComponentRotation(),
        SpawnParams);

    if (SpawnedActor)
    {
        // 모든 클라이언트에서 애니메이션 재생
        if (SkeletalMesh && AnimToPlay)
            Multicast_PlayAnimation();

        // 타이머 시작
        GetWorldTimerManager().SetTimer(
            DelayTimerHandle,
            this,
            &ADropper::OnDelayCompleted,
            2.0f,
            false
        );
    }

    return SpawnedActor;
}

void ADropper::OnDelayCompleted()
{
    // 서버 타이머 콜백 → Multicast로 충돌 비활성화
    if (!HasAuthority())
        return;

    Multicast_DisableCollision();
}

void ADropper::OnRestoreDelayCompleted()
{
    // 서버 타이머 콜백 → Multicast로 충돌 복구
    if (!HasAuthority())
        return;
    
    Multicast_RestoreCollision();
}

void ADropper::Multicast_PlayAnimation_Implementation()
{
    if (SkeletalMesh && AnimToPlay)
    {
        SkeletalMesh->PlayAnimation(AnimToPlay, false);
        PRINTLOG(TEXT("ADropper::Multicast_PlayAnimation - Playing animation, Role: %s"),
            GetLocalRole() == ROLE_Authority ? TEXT("Server") : TEXT("Client"));
    }
}

void ADropper::Multicast_DisableCollision_Implementation()
{
    if (BoxCollision)
    {
        BoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        PRINTLOG(TEXT("ADropper::Multicast_DisableCollision - Disabled collision, Role: %s"),
            GetLocalRole() == ROLE_Authority ? TEXT("Server") : TEXT("Client"));
    }

    // 서버에서만 다음 타이머 설정
    if (HasAuthority())
    {
        GetWorldTimerManager().SetTimer(
            RestoreTimerHandle,
            this,
            &ADropper::OnRestoreDelayCompleted,
            2.0,
            false
        );
    }
}

void ADropper::Multicast_RestoreCollision_Implementation()
{
    if (BoxCollision)
    {
        BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        BoxCollision->SetCollisionResponseToAllChannels(ECR_Block);

        PRINTLOG(TEXT("ADropper::Multicast_RestoreCollision - Restored collision, Role: %s"),
            GetLocalRole() == ROLE_Authority ? TEXT("Server") : TEXT("Client"));
    }
}
