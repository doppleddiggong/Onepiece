// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "AWheatly.h"

#include "ALingoPlayerState.h"
#include "APlayerActor.h"
#include "APlayerControl.h"
#include "ASpeakStageActor.h"
#include "FComponentHelper.h"
#include "GameLogging.h"
#include "UInteractWidget.h"
#include "UKLingoNetworkSystem.h"
#include "UBroadcastManager.h"

#include "InteractableComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"

#define WHEATLY_MESH_PATH		TEXT("/Game/CustomContents/Platfrom/Assets/Wheatly_Talk/Wheatly_Talk")
#define WHEATLY_MATERIAL_0		TEXT("/Script/Engine.Material'/Game/CustomContents/Platfrom/Assets/Wheatly_Talk/M_Wheatly_01.M_Wheatly_01'")
#define WHEATLY_MATERIAL_1		TEXT("/Script/Engine.Material'/Game/CustomContents/Platfrom/Assets/Wheatly_Talk/M_Wheatly_02.M_Wheatly_02'")
#define WHEATLY_MATERIAL_2		TEXT("/Script/Engine.Material'/Game/CustomContents/Platfrom/Assets/Wheatly_Talk/M_Wheatly_03.M_Wheatly_03'")

class UInteractWidget;

AWheatly::AWheatly()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	//--------------------------------------------------------------
	// Skeletal Mesh Component 생성
	//--------------------------------------------------------------
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	
	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	MeshComponent->SetupAttachment(RootComponent);

	//--------------------------------------------------------------
	// 스켈레탈 메시 로드
	//--------------------------------------------------------------
	MeshComponent->SetSkeletalMesh( FComponentHelper::LoadAsset<USkeletalMesh>(WHEATLY_MESH_PATH) );

	//--------------------------------------------------------------
	// 머티리얼 로드 (0번, 1번, 2번)
	//--------------------------------------------------------------
	baseMaterial = FComponentHelper::LoadAsset<UMaterialInterface>(WHEATLY_MATERIAL_2);  // 2번은 눈 (동적 머티리얼용)

	MeshComponent->SetMaterial(0, FComponentHelper::LoadAsset<UMaterialInterface>(WHEATLY_MATERIAL_0));
	MeshComponent->SetMaterial(1, FComponentHelper::LoadAsset<UMaterialInterface>(WHEATLY_MATERIAL_1));
	MeshComponent->SetMaterial(2, baseMaterial);

	// InteractableComponent 생성
	InteractableComp = CreateDefaultSubobject<UInteractableComponent>(TEXT("Interactable"));
	InteractableComp->DetectionDistance = 500.0f;
	InteractableComp->InteractionType = EInteractionType::Button;
	InteractableComp->InteractionPrompt = TEXT("Talk to Activate");

	WidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComp"));
	ConstructorHelpers::FClassFinder<UInteractWidget> WidgetRef(INTERACT_WIDGET_PATH);
	WidgetComp->SetWidgetClass(WidgetRef.Class);
	WidgetComp->SetupAttachment(GetRootComponent());
	WidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
	WidgetComp->SetDrawSize(FVector2D(2048.0f, 1024.0f));
	
	// 초기값 설정
	targetPlayer = nullptr;
	bIsBusy = false;
	busyPlayerName = TEXT("");
	currentAnimDuration = 0.0f;
}

void AWheatly::BeginPlay()
{
	Super::BeginPlay();

	if (baseMaterial && MeshComponent)
	{
		dynamicMaterial = UMaterialInstanceDynamic::Create(baseMaterial, this);
		MeshComponent->SetMaterial(2, dynamicMaterial);

		// 초기 눈 색상 설정 (파란색 - Available 상태)
		ChangeEyeColor(FLinearColor(0.0f, 0.5f, 1.0f, 1.0f));
	}

	if (InteractableComp)
	{
		InteractableComp->InitWidget(WidgetComp);

		InteractableComp->OnInteractionTriggered.RemoveDynamic(this, &AWheatly::OnInteractionTriggered);
		InteractableComp->OnInteractionTriggered.AddDynamic(this, &AWheatly::OnInteractionTriggered);
	}

	PlayAnimation(EWheatlyAnim::Reaction_01);
}

//----------------------------------------------------------//
// Animation System
//----------------------------------------------------------//

void AWheatly::PlayAnimation(EWheatlyAnim InAnimType)
{
	SetAnimationType(InAnimType);

	// 서버에서만 Multicast 호출
	if (HasAuthority())
	{
		Multicast_PlayAnimation(InAnimType);
	}
}

void AWheatly::Multicast_PlayAnimation_Implementation(EWheatlyAnim InAnimType)
{
	// 애니메이션 타입 설정
	AnimType = InAnimType;

	if (!MeshComponent)
	{
		PRINTLOG(TEXT("[AWheatly] Multicast_PlayAnimation - MeshComponent is null"));
		return;
	}

	UAnimSequence* AnimSeq = AnimSequences.FindRef(AnimType);
	if (!AnimSeq)
	{
		PRINTLOG(TEXT("[AWheatly] Multicast_PlayAnimation - AnimSequence not found for type: %d"), static_cast<int32>(AnimType));
		return;
	}

	// 애니메이션 직접 재생 (반복 없음)
	MeshComponent->PlayAnimation(AnimSeq, false);
	currentAnimDuration = AnimSeq->GetPlayLength();

	PRINTLOG(TEXT("[AWheatly] Animation played: %d (Duration: %.2f)"), static_cast<int32>(AnimType), currentAnimDuration);
}


//----------------------------------------------------------//
// Replication
//----------------------------------------------------------//

void AWheatly::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWheatly, bIsBusy);
	DOREPLIFETIME(AWheatly, busyPlayerName);
}

//----------------------------------------------------------//
// Speak Stage System
//----------------------------------------------------------//

void AWheatly::SetSpeakStage(ASpeakStageActor* InSpeakStage)
{
	SpeakStage = InSpeakStage;

	PRINTLOG(TEXT("[AWheatly] SpeakStage connected: %s"),
		SpeakStage ? TEXT("Success") : TEXT("Failed"));
}

FString AWheatly::GetCurrentQuestion() const
{
	if (SpeakStage)
	{
		return SpeakStage->GetCurrentQuestion();
	}

	return TEXT("");
}

void AWheatly::BeginSpeakQuest(APlayerActor* Player)
{
	if (!HasAuthority())
		return;

	if (!Player)
	{
		bIsBusy = false;
		busyPlayerName = TEXT("");
		return;
	}

	RequestSpeakScenario(Player);
}


void AWheatly::CompleteSpeakQuest(APlayerActor* Player)
{
	if (!HasAuthority())
		return;

	if (!Player)
		return;

	// Busy 상태 해제
	bIsBusy = false;
	busyPlayerName = TEXT("");

	// PlayerState에서 축적된 평가 결과 가져오기
	ALingoPlayerState* PS = Player->GetPlayerState<ALingoPlayerState>();
	if (!PS)
		return;

	if (UBroadcastManager* BroadcastMgr = UBroadcastManager::Get(GetWorld()))
		BroadcastMgr->SendTutorMessage(FText::FromString("SPEAK COMPLETE"));
}

void AWheatly::RequestSpeakScenario(APlayerActor* Player)
{
	this->RequestPlayer = Player;
	
	if ( auto KLingoNetwork = UKLingoNetworkSystem::Get(GetWorld()) )
		KLingoNetwork->RequestSpeakScenario(FResponseSpeakScenarioDelegate::CreateUObject(this, &AWheatly::OnResponseSpeakScenario));
}


void AWheatly::OnResponseSpeakScenario(FResponseSpeakScenario& ResponseData, bool bWasSuccessful)
{
	// 응답 델리게이트 생성
	if (bWasSuccessful)
	{
		// PlayerState에 데이터 저장
		ALingoPlayerState* PS = RequestPlayer->GetPlayerState<ALingoPlayerState>();
		if (!PS)
		{
			bIsBusy = false;
			busyPlayerName = TEXT("");
			return;
		}

		PS->SpeakScenarioData = ResponseData;
		PS->CurSpeakQuestStep = 0;

		// 질문을 플레이어에게 전달
		if (PS->SpeakScenarioData.speak_quest_data.Num() > 0)
		{
			if (APlayerControl* PC = Cast<APlayerControl>(RequestPlayer->GetController()))
			{
				FSpeakStageQuestion& StageQuestion = PS->SpeakScenarioData.speak_quest_data[0];
				PC->Client_ToastMessage(*StageQuestion.GetQuestionMessage());

				RequestPlayer->PlayTTSAudio( StageQuestion.voice_data );
			}
		}
	}
	else
	{
		// 실패 시 Busy 상태 해제
		bIsBusy = false;
		busyPlayerName = TEXT("");
	}
}

//----------------------------------------------------------//
// Interaction System
//----------------------------------------------------------//

void AWheatly::OnInteractionTriggered(AActor* InteractingActor)
{
	if (!HasAuthority())
		return;

	APlayerActor* InteractingPlayer = Cast<APlayerActor>(InteractingActor);
	if (!InteractingPlayer)
		return;

	// bIsBusy가 true이면 토스트 메시지 표시
	if (bIsBusy)
	{
		if (auto PC = Cast<APlayerControl>(InteractingPlayer->GetController()))
			PC->Client_ToastMessage( FString::Printf(TEXT("Current Turn is [%s]"), *busyPlayerName) );

		return;
	}

	// bIsBusy가 false이면 SpeakQuest 시작
	if (auto PS = InteractingPlayer->GetPlayerState() )
	{
		// Busy 상태로 전환
		bIsBusy = true;
		busyPlayerName = PS->GetPlayerName();

		// SpeakQuest 시작
		BeginSpeakQuest(InteractingPlayer);
	}
}

void AWheatly::OnRep_bIsBusy()
{
	// Busy 상태에 따라 눈 색상 변경
	FLinearColor newColor = bIsBusy
		? FLinearColor(1.0f, 1.0f, 0.0f, 1.0f)  // Yellow: Busy
		: FLinearColor(0.0f, 0.5f, 1.0f, 1.0f); // Blue: Available

	ChangeEyeColor(newColor);
}

//----------------------------------------------------------//
// Visual System
//----------------------------------------------------------//

void AWheatly::ChangeEyeColor(FLinearColor newColor)
{
	if (!dynamicMaterial)
		return;

	dynamicMaterial->SetVectorParameterValue(TEXT("EyeColor"), newColor);
}