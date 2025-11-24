// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AGate.generated.h"

/// @file AGate.h
/// @brief 출입문 개폐를 담당하는 게이트 액터를 선언합니다.

/// @brief 플레이어 접근 및 브로드캐스트 메시지에 반응해 문을 여닫는 액터입니다.
UCLASS()
class ONEPIECE_API AGate : public AActor
{
    GENERATED_BODY()

public:
    /// @brief 컴포넌트 계층을 구성하는 기본 생성자입니다.
    AGate();

    /// @brief 문을 여는 동작을 트리거합니다.
    UFUNCTION(BlueprintCallable, Category = "Gate")
    void OpenDoor();

    /// @brief 문을 닫는 동작을 트리거합니다.
    UFUNCTION(BlueprintCallable, Category = "Gate")
    void CloseDoor();

protected:
    /// @brief 브로드캐스트 구독 등 초기화를 수행합니다.
    virtual void BeginPlay() override;

    /// @brief 외부 시스템에서 전달된 개폐 메시지를 처리합니다.
    /// @param InGateID [in] 대상 게이트 식별자입니다.
    /// @param Open [in] true이면 문을 열고 false이면 닫습니다.
    UFUNCTION()
    void OnDoorMessage(int32 InGateID, bool Open);

    /// @brief 플레이어가 감지 범위에 들어왔을 때 호출됩니다.
    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    /// @brief 플레이어가 감지 범위에서 벗어났을 때 호출됩니다.
    UFUNCTION()
    void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    /// @brief 문을 열 때 재생할 블루프린트 애니메이션입니다.
    UFUNCTION(BlueprintImplementableEvent, Category = "Gate")
    void PlayOpenDoorAnimation();

    /// @brief 문을 닫을 때 재생할 블루프린트 애니메이션입니다.
    UFUNCTION(BlueprintImplementableEvent, Category = "Gate")
    void PlayCloseDoorAnimation();

    /// @brief 겹친 액터가 플레이어인지 판별합니다.
    bool IsPlayerActor(const AActor* OtherActor) const;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<class USceneComponent> RootComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<class UStaticMeshComponent> Door_Left;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<class UStaticMeshComponent> Door_Right;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<class UBoxComponent> BoxCollision;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gate")
    int32 GateID;

private:
    UPROPERTY(VisibleInstanceOnly, Category = "Gate")
    bool bIsOpen;

    UPROPERTY()
    TArray<TObjectPtr<class APawn>> OverlappingPawns;
};
