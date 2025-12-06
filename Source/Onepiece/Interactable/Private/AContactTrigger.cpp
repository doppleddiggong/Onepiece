// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "AContactTrigger.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"
#include "APlayerActor.h"
#include "GameFramework/PlayerController.h"
#include "UKLingoNetworkSystem.h"
#include "NetworkData.h"
#include "ALingoGameMode.h"
#include "GameLogging.h"
#include "LuggageManager.h"
#include "ULingoGameHelper.h"
#include "UVoiceConversationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "AHolder.h"
#include "EngineUtils.h"

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

	VoiceConversationSystem = CreateDefaultSubobject<UVoiceConversationSystem>(TEXT("VoiceConversationSystem"));

	QuestType = EQuestType::Read;
}

void AContactTrigger::BeginPlay()
{
	Super::BeginPlay();

	EventMessage = ULingoGameHelper::GetStageStartMessage(QuestType);
	
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

	this->OnTriggerScenario(QuestType);
}


void AContactTrigger::OnTriggerScenario(EQuestType InQuestType)
{
	if (auto KLingoNetwork = UKLingoNetworkSystem::Get(GetWorld()))
	{
		switch (InQuestType)
		{
			case EQuestType::Read:
				KLingoNetwork->RequestReadScenario( FResponseReadScenarioDelegate::CreateUObject(this, &AContactTrigger::OnReadResponseScenario));
				break;

			case EQuestType::Listen: // 듣기
				KLingoNetwork->RequestListenScenario( FResponseListenScenarioDelegate::CreateUObject(this, &AContactTrigger::OnListenResponseScenario));
				break;
		}
	}
}

void AContactTrigger::OnReadResponseScenario(FResponseReadScenario& ResponseData, bool bWasSuccessful)
{
	if (!bWasSuccessful)
		return;

	// ALingoGameState에 시나리오 데이터 전체 저장
	if (UWorld* World = GetWorld())
	{
		if (auto GM = ULingoGameHelper::GetLingoGameMode(World))
			GM->BeginReadQuest( ResponseData);

		ALuggageManager* LuggageManager = Cast<ALuggageManager>( UGameplayStatics::GetActorOfClass(World, ALuggageManager::StaticClass()));

		if (LuggageManager)
			LuggageManager->StartSpawning();

		TEST_Holder(ResponseData);
	}
}

void AContactTrigger::OnListenResponseScenario(FResponseListenScenario& ResponseData, bool bWasSuccessful)
{
	if (!bWasSuccessful)
		return;

	if (UWorld* World = GetWorld())
	{
		if (auto GM = ULingoGameHelper::GetLingoGameMode(World))
			GM->BeginListenQuest(ResponseData);

		VoiceConversationSystem->PlayVoiceAudio(ResponseData.voice_data);
	}
}

void AContactTrigger::TEST_Holder(FResponseReadScenario& ResponseData)
{
	// HACK, 임시 코드
	// 맵에 있는 AHolder를 찾아서 정답 데이터 설정
	if (ResponseData.target_data.Num() > 0 && ResponseData.correct_answer_index >= 0
		&& ResponseData.correct_answer_index < ResponseData.target_data.Num())
	{
		const FScenarioTargetData& CorrectAnswer = ResponseData.target_data[ResponseData.correct_answer_index];

		// {
		// 	"word1": { "name": "닭", "code": "6" },
		// 	"word2": { "name": "빨강", "code": "1" }
		// }
			
		const int32 PatternIdx = FCString::Atoi(*CorrectAnswer.word1.code);
		const int32 ColorIdx = FCString::Atoi(*CorrectAnswer.word2.code);

		for (TActorIterator<AHolder> It(GetWorld()); It; ++It)
		{
			AHolder* Holder = *It;
			if (Holder)
			{
				Holder->SetAnswerData(ColorIdx, PatternIdx);
				break;
			}
		}
	}
}
