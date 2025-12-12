// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "AWheatly.h"

#include "ALingoPlayerState.h"
#include "APlayerActor.h"
#include "APlayerControl.h"
#include "ASpeakStageActor.h"
#include "FComponentHelper.h"
#include "GameLogging.h"
#include "UInteractWidget.h"
#include "ULingoGameHelper.h"
#include "UKLingoNetworkSystem.h"
#include "UBroadcastManager.h"
#include "GameFramework/PlayerState.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "InteractableComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h" // Added for DrawDebugLine

#define WHEATLY_MESH_PATH		TEXT("/Game/CustomContents/Platfrom/Assets/Wheatly_Talk/Wheatly_Talk")
#define WHEATLY_MATERIAL_0		TEXT("/Script/Engine.Material'/Game/CustomContents/Platfrom/Assets/Wheatly_Talk/M_Wheatly_01.M_Wheatly_01'")
#define WHEATLY_MATERIAL_1		TEXT("/Script/Engine.Material'/Game/CustomContents/Platfrom/Assets/Wheatly_Talk/M_Wheatly_02.M_Wheatly_02'")
#define WHEATLY_MATERIAL_2		TEXT("/Script/Engine.Material'/Game/CustomContents/Platfrom/Assets/Wheatly_Talk/M_Wheatly_03.M_Wheatly_03'")

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

	PlayerDetectionZone = CreateDefaultSubobject<UBoxComponent>(TEXT("PlayerDetectionZone"));
	PlayerDetectionZone->SetupAttachment(RootComponent);
	PlayerDetectionZone->SetBoxExtent(FVector(1000.f, 1000.f, 100.f));
	PlayerDetectionZone->SetCollisionProfileName(TEXT("Trigger"));

	InteractingPlayerIndicator = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InteractingPlayerIndicator"));
	InteractingPlayerIndicator->SetupAttachment(RootComponent);
	
	//--------------------------------------------------------------
	// 스켈레탈 메시 로드
	//--------------------------------------------------------------
	MeshComponent->SetSkeletalMesh( FComponentHelper::LoadAsset<USkeletalMesh>(WHEATLY_MESH_PATH) );
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (CylinderMesh.Succeeded())
	{
		InteractingPlayerIndicator->SetStaticMesh(CylinderMesh.Object);
		InteractingPlayerIndicator->SetRelativeScale3D(FVector(0.5f, 0.5f, 2.0f));
	}
	InteractingPlayerIndicator->SetVisibility(false);
	InteractingPlayerIndicator->SetCollisionEnabled(ECollisionEnabled::NoCollision);


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

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	BoxComp->SetupAttachment(GetRootComponent());
	BoxComp->SetRelativeLocation(FVector(0.0f, 0.0f, 45.f));
	BoxComp->SetBoxExtent(FVector(32, 32, 45));
	BoxComp->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	BoxComp->SetCollisionObjectType(ECC_WorldStatic);
	BoxComp->SetCollisionResponseToAllChannels(ECR_Block);	
	
	// 초기값 설정
	CurAnimDuration = 0.0f;
}

void AWheatly::BeginPlay()
{
	Super::BeginPlay();

	if (baseMaterial && MeshComponent)
	{
		dynamicMaterial = UMaterialInstanceDynamic::Create(baseMaterial, this);
		MeshComponent->SetMaterial(2, dynamicMaterial);
	}

	if (InteractableComp)
	{
		InteractableComp->InitWidget(WidgetComp);

		InteractableComp->OnInteractionTriggered.RemoveDynamic(this, &AWheatly::OnInteractionTriggered);
		InteractableComp->OnInteractionTriggered.AddDynamic(this, &AWheatly::OnInteractionTriggered);
	}

	// SpeakStage 자동 연결 (서버에서만)
	if (HasAuthority() && !SpeakStage)
	{
		ASpeakStageActor* FoundStage = Cast<ASpeakStageActor>(
			UGameplayStatics::GetActorOfClass(GetWorld(), ASpeakStageActor::StaticClass())
		);

		if (FoundStage)
		{
			SetSpeakStage(FoundStage);
			PRINTLOG(TEXT("[AWheatly] SpeakStage auto-connected"));
		}
		else
		{
			PRINTLOG(TEXT("[AWheatly] Warning: No SpeakStageActor found in world"));
		}
	}

	PlayAnimation(EWheatlyAnim::Reaction_01);
}

void AWheatly::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!HasAuthority() || !SpeakStage)
		return;

	APlayerState* CurrentSpeaker = SpeakStage->GetCurrentSpeaker();
	
	// 퀘스트 진행 중일 때
	if (CurrentSpeaker)
	{
		if (InteractingPlayerIndicator)
			InteractingPlayerIndicator->SetVisibility(true);
		
		if (APawn* SpeakerPawn = CurrentSpeaker->GetPawn())
		{
			// 스피커를 쳐다보도록 회전
			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), SpeakerPawn->GetActorLocation());
			FRotator TargetRotation( LookAtRotation.Pitch, LookAtRotation.Yaw, 0); // Yaw만 사용
			SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaSeconds, 5.0f));
			
			// 표시기 위치 업데이트
			FVector IndicatorLocation = SpeakerPawn->GetActorLocation() - FVector(0,0,100.f); // 발 밑에 표시
			if (InteractingPlayerIndicator)
				InteractingPlayerIndicator->SetWorldLocation(IndicatorLocation);
		}
	}
	// 퀘스트 진행 중이 아닐 때
	else
	{
		if (InteractingPlayerIndicator)
			InteractingPlayerIndicator->SetVisibility(false);
		
		TArray<AActor*> OverlappingActors;
		if (PlayerDetectionZone)
		{
			PlayerDetectionZone->GetOverlappingActors(OverlappingActors, APlayerActor::StaticClass());
		}

		APawn* NearestPawn = nullptr;
		double MinDistanceSquared = MAX_dbl;

		for (AActor* OverlappingActor : OverlappingActors)
		{
			if (APawn* Pawn = Cast<APawn>(OverlappingActor))
			{
				double DistanceSquared = FVector::DistSquared(GetActorLocation(), Pawn->GetActorLocation());
				if (DistanceSquared < MinDistanceSquared)
				{
					MinDistanceSquared = DistanceSquared;
					NearestPawn = Pawn;
				}
			}
		}
		
		if(NearestPawn)
		{
			// LineTrace로 시야 확인
			FHitResult HitResult;
			FVector StartLocation = GetActorLocation();
			FVector EndLocation = NearestPawn->GetActorLocation();
			FCollisionQueryParams CollisionParams;
			CollisionParams.AddIgnoredActor(this); // 자기 자신은 무시

			bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, CollisionParams);

			// 아무것도 맞지 않았거나, 맞은 대상이 목표한 폰일 경우에만 시야가 확보된 것으로 간주
			if (!bHit || (bHit && HitResult.GetActor() == NearestPawn))
			{
				// 가장 가까운 플레이어를 쳐다보도록 회전
				FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), NearestPawn->GetActorLocation());
				FRotator TargetRotation( LookAtRotation.Pitch, LookAtRotation.Yaw, 0); // Yaw만 사용
				SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaSeconds, 2.0f));

				if (bShowDebugInfo)
					DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Green, false, 0.1f, 0, 2.0f);
			}
			else
			{
				if (bShowDebugInfo)
				{
					DrawDebugLine(GetWorld(), StartLocation, HitResult.Location, FColor::Red, false, 0.1f, 0, 2.0f);
					DrawDebugLine(GetWorld(), HitResult.Location, EndLocation, FColor::Yellow, false, 0.1f, 0, 2.0f); // 장애물 뒤는 노란색
				}
			}
		}
		else
		{
			if (bShowDebugInfo)
			{
				// 감지 영역 내 플레이어 없음
				FVector DrawEnd = GetActorLocation() + GetActorForwardVector() * 200.0f;
				DrawDebugLine(GetWorld(), GetActorLocation(), DrawEnd, FColor::Cyan, false, 0.1f, 0, 2.0f);
			}
		}
	}
}


//----------------------------------------------------------//
// Animation System
//----------------------------------------------------------//

void AWheatly::PlayAnimation(EWheatlyAnim InAnimType)
{
	SetAnimationType(InAnimType);

	// 서버에서만 Multicast 호출
	if (HasAuthority())
		Multicast_PlayAnimation(InAnimType);
}

void AWheatly::Multicast_PlayAnimation_Implementation(EWheatlyAnim InAnimType)
{
	// 애니메이션 타입 설정
	AnimType = InAnimType;

	if (!MeshComponent)
		return;

	UAnimSequence* AnimSeq = AnimSequences.FindRef(AnimType);
	if (!AnimSeq)
		return;

	// 애니메이션 직접 재생 (반복 없음)
	MeshComponent->PlayAnimation(AnimSeq, false);
	CurAnimDuration = AnimSeq->GetPlayLength();
}

void AWheatly::SetSpeakStage(ASpeakStageActor* InSpeakStage)
{
	// 기존 SpeakStage가 있다면 이벤트 바인딩 해제
	if (SpeakStage)
		SpeakStage->OnSpeakerChanged.RemoveDynamic(this, &AWheatly::OnSpeakStageSpeakerChanged);
	
	SpeakStage = InSpeakStage;

	// 새로운 SpeakStage에 이벤트 바인딩
	if (SpeakStage)
	{
		SpeakStage->OnSpeakerChanged.AddDynamic(this, &AWheatly::OnSpeakStageSpeakerChanged);
		// 초기 상태 동기화
		OnSpeakStageSpeakerChanged(SpeakStage->GetCurrentSpeaker());
	}
}

void AWheatly::BeginSpeakQuest(APlayerActor* Player)
{
	if (!HasAuthority() || !Player)
		return;

	RequestSpeakScenario(Player);
}


void AWheatly::CompleteSpeakQuest(APlayerActor* Player)
{
	if (!HasAuthority() || !Player)
		return;
	
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
	{
		KLingoNetwork->RequestSpeakScenario(FResponseSpeakScenarioDelegate::CreateUObject(this, &AWheatly::OnResponseSpeakScenario));
	}
}


void AWheatly::OnResponseSpeakScenario(FResponseSpeakScenario& ResponseData, bool bWasSuccessful)
{
	// 응답 델리게이트 생성
	if (bWasSuccessful)
	{
		// PlayerState에 데이터 저장
		if (auto PS = RequestPlayer->GetPlayerState<ALingoPlayerState>() )
		{
			// PlayerState에 시나리오 데이터 저장
			PS->SpeakScenarioData = ResponseData;
			PS->CurSpeakQuestStep = 0;

			// SpeakStage를 통해 퀘스트 시작 (첫 질문 표시, TTS 재생, UI 업데이트 포함)
			if (SpeakStage && PS->SpeakScenarioData.speak_quest_data.Num() > 0)
			{
				SpeakStage->StartStageForPlayer(PS);
				PRINTLOG(TEXT("[AWheatly] SpeakQuest started for: %s"), *ULingoGameHelper::GetPlayerNameFromState(PS));
			}		
		}
		else
		{
			// 실패 시 SpeakStage를 종료해야 할 수 있음
			if(SpeakStage && SpeakStage->GetCurrentSpeaker())
				SpeakStage->EndStage();
			return;
		}
	}
	else
	{
		// 실패 시 SpeakStage를 종료해야 할 수 있음
		if(SpeakStage && SpeakStage->GetCurrentSpeaker())
		{
			SpeakStage->EndStage();
		}
	}
}

//----------------------------------------------------------//
// Interaction System
//----------------------------------------------------------//

void AWheatly::OnInteractionTriggered(AActor* InteractingActor)
{
	if (!HasAuthority() || !SpeakStage)
		return;

	APlayerActor* InteractingPlayer = Cast<APlayerActor>(InteractingActor);
	if (!InteractingPlayer)
		return;

	// SpeakStage의 상태를 직접 확인
	if (APlayerState* CurrentSpeaker = SpeakStage->GetCurrentSpeaker())
	{
		if (auto PC = Cast<APlayerControl>(InteractingPlayer->GetController()))
		{
			PC->Client_ToastMessage(FString::Printf(TEXT("Current Turn is [%s]"), *ULingoGameHelper::GetPlayerNameFromState(CurrentSpeaker)));
		}
		return;
	}

	// SpeakQuest 시작 (시나리오 데이터 요청)
	BeginSpeakQuest(InteractingPlayer);
}

void AWheatly::OnSpeakStageSpeakerChanged(APlayerState* NewSpeaker)
{
	const bool bIsStageBusy = NewSpeaker != nullptr;
	FLinearColor newColor = bIsStageBusy
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
