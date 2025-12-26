// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "AEvaluationTrigger.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"
#include "APlayerActor.h"
#include "GameFramework/PlayerController.h"
#include "GameLogging.h"
#include "UKLingoNetworkSystem.h"
#include "ULingoGameHelper.h"
#include "UPopupManager.h"
#include "UPopup_Evaluation.h"

AEvaluationTrigger::AEvaluationTrigger()
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
	bIsTriggered = false;
	bShowDebugBox = true;
	DebugBoxColor = FColor::Green;
	MarkerType = ECompassMarkerType::FinalResult;
	
}

void AEvaluationTrigger::BeginPlay()
{
	Super::BeginPlay();

	if (TriggerBox)
		TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AEvaluationTrigger::OnTriggerBeginOverlap);
}

void AEvaluationTrigger::Tick(float DeltaTime)
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

void AEvaluationTrigger::OnTriggerBeginOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	// 서버에서만 처리
	if (!HasAuthority())
		return;

	// 원샷 모드이고 이미 트리거되었으면 무시
	if ( bIsTriggered)
		return;

	// PlayerActor인지 확인
	APlayerActor* PlayerActor = Cast<APlayerActor>(OtherActor);
	if (PlayerActor)
	{
		OnActivate();
		
		bIsTriggered = true;
	}
}

void AEvaluationTrigger::OnActivate_Implementation()
{
	if (auto KLingoNetwork = UKLingoNetworkSystem::Get(GetWorld()))
	{
		if (auto GS = ULingoGameHelper::GetLingoGameState(GetWorld()))
		{
			KLingoNetwork->RequestEvaluationResult(
				GS->GetRoomId(),
				FResponseEvaluationResultDelegate::CreateUObject(this, &AEvaluationTrigger::OnResponseEvaluationResult)
			);
		}
	}
	else
	{
		PRINTLOG(TEXT("UKLingoNetworkSystem not found!"));
	}
}

void AEvaluationTrigger::OnResponseEvaluationResult(FResponseEvaluationResult& ResponseData, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		PRINTLOG(TEXT("--- Evaluation Result SUCCESS ---"));

		if (auto Popup = UPopupManager::ShowPopupAs<UPopup_Evaluation>( GetWorld(), EPopupType::Evaluation))
			Popup->InitPopup(ResponseData);
	}
	else
	{
		PRINTLOG(TEXT("--- OCR Extract FAILED ---"));
	}
}