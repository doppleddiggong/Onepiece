// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "ATeleportTrigger.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"
#include "APlayerActor.h"
#include "ATeleportOut.h"
#include "ANetworkBroadcastActor.h"
#include "GameFramework/PlayerController.h"
#include "GameLogging.h"


ATeleportTrigger::ATeleportTrigger()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	// 루트 컴포넌트로 BoxComponent 생성
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;

	// 박스 크기 기본값 설정
	TriggerBox->SetBoxExtent(FVector(100.0f, 100.0f, 100.0f));

	// Overlap 이벤트 활성화
	TriggerBox->SetGenerateOverlapEvents(true);
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	// 초기값 설정
	bIsOneShot = true;
	bIsTriggered = false;
	bShowDebugBox = true;
	DebugBoxColor = FColor::Green;
}

void ATeleportTrigger::BeginPlay()
{
	Super::BeginPlay();

	if (TriggerBox)
		TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ATeleportTrigger::OnTriggerBeginOverlap);
}

void ATeleportTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bShowDebugBox && TriggerBox)
	{
		FVector BoxCenter = TriggerBox->GetComponentLocation();
		FVector BoxExtent = TriggerBox->GetScaledBoxExtent();
		FRotator BoxRotation = TriggerBox->GetComponentRotation();

		// 디버그 박스 표시 (트리거 활성화 상태일 때만)
		if (!bIsTriggered)
		{
			DrawDebugBox(
				GetWorld(),
				BoxCenter,
				BoxExtent,
				BoxRotation.Quaternion(),
				DebugBoxColor,
				false,
				-1.0f,
				0,
				2.0f
			);
		}

		// 트리거 정보를 텍스트로 표시
		FString StatusText = bIsTriggered ? TEXT("[TRIGGERED]") : TEXT("[ACTIVE]");
		FColor TextColor = bIsTriggered ? FColor::Red : FColor::Green;

		FVector TextLocation = BoxCenter + FVector(0.0f, 0.0f, BoxExtent.Z + 50.0f);

		// 상태 표시
		DrawDebugString(
			GetWorld(),
			TextLocation,
			StatusText,
			nullptr,
			TextColor,
			0.0f,
			true,
			1.2f
		);
	}
}

void ATeleportTrigger::OnTriggerBeginOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	// 서버에서만 처리
	if (!HasAuthority())
		return;

	// 원샷 모드이고 이미 트리거되었으면 무시
	if (bIsOneShot && bIsTriggered)
		return;

	// PlayerActor인지 확인
	APlayerActor* PlayerActor = Cast<APlayerActor>(OtherActor);
	if (PlayerActor)
	{
		// TeleportOut이 설정되어 있는지 확인
		if (!TeleportOut)
		{
			PRINTLOG(TEXT("ATeleportTrigger: TeleportOut이 설정되지 않았습니다!"));
			return;
		}

		// TeleportOut의 위치 가져오기
		FVector TargetLocation = TeleportOut->GetActorLocation();

		// NetworkBroadcastActor를 통해 모든 플레이어 텔레포트
		if (ANetworkBroadcastActor* BroadcastActor = ANetworkBroadcastActor::Get(this))
		{
			BroadcastActor->SendTeleportAllPlayers(TargetLocation, this);
			PRINTLOG( TEXT("텔레포트 요청: %s"), *TargetLocation.ToString());
		}
		else
		{
			PRINTLOG(TEXT("ATeleportTrigger: NetworkBroadcastActor를 찾을 수 없습니다!"));
		}

		OnActivate();
		
		// 원샷 모드일 때만 트리거 상태 변경
		if (bIsOneShot)
			bIsTriggered = true;
	}
}

void ATeleportTrigger::OnActivate_Implementation()
{
	PRINT_STRING(TEXT("ABroadcastTrigger Activated"));
}

