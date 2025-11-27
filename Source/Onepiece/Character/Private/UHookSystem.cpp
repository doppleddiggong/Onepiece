// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file UHookSystem.cpp
 * @brief UHookSystem 구현
 */

#include "UHookSystem.h"
#include "APlayerActor.h"
#include "UInteractionSystem.h"
#include "UHookComponent.h"
#include "UMainWidget.h"
#include "GameLogging.h"
#include "DrawDebugHelpers.h"
#include "Math/RotationMatrix.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Net/UnrealNetwork.h"

UHookSystem::UHookSystem()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = TG_PostUpdateWork;
	SetIsReplicatedByDefault(true);
}

void UHookSystem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHookSystem, HookState);
	DOREPLIFETIME(UHookSystem, HookedTarget);
	DOREPLIFETIME(UHookSystem, HookProjectileLocation);
	DOREPLIFETIME(UHookSystem, HookLaunchDirection);
}

void UHookSystem::BeginPlay()
{
	Super::BeginPlay();

	// Owner PlayerActor 캐싱
	OwnerPlayer = Cast<APlayerActor>(GetOwner());
	if (!OwnerPlayer)
	{
		PRINTLOG(TEXT("UHookSystem: Owner is not APlayerActor!"));
		SetComponentTickEnabled(false);
		return;
	}
}

void UHookSystem::InitSystem(UStaticMeshComponent* InCableMesh, UStaticMeshComponent* InProjectileMesh)
{
	CableMesh = InCableMesh;
	ProjectileMesh = InProjectileMesh;

	if (CableMesh)
	{
		// Cable Component 초기 설정 - 직선으로 나가도록
		CableMesh->SetVisibility(false);
		CableMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CableMesh->SetCastShadow(false);
		CableMesh->SetMobility(EComponentMobility::Movable);
		CableMesh->SetUsingAbsoluteLocation(true);
		CableMesh->SetUsingAbsoluteRotation(true);

		if (UStaticMesh* Mesh = CableMesh->GetStaticMesh())
		{
			CableMeshBaseLength = FMath::Max(Mesh->GetBounds().BoxExtent.Z * 2.0f, 1.0f);
			CableMeshBaseRadius = FMath::Max(Mesh->GetBounds().BoxExtent.X, Mesh->GetBounds().BoxExtent.Y);
		}

		PRINTLOG(TEXT("UHookSystem: InitSystem completed with Cable Mesh (straight line mode)"));
	}
	else
	{
		PRINTLOG(TEXT("UHookSystem: InitSystem - Cable Mesh is null!"));
	}

	if (ProjectileMesh)
	{
		ProjectileMesh->SetVisibility(false);
		PRINTLOG(TEXT("UHookSystem: InitSystem completed with ProjectileMesh"));
	}
	else
	{
		PRINTLOG(TEXT("UHookSystem: InitSystem - ProjectileMesh is null!"));
	}
}

void UHookSystem::TickComponent(float DeltaTime, ELevelTick TickType,
                                 FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 상태별 업데이트
	switch (HookState)
	{
		case EHookState::Idle:
			DetectHookTarget();
			UpdateHookTargetUI();
			break;

		case EHookState::Launching:
			UpdateHookLaunching(DeltaTime);
			UpdateCable();
			break;

		case EHookState::Pulling:
			UpdateHookPulling(DeltaTime);
			UpdateCable();
			break;
	}
}

void UHookSystem::TryHook()
{
	if (!OwnerPlayer || !OwnerPlayer->InteractionSystem)
	{
		PRINTLOG(TEXT("UHookSystem: OwnerPlayer or InteractionSystem is null!"));
		return;
	}

	// 이미 Hook 중이면 무시
	if (HookState != EHookState::Idle)
	{
		PRINTLOG(TEXT("UHookSystem: Already in hook state!"));
		return;
	}

	// InteractionSystem의 LineTrace 사용
	FHitResult HitResult;
	if (!PerformCenterLineTrace(HitResult))
	{
		PRINTLOG(TEXT("UHookSystem: LineTrace failed!"));
		return;
	}

	// Hit된 Actor가 UHookComponent를 가지고 있는지 확인
	AActor* HitActor = HitResult.GetActor();
	if (!HitActor)
	{
		PRINTLOG(TEXT("UHookSystem: No actor hit!"));
		return;
	}

	UHookComponent* HookComp = HitActor->FindComponentByClass<UHookComponent>();
	if (!HookComp || !HookComp->bIsHookable)
	{
		PRINTLOG(TEXT("UHookSystem: Target is not hookable!"));
		return;
	}

	// Server에 Hook 요청
	ServerTryHook(HitResult);
}

void UHookSystem::ServerTryHook_Implementation(const FHitResult& HitResult)
{
	// 서버에서만 실행
	if (!OwnerPlayer || !OwnerPlayer->HasAuthority())
	{
		return;
	}

	// 다시 한번 유효성 검사 (보안)
	AActor* HitActor = HitResult.GetActor();
	if (!HitActor)
	{
		return;
	}

	UHookComponent* HookComp = HitActor->FindComponentByClass<UHookComponent>();
	if (!HookComp || !HookComp->bIsHookable)
	{
		return;
	}

	// Hook 시작
	StartHook(HitResult);
}

void UHookSystem::StartHook(const FHitResult& Hit)
{
	AActor* TargetActor = Hit.GetActor();
	if (!TargetActor)
	{
		PRINTLOG(TEXT("UHookSystem: StartHook - Target actor is null!"));
		return;
	}

	// 발사 상태로 전환
	HookState = EHookState::Launching;
	HookedTarget = TargetActor;

	// 발사 위치 및 방향 설정
	HookProjectileLocation = OwnerPlayer->GetActorLocation();
	HookLaunchDirection = (Hit.Location - HookProjectileLocation).GetSafeNormal();

	// Cable 및 Projectile Mesh 표시
	if (CableMesh)
	{
		CableMesh->SetVisibility(true);
	}

	if (ProjectileMesh)
	{
		ProjectileMesh->SetVisibility(true);
		ProjectileMesh->SetWorldLocation(HookProjectileLocation);
	}

	PRINTLOG(TEXT("UHookSystem: Hook launched toward %s"), *HookedTarget->GetName());
}

void UHookSystem::ReleaseHook()
{
	// 클라이언트에서 호출 시 Server에 요청
	if (OwnerPlayer && !OwnerPlayer->HasAuthority())
	{
		ServerReleaseHook();
		return;
	}

	// 서버에서 실행
	if (HookState != EHookState::Idle)
	{
		if (HookedTarget)
		{
			PRINTLOG(TEXT("UHookSystem: Hook released from %s"), *HookedTarget->GetName());
		}

		HookState = EHookState::Idle;
		HookedTarget = nullptr;

		// Cable 및 Projectile Mesh 숨김
		if (CableMesh)
		{
			CableMesh->SetVisibility(false);
		}

		if (ProjectileMesh)
		{
			ProjectileMesh->SetVisibility(false);
		}
	}
}

void UHookSystem::ServerReleaseHook_Implementation()
{
	ReleaseHook();
}

void UHookSystem::DetectHookTarget()
{
	if (!OwnerPlayer || !OwnerPlayer->InteractionSystem)
		return;

	// LineTrace로 타겟 감지
	FHitResult HitResult;
	if (OwnerPlayer->InteractionSystem->PerformCenterLineTrace(HitResult))
	{
		AActor* HitActor = HitResult.GetActor();
		if (HitActor)
		{
			UHookComponent* HookComp = HitActor->FindComponentByClass<UHookComponent>();
			if (HookComp && HookComp->bIsHookable)
			{
				CurHookTarget = HitActor;
				return;
			}
		}
	}

	CurHookTarget = nullptr;
}


bool UHookSystem::PerformCenterLineTrace(FHitResult& OutHit)
{
	if (!OwnerPlayer)
		return false;

	// 플레이어 컨트롤러 가져오기
	APlayerController* PC = Cast<APlayerController>(OwnerPlayer->GetController());
	if (!PC)
		return false;

	APlayerCameraManager* CameraManager = PC->PlayerCameraManager;
	if (!CameraManager)
		return false;

	FVector CameraLocation = CameraManager->GetCameraLocation();
	FVector CameraForward = CameraManager->GetCameraRotation().Vector();

	// 레이 트레이스 시작/끝 지점                                                                                                                                                    
	FVector TraceStart = CameraLocation;
	FVector TraceEnd = TraceStart + (CameraForward * InteractionDistance);

	// Ray trace 실행
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		OutHit, TraceStart, TraceEnd, ECC_Visibility
	);

	// 디버그 라인
	if (bShowDebugInfo)
	{
		DrawDebugLine(
			GetWorld(), TraceStart, TraceEnd,
			bHit ? FColor::Green : FColor::Red,
			false, 0.0f, 0, 0.5f
		);
	}

	return bHit;
}

void UHookSystem::UpdateHookTargetUI()
{
	if (!OwnerPlayer)
		return;

	// MainWidget에 훅 타겟 표시 업데이트
	if (auto MainWidget = OwnerPlayer->GetMainWidget())
	{
		// 에임 상태에 따라 이미지 변경
		MainWidget->UpdateHookIndicatorState(CurHookTarget != nullptr);
	}

	// 디버그 표시
	if (bShowDebugInfo && CurHookTarget)
	{
		FVector TargetLocation = CurHookTarget->GetActorLocation();
		DrawDebugSphere(
			GetWorld(),
			TargetLocation,
			50.0f,
			12,
			FColor::Yellow,
			false,
			0.0f,
			0,
			2.0f
		);
	}
}

void UHookSystem::UpdateHookLaunching(float DeltaTime)
{
	if (!HookedTarget || !IsValid(HookedTarget))
	{
		PRINTLOG(TEXT("UHookSystem: Target lost during launch"));
		ReleaseHook();
		return;
	}

	// 서버 권한 체크 - 상태 변경은 서버에서만
	if (!OwnerPlayer || !OwnerPlayer->HasAuthority())
	{
		return;
	}

	// 훅 발사체 이동
	HookProjectileLocation += HookLaunchDirection * LaunchSpeed * DeltaTime;

	// 타겟 위치
	FVector TargetLocation = HookedTarget->GetActorLocation();

	// 타겟에 도달했는지 확인
	float DistanceToTarget = FVector::Dist(HookProjectileLocation, TargetLocation);

	if (DistanceToTarget < 50.0f)
	{
		// 타겟에 도달 - Pulling 상태로 전환
		HookState = EHookState::Pulling;
		PRINTLOG(TEXT("UHookSystem: Hook reached target, start pulling"));
	}
	else
	{
		// 최대 거리 체크
		float DistanceFromPlayer = FVector::Dist(HookProjectileLocation, OwnerPlayer->GetActorLocation());
		if (DistanceFromPlayer > MaxHookDistance)
		{
			PRINTLOG(TEXT("UHookSystem: Hook exceeded max distance"));
			ReleaseHook();
		}
	}

	// 디버그 표시
	if (bShowDebugInfo)
	{
		DrawDebugSphere(
			GetWorld(),
			HookProjectileLocation,
			20.0f,
			8,
			FColor::Orange,
			false,
			0.0f,
			0,
			2.0f
		);
	}
}

void UHookSystem::UpdateHookPulling(float DeltaTime)
{
	// 안전 체크: 대상이 유효하지 않으면 자동 해제
	if (!HookedTarget || !IsValid(HookedTarget))
	{
		PRINTLOG(TEXT("UHookSystem: Hooked target became invalid, releasing hook"));
		ReleaseHook();
		return;
	}

	if (!OwnerPlayer)
	{
		ReleaseHook();
		return;
	}

	// 서버 권한 체크 - Actor 위치 변경은 서버에서만
	if (!OwnerPlayer->HasAuthority())
	{
		return;
	}

	// 플레이어 앞 목표 위치 계산
	FVector PlayerLocation = OwnerPlayer->GetActorLocation();
	FVector PlayerForward = OwnerPlayer->GetActorForwardVector();
	FVector TargetLocation = PlayerLocation + (PlayerForward * DesiredDistance);

	// 현재 대상 위치
	FVector CurrentLocation = HookedTarget->GetActorLocation();

	// 목표 위치까지의 거리 확인
	float DistanceToTarget = FVector::Dist(CurrentLocation, TargetLocation);

	// 너무 가까우면 Hook 완료
	if (DistanceToTarget < CompleteThreshold)
	{
		PRINTLOG(TEXT("UHookSystem: Hook completed, target reached destination"));
		ReleaseHook();
		return;
	}

	// FMath::VInterpTo로 부드럽게 이동
	FVector NewLocation = FMath::VInterpTo(
		CurrentLocation,
		TargetLocation,
		DeltaTime,
		HookSpeed
	);

	// 위치 업데이트 (서버에서만 실행됨)
	HookedTarget->SetActorLocation(NewLocation);

	// 디버그 표시
	if (bShowDebugInfo)
	{
		DrawDebugLine(
			GetWorld(),
			CurrentLocation,
			TargetLocation,
			FColor::Green,
			false,
			0.0f,
			0,
			2.0f
		);

		DrawDebugSphere(
			GetWorld(),
			TargetLocation,
			50.0f,
			12,
			FColor::Green,
			false,
			0.0f,
			0,
			1.0f
		);
	}
}

void UHookSystem::UpdateCable()
{
	if (!OwnerPlayer)
		return;

	// Cable 시작 위치 (플레이어)
	FVector CableStart = OwnerPlayer->GetActorLocation();

	// Cable 끝 위치
	FVector CableEnd;
	if (HookState == EHookState::Launching)
	{
		CableEnd = HookProjectileLocation;
	}
	else if (HookState == EHookState::Pulling && HookedTarget)
	{
		CableEnd = HookedTarget->GetActorLocation();
	}
	else
	{
		if (CableMesh)
		{
			CableMesh->SetVisibility(false);
		}
		return;
	}

	// Cable Component 업데이트
	if (CableMesh)
	{
		UpdateCableMeshTransform(CableStart, CableEnd);
	}

	// Projectile Mesh 위치 업데이트
	if (ProjectileMesh)
	{
		ProjectileMesh->SetWorldLocation(CableEnd);
	}

	// 항상 보이는 Debug Line 추가
	DrawDebugLine(
		GetWorld(),
		CableStart,
		CableEnd,
		FColor::Cyan,
		false,
		0.0f,
		0,
		3.0f
	);

	// 발사체 위치 표시 (Launching 중)
	if (HookState == EHookState::Launching)
	{
		DrawDebugSphere(
			GetWorld(),
			HookProjectileLocation,
			15.0f,
			8,
			FColor::Red,
			false,
			0.0f,
			0,
			2.0f
		);
	}
}

void UHookSystem::UpdateCableMeshTransform(const FVector& CableStart, const FVector& CableEnd)
{
	if (!CableMesh)
	{
		return;
	}

	FVector CableDelta = CableEnd - CableStart;
	float CableLength = CableDelta.Size();
	if (CableLength <= KINDA_SMALL_NUMBER)
	{
		CableMesh->SetVisibility(false);
		return;
	}

	FVector CableMidpoint = CableStart + (CableDelta * 0.5f);
	CableMesh->SetWorldLocation(CableMidpoint);

	FVector Direction = CableDelta / CableLength;
	FRotator CableRotation = FRotationMatrix::MakeFromZ(Direction).Rotator();
	CableMesh->SetWorldRotation(CableRotation);

	const float LengthScale = CableLength / FMath::Max(CableMeshBaseLength, KINDA_SMALL_NUMBER);
	const float TargetThickness = FMath::Max(CableThickness, 1.0f);
	const float MeshDiameter = FMath::Max(CableMeshBaseRadius * 2.0f, KINDA_SMALL_NUMBER);
	const float RadiusScale = TargetThickness / MeshDiameter;

	CableMesh->SetWorldScale3D(FVector(RadiusScale, RadiusScale, LengthScale));
	CableMesh->SetVisibility(true);
}

void UHookSystem::OnRep_HookState()
{
	// HookState가 변경되면 Visual 업데이트
	switch (HookState)
	{
		case EHookState::Idle:
			// Hook이 해제되면 Cable과 Projectile 숨김
			if (CableMesh)
			{
				CableMesh->SetVisibility(false);
			}
			if (ProjectileMesh)
			{
				ProjectileMesh->SetVisibility(false);
			}
			break;

		case EHookState::Launching:
		case EHookState::Pulling:
			// Hook이 시작되면 Cable과 Projectile 표시
			if (CableMesh)
			{
				CableMesh->SetVisibility(true);
			}
			if (ProjectileMesh)
			{
				ProjectileMesh->SetVisibility(true);
			}
			break;
	}

	PRINTLOG(TEXT("UHookSystem: OnRep_HookState - State changed to %d"), (int32)HookState);
}

void UHookSystem::OnRep_HookProjectileLocation()
{
	// HookProjectileLocation이 변경되면 Projectile Mesh 위치 업데이트
	if (ProjectileMesh && HookState == EHookState::Launching)
	{
		ProjectileMesh->SetWorldLocation(HookProjectileLocation);
	}
}
