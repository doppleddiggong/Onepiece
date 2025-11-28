// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file UHookSystem.h
 * @brief 그래플링 훅 시스템
 * @details 블리츠크랭크 Q와 유사한 훅 끌어오기 시스템
 */

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UHookSystem.generated.h"

class UStaticMeshComponent;

/** Hook 상태 */
UENUM(BlueprintType)
enum class EHookState : uint8
{
	Idle		UMETA(DisplayName = "Idle"),
	Launching	UMETA(DisplayName = "Launching"),
	Pulling		UMETA(DisplayName = "Pulling")
};

/**
 * @brief 그래플링 훅 시스템
 * @details 대상 Actor를 플레이어 앞으로 끌어오는 훅 시스템
 * @ingroup Character
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ONEPIECE_API UHookSystem : public UActorComponent
{
	GENERATED_BODY()

public:
	UHookSystem();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	/** Hook System 초기화 (PlayerActor의 Cable과 Projectile Mesh 전달) */
	/** Cylinder mesh is used to visualize the cable connection. */
	UFUNCTION(BlueprintCallable, Category = "Hook")
	void InitSystem(class UStaticMeshComponent* InCableMesh, class UStaticMeshComponent* InProjectileMesh);

	/** Hook 시도 (우클릭 입력 시 호출) - Client에서 호출 → Server로 전달 */
	UFUNCTION(BlueprintCallable, Category = "Hook")
	void TryHook();

	/** Hook 시작 - Server에서만 실행 */
	UFUNCTION(BlueprintCallable, Category = "Hook")
	void StartHook(const FHitResult& Hit);

	/** Hook 해제 - Server에서만 실행 */
	UFUNCTION(BlueprintCallable, Category = "Hook")
	void ReleaseHook();

protected:
	/** [Server RPC] Hook 시도 요청 */
	UFUNCTION(Server, Reliable)
	void ServerTryHook(const FHitResult& HitResult);

	/** [Server RPC] Hook 해제 요청 */
	UFUNCTION(Server, Reliable)
	void ServerReleaseHook();

public:

	/** Hook 중인지 확인 */
	UFUNCTION(BlueprintPure, Category = "Hook")
	bool IsHooking() const { return HookedTarget != nullptr; }

	/** 현재 훅 타겟이 있는지 확인 */
	UFUNCTION(BlueprintPure, Category = "Hook")
	bool HasHookTarget() const { return CurHookTarget != nullptr; }

	bool PerformCenterLineTrace(FHitResult& OutHit);
	
protected:
	/** 훅 타겟 감지 */
	void DetectHookTarget();

	/** 훅 타겟 UI 업데이트 */
	void UpdateHookTargetUI();

	/** 훅 발사 업데이트 */
	void UpdateHookLaunching(float DeltaTime);

	/** Hook 대상 이동 처리 */
	void UpdateHookPulling(float DeltaTime);

	/** Cable 업데이트 */
	void UpdateCable();

	/** HookState 복제 시 호출 */
	UFUNCTION()
	void OnRep_HookState();

	/** HookProjectileLocation 복제 시 호출 */
	UFUNCTION()
	void OnRep_HookProjectileLocation();
	
public:
	/** Hook 발사 속도 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hook|Launch")
	float LaunchSpeed = 2000.0f;

	/** Hook 최대 거리 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hook|Launch")
	float MaxHookDistance = 1500.0f;
/
	/** Hook 이동 속도 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hook|Pull")
	float HookSpeed = 500.0f;

	/** 목표 거리 (플레이어 앞 얼마나 떨어진 위치로 끌어올지) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hook|Pull")
	float DesiredDistance = 100.0f;

	/** LineTrace 최대 거리 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float InteractionDistance = 1200.0f;
	
	/** Hook 완료 판정 거리 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hook|Pull")
	float CompleteThreshold = 100.0f;

	/** 디버그 표시 여부 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bShowDebugInfo = true;

	/** Cable Mesh 두께 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hook|Visual")
	float CableThickness = 8.0f;
	
protected:
	/** 현재 Hook 상태 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HookState, Category = "Hook")
	EHookState HookState = EHookState::Idle;

	/** 현재 Hook된 대상 */
	UPROPERTY(Replicated)
	TObjectPtr<AActor> HookedTarget;

	/** 현재 감지된 훅 가능 타겟 */
	UPROPERTY()
	TObjectPtr<AActor> CurHookTarget;

	UPROPERTY()
	TObjectPtr<class Aluggage> CurHookTarget_Luggage;
	
	UPROPERTY()
	TObjectPtr<class UStaticMeshComponent> CableMesh;

	UPROPERTY()
	TObjectPtr<class UStaticMeshComponent> ProjectileMesh;

	/** 훅 발사체 위치 */
	UPROPERTY(ReplicatedUsing = OnRep_HookProjectileLocation)
	FVector HookProjectileLocation;

	/** 훅 발사 방향 */
	UPROPERTY(Replicated)
	FVector HookLaunchDirection;

	/** Owner PlayerActor 캐싱 */
	UPROPERTY()
	TObjectPtr<class APlayerActor> OwnerPlayer;

	/** Cylinder 기반 Cable Mesh 원래 크기 */
	float CableMeshBaseLength = 100.0f;
	float CableMeshBaseRadius = 1.0f;

	/** Hook 전 원래 Collision 상태 저장 */
	TEnumAsByte<ECollisionEnabled::Type> OriginalCollisionEnabled;

	/** Cable Mesh 변환 보조 */
	void UpdateCableMeshTransform(const FVector& CableStart, const FVector& CableEnd);
};
