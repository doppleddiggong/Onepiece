// Copyright (c) 2025.
// Proprietary and confidential.

#include "ADropper.h"

#include "Luggage.h"
#include "Food.h"
#include "GameLogging.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
#include "UDialogManager.h"
#include "Net/UnrealNetwork.h"

ADropper::ADropper()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

    // Root
    USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;

    // SkeletalMesh
    SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
    SkeletalMesh->SetupAttachment(RootComponent);

    // Collision Mesh
    BoxCollision = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoxCollision"));
    BoxCollision->SetupAttachment(SkeletalMesh);
    BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    BoxCollision->SetCollisionResponseToAllChannels(ECR_Block);

    // Spawn Position
    SpawnPos = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnPos"));
    SpawnPos->SetupAttachment(SkeletalMesh);
}

void ADropper::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ADropper, SpawnClass);
    DOREPLIFETIME(ADropper, NextData);
    DOREPLIFETIME(ADropper, NextFoodData);
    DOREPLIFETIME(ADropper, bIsSpawnIng);
}

bool ADropper::RequestSpawn()
{
    PRINTLOG(TEXT("Dropper GUID = %s, Authority=%d"),
    *GetPathName(), HasAuthority());
    
    if (!SpawnClass || !SpawnPos)
        return false;

    if (bIsSpawnIng)
        return false;

    if (HasAuthority())
        Spawn();
    else
        Server_Spawn();

    return true;
}

void ADropper::Spawn()
{
    if (!HasAuthority())
        return;

    if (!SpawnClass || !SpawnPos)
    {
        PRINTLOG(TEXT("ADropper::SpawnInternal - Invalid data"));
        bIsSpawnIng = false;
        return;
    }
   
    FActorSpawnParameters Params;
    Params.Owner = this;
    Params.Instigator = GetInstigator();

    // 실제 스폰
    AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(
        SpawnClass,
        SpawnPos->GetComponentLocation(),
        SpawnPos->GetComponentRotation(),
        Params
    );

    if (!SpawnedActor)
    {
        bIsSpawnIng = false;
        return;
    }

    bIsSpawnIng = true;

    // 데이터 전달
    if (auto tmpLuggage = Cast<Aluggage>(SpawnedActor))
    {
        tmpLuggage->SetLuggageInfo(NextData.SpawnIndex, NextData.word2.name, NextData.word1.name);

        int32 ColorIdx = FCString::Atoi(*NextData.word2.code);
        tmpLuggage->ApplyColorToMesh(ColorIdx);

        int32 PatternIdx = FCString::Atoi(*NextData.word1.code);
        tmpLuggage->ApplyPatternToMesh(PatternIdx);
    }
    else if (auto tmpFood = Cast<AFood>(SpawnedActor))
    {
        FString InfoToShow = FString::Printf(TEXT("%s / %s"), *NextFoodData.word1.name, *NextFoodData.word2.name);
        tmpFood->SetFoodInfo(NextFoodData.SpawnIndex, InfoToShow);

        UE_LOG(LogTemp, Warning, TEXT("[Dropper::Spawn] Food spawned with Index=%d, Name=%s"),
            NextFoodData.SpawnIndex, *InfoToShow);
    }

    // 애니메이션 실행
    if (SkeletalMesh && AnimToPlay)
        Multicast_PlayAnimation();

    // 2초 뒤 충돌 OFF → 다시 ON
    GetWorldTimerManager().SetTimer(
        DelayTimerHandle,
        this,
        &ADropper::OnDelayCompleted,
        2.0f,
        false
    );
}

void ADropper::Server_Spawn_Implementation()
{
    PRINTLOG(TEXT("Dropper Authority=%d  RemoteRole=%d"), HasAuthority(), (int)GetRemoteRole());
    
    Spawn();
}

void ADropper::OnDelayCompleted()
{
    if (!HasAuthority())
        return;

    Multicast_DisableCollision();

    GetWorldTimerManager().SetTimer(
        RestoreTimerHandle,
        this,
        &ADropper::OnRestoreCompleted,
        2.0f,
        false
    );
}

void ADropper::OnRestoreCompleted()
{
    if (!HasAuthority())
        return;

    Multicast_RestoreCollision();
    bIsSpawnIng = false;
}

void ADropper::Multicast_PlayAnimation_Implementation()
{
    if (SkeletalMesh && AnimToPlay)
        SkeletalMesh->PlayAnimation(AnimToPlay, false);
}

void ADropper::Multicast_DisableCollision_Implementation()
{
    if (BoxCollision)
        BoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ADropper::Multicast_RestoreCollision_Implementation()
{
    if (BoxCollision)
    {
        BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        BoxCollision->SetCollisionResponseToAllChannels(ECR_Block);
    }
}
