// Copyright (c) 2025 Doppleddiggong.
// Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "NetworkData.h"
#include "GameFramework/Actor.h"
#include "ADropper.generated.h"

USTRUCT(BlueprintType)
struct FLuggageData
{
    GENERATED_BODY()

    /** 시나리오 단어 정보 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FWordInfo word1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FWordInfo word2;

    /** 스폰 인덱스(선택적으로 사용) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 SpawnIndex = -1;

    FLuggageData() {}

    FLuggageData(const FWordInfo& InWord1, const FWordInfo& InWord2, int32 InSpawnIndex = -1)
        : word1(InWord1)
        , word2(InWord2)
        , SpawnIndex(InSpawnIndex)
    {}
};

USTRUCT(BlueprintType)
struct FFoodData
{
    GENERATED_BODY()

    /** 시나리오 단어 정보 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FWordInfo word1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FWordInfo word2;

    /** 스폰 인덱스(선택적으로 사용) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 SpawnIndex = -1;

    FFoodData() {}

    FFoodData(const FWordInfo& InWord1, const FWordInfo& InWord2, int32 InSpawnIndex = -1)
        : word1(InWord1), word2(InWord2)
        , SpawnIndex(InSpawnIndex)
    {}
};

UCLASS()
class ONEPIECE_API ADropper : public AActor
{
    GENERATED_BODY()

public:
    ADropper();

    /** 스폰 전에 Dropper가 어떤 클래스를 스폰할지 등록 */
    void SetSpawnClass(TSubclassOf<AActor> InClass) { SpawnClass = InClass; }

    /** [Luggage용] 스폰 전에 Dropper가 생성될 액터에게 넘겨줄 데이터 등록 */
    void SetSpawnData(const FLuggageData& InData) { NextData = InData; }

    /** [Food용] 스폰 전에 Dropper가 생성될 액터에게 넘겨줄 데이터 등록 */
    void SetFoodSpawnData(const FFoodData& InData) { NextFoodData = InData; }

    
    /** 스폰 요청 (서버에서만 동작) */
    UFUNCTION(BlueprintCallable)
    bool RequestSpawn();

    FORCEINLINE bool IsBusy() const { return bIsSpawnIng; };

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Index)
    int32 DropperIndex = 0;
    
protected:
    /** 서버에서만 실행되는 실제 스폰 로직 */
    void Spawn();

    UFUNCTION(Server, Reliable)
    void Server_Spawn();
    
    /** 애니메이션 재생 */
    UFUNCTION(NetMulticast, Reliable)
    void Multicast_PlayAnimation();

    /** 충돌 비활성화 */
    UFUNCTION(NetMulticast, Reliable)
    void Multicast_DisableCollision();

    /** 충돌 복구 */
    UFUNCTION(NetMulticast, Reliable)
    void Multicast_RestoreCollision();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
    void OnDelayCompleted();
    void OnRestoreCompleted();

private:
    /** 스폰 위치 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<USceneComponent> SpawnPos;

    /** 드랍퍼 스켈레탈 메시 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<USkeletalMeshComponent> SkeletalMesh;

    /** 충돌 박스 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UStaticMeshComponent> BoxCollision;

    /** 드랍 시 재생할 애니메이션 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UAnimationAsset> AnimToPlay;

    /** 이번 스폰에서 사용할 클래스 */
    UPROPERTY(Replicated)
    TSubclassOf<AActor> SpawnClass;

    /** 이번 스폰에서 사용할 데이터 */
    UPROPERTY(Replicated)
    FLuggageData NextData;

    /** 이번 스폰에서 사용할 데이터 */
    UPROPERTY(Replicated)
    FFoodData NextFoodData;

    /** 스폰 처리 진행 중 여부 (RPC로 공유) */
    UPROPERTY(Replicated)
    bool bIsSpawnIng = false;

    FTimerHandle DelayTimerHandle;
    FTimerHandle RestoreTimerHandle;
};
