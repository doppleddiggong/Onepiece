// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "ADropper.h"

#include "GameLogging.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Animation/AnimationAsset.h"
#include "TimerManager.h"

ADropper::ADropper()
{
    PrimaryActorTick.bCanEverTick = false;

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

AActor* ADropper::SpawnActor(TSubclassOf<class AActor> SpawnActorClass)
{
    if (!SpawnActorClass || !SpawnPos)
    {
        PRINTLOG( TEXT("SpawnActorClass or SpawnPos is not set"));
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
        // Start timer for delayed animation and collision disable
        GetWorldTimerManager().SetTimer(
            DelayTimerHandle,
            this,
            &ADropper::PlayAnimationAndDisableCollision,
            Delay,
            false
        );
    }

    return SpawnedActor;
}

void ADropper::PlayAnimationAndDisableCollision()
{
    if (SkeletalMesh && AnimToPlay)
        SkeletalMesh->PlayAnimation(AnimToPlay, false);

    if (BoxCollision)
    {
        BoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

        // NoCollision 후 다시 BlockAll로 복구하는 타이머 설정
        GetWorldTimerManager().SetTimer(
            RestoreTimerHandle,
            this,
            &ADropper::RestoreCollision,
            Delay,
            false
        );
    }
}

void ADropper::RestoreCollision()
{
    if (BoxCollision)
    {
        BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        BoxCollision->SetCollisionResponseToAllChannels(ECR_Block);
    }
}