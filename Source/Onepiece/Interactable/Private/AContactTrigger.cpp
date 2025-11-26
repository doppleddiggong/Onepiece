// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "AContactTrigger.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"
#include "APlayerActor.h"
#include "GameFramework/PlayerController.h"
#include "UKLingoNetworkSystem.h"
#include "NetworkData.h"
#include "ALingoGameState.h"
#include "GameLogging.h"
#include "LuggageManager.h"
#include "ULingoGameHelper.h"
#include "Kismet/GameplayStatics.h"

AContactTrigger::AContactTrigger()
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

	StageIndex = 1;
}

void AContactTrigger::BeginPlay()
{
	Super::BeginPlay();

	EventMessage = ULingoGameHelper::GetStageStartMessage(StageIndex);
	
	// Overlap 이벤트 바인딩
	if (TriggerBox)
		TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AContactTrigger::OnTriggerBeginOverlap);
}

void AContactTrigger::Tick(float DeltaTime)
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

		// 이벤트 메시지 표시
		FVector MessageLocation = TextLocation + FVector(0.0f, 0.0f, 30.0f);
		DrawDebugString(
			GetWorld(),
			MessageLocation,
			FString::Printf(TEXT("Message: %s"), *EventMessage),
			nullptr,
			FColor::White,
			0.0f,
			true,
			1.0f
		);
	}
}

void AContactTrigger::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 이미 트리거되었으면 무시
	if (bIsTriggered)
		return;

	// PlayerActor인지 확인
	APlayerActor* PlayerActor = Cast<APlayerActor>(OtherActor);
	if (PlayerActor)
	{
		// 서버 RPC 호출
		ServerRPC_OnTrigger(PlayerActor);
	}
}

void AContactTrigger::ServerRPC_OnTrigger_Implementation(AActor* TriggeringActor)
{
	// 이미 트리거되었으면 무시
	if (bIsTriggered)
		return;

	// 트리거 비활성화
	bIsTriggered = true;

	// TODO, 나중에 분기 처리를 위해
	this->OnTriggerScenario(StageIndex);
}


void AContactTrigger::OnTriggerScenario(const int InStageIndex)
{
// 시나리오 데이터 요청
	if (UKLingoNetworkSystem* KLingoNetwork = UKLingoNetworkSystem::Get(GetWorld()))
	{
		PRINTLOG(TEXT("[ContactTrigger] Requesting Scenario - StageIndex: %d"), InStageIndex);

		// ScenarioStageIndex를 이용해 시나리오 데이터 요청
		// 파라미터: Index, Difficulty, Level (1: 한국어)
		KLingoNetwork->RequestScenario(
		1,
			StageIndex,
			1,  // Level
			FResponseScenarioDelegate::CreateUObject(this, &AContactTrigger::OnResponseScenario)
		);
	}
	else
	{
		PRINTLOG(TEXT("[ContactTrigger] ERROR: UKLingoNetworkSystem not found!"));
	}
}

void AContactTrigger::OnResponseScenario(FResponseScenario& ResponseData, bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		PRINTLOG(TEXT("[ContactTrigger] Scenario request FAILED!"));
		return;
	}

	PRINTLOG(TEXT("[ContactTrigger] Scenario request SUCCESS!"));
	ResponseData.PrintData();

	// ALingoGameState에 시나리오 데이터 전체 저장
	if (UWorld* World = GetWorld())
	{
		if (ALingoGameState* LingoGameState = World->GetGameState<ALingoGameState>())
		{
			// GameState에 시나리오 정보 저장 (서버에서만 실행되므로 자동으로 복제됨)
			LingoGameState->SetStageData(StageIndex, ResponseData);

			PRINTLOG(TEXT("[ContactTrigger] Saved scenario data & Started timer (180s) - StageIndex: %d, Index: %d, Difficulty: %d"),
				StageIndex, ResponseData.index, ResponseData.dificulity);
		}

		// 모든 플레이어에게 이벤트 메시지 전송
		for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
		{
			if (APlayerController* PC = It->Get())
			{
				if (APlayerActor* PlayerActor = Cast<APlayerActor>(PC->GetPawn()))
				{
					PlayerActor->OnGameMessage(EventMessage);
					PRINTLOG(TEXT("[ContactTrigger] Sent message to player: %s"), *EventMessage);
				}
			}
		}

		ALuggageManager* LuggageManager = Cast<ALuggageManager>(
			  UGameplayStatics::GetActorOfClass(World, ALuggageManager::StaticClass()));
		if (LuggageManager)
		{
			LuggageManager->StartSpawning();
		}
	}
}