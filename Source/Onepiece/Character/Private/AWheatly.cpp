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
#include "Net/UnrealNetwork.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "InteractableComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#define WHEATLY_MESH_PATH		TEXT("/Game/CustomContents/Platfrom/Assets/Wheatly_Talk/Wheatly_Talk")
#define WHEATLY_MATERIAL_0		TEXT("/Script/Engine.Material'/Game/CustomContents/Platfrom/Assets/Wheatly_Talk/M_Wheatly_01.M_Wheatly_01'")
#define WHEATLY_MATERIAL_1		TEXT("/Script/Engine.Material'/Game/CustomContents/Platfrom/Assets/Wheatly_Talk/M_Wheatly_02.M_Wheatly_02'")
#define WHEATLY_MATERIAL_2		TEXT("/Script/Engine.Material'/Game/CustomContents/Platfrom/Assets/Wheatly_Talk/M_Wheatly_03.M_Wheatly_03'")

AWheatly::AWheatly()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);
	
	//--------------------------------------------------------------
	// Skeletal Mesh Component 생성
	//--------------------------------------------------------------
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	
	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));

	EyeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EyeMesh"));
	EyeMesh->SetupAttachment(MeshComponent, TEXT("eyelight_aimjoint"));
	
	PlayerDetectionZone = CreateDefaultSubobject<UBoxComponent>(TEXT("PlayerDetectionZone"));
	PlayerDetectionZone->SetupAttachment(RootComponent);
	PlayerDetectionZone->SetBoxExtent(FVector(1000.f, 1000.f, 1000.f));
	PlayerDetectionZone->SetCollisionProfileName(TEXT("Trigger"));

	EyeSightComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EyeSightComp"));
	EyeSightComp->SetupAttachment(RootComponent);
	
	//--------------------------------------------------------------
	// 스켈레탈 메시 로드
	//--------------------------------------------------------------
	MeshComponent->SetSkeletalMesh( FComponentHelper::LoadAsset<USkeletalMesh>(WHEATLY_MESH_PATH) );
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (CylinderMesh.Succeeded())
	{
		UStaticMesh* Mesh = CylinderMesh.Object;
		EyeSightComp->SetStaticMesh(Mesh);
		
		if (Mesh)
		{
			IndicatorBaseLength = FMath::Max(Mesh->GetBounds().BoxExtent.Z * 2.0f, 1.0f);
			IndicatorBaseRadius = FMath::Max(Mesh->GetBounds().BoxExtent.X, Mesh->GetBounds().BoxExtent.Y);
		}
	}
	EyeSightComp->SetVisibility(false);
	EyeSightComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	MeshComponent->SetMaterial(0, FComponentHelper::LoadAsset<UMaterialInterface>(WHEATLY_MATERIAL_0));
	MeshComponent->SetMaterial(1, FComponentHelper::LoadAsset<UMaterialInterface>(WHEATLY_MATERIAL_1));
	MeshComponent->SetMaterial(2, FComponentHelper::LoadAsset<UMaterialInterface>(WHEATLY_MATERIAL_2));

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
	CurAnimDuration = 0.0f;
}

void AWheatly::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWheatly, ReplicatedEyeColor);
	DOREPLIFETIME(AWheatly, ReplicatedEyeSightEnd);
	DOREPLIFETIME(AWheatly, bEyeSightVisible);
}

void AWheatly::BeginPlay()
{
	Super::BeginPlay();

	if (MeshComponent)
	{
		UMaterialInterface* BaseMaterial = MeshComponent->GetMaterial(2);
		
		EyeMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this);
		MeshComponent->SetMaterial(2, EyeMaterial);
	}

	if (EyeSightComp)
	{
		UMaterialInterface* BaseMaterial = EyeSightComp->GetMaterial(0);
		
		EyeTraceMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this);
		EyeSightComp->SetMaterial(0, EyeTraceMaterial);
	}
	
	if (InteractableComp)
	{
		InteractableComp->InitWidget(WidgetComp);

		InteractableComp->OnInteractionTriggered.RemoveDynamic(this, &AWheatly::OnInteractionTriggered);
		InteractableComp->OnInteractionTriggered.AddDynamic(this, &AWheatly::OnInteractionTriggered);
		InteractableComp->OnOutlineStateChanged.AddDynamic(this, &AWheatly::OnOutlineStateChanged);
	}

	// SpeakStage 자동 연결 (서버에서만)
	if (HasAuthority() && !SpeakStageActor)
	{
		ASpeakStageActor* FoundStage = Cast<ASpeakStageActor>(
			UGameplayStatics::GetActorOfClass(GetWorld(), ASpeakStageActor::StaticClass())
		);

		if (FoundStage)
		{
			SetSpeakStageActor(FoundStage);
			PRINTLOG(TEXT("[AWheatly] SpeakStage auto-connected"));
		}
		else
		{
			PRINTLOG(TEXT("[AWheatly] Warning: No SpeakStageActor found in world"));
		}
	}

	PlayAnimation(EWheatlyAnim::PowerOn);
}

void AWheatly::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!SpeakStageActor)
		return;

	// 퀘스트 진행 중일 때
	if (auto CurrentSpeaker = SpeakStageActor->GetCurrentSpeaker())
	{
		if (HasAuthority())
		{
			if (auto SpeakerPawn = CurrentSpeaker->GetPawn())
			{
				ReplicatedEyeSightEnd = SpeakerPawn->GetActorLocation();
				bEyeSightVisible = true;

				ApplyEyeSight();
				
				// 스피커를 쳐다보도록 회전
				FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), SpeakerPawn->GetActorLocation());
				FRotator TargetRotation( LookAtRotation.Pitch, LookAtRotation.Yaw, 0); // Yaw만 사용
				SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaSeconds, 5.0f));
			}
		}
	}
	// 퀘스트 진행 중이 아닐 때
	else
	{
		if (HasAuthority())
		{
			bEyeSightVisible = false;

			ApplyEyeSight();
		}
		
		TArray<AActor*> OverlappingActors;
		if (PlayerDetectionZone)
			PlayerDetectionZone->GetOverlappingActors(OverlappingActors, APlayerActor::StaticClass());
		
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
			FVector StartLocation = EyeMesh->GetComponentLocation();
			FVector EndLocation = NearestPawn->GetActorLocation();
			FCollisionQueryParams CollisionParams;
			CollisionParams.AddIgnoredActor(this); // 자기 자신은 무시
		
			bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, CollisionParams);
		
			// 아무것도 맞지 않았거나, 맞은 대상이 목표한 폰일 경우에만 시야가 확보된 것으로 간주
			if (!bHit || (bHit && HitResult.GetActor() == NearestPawn))
			{
				// 가장 가까운 플레이어를 쳐다보도록 회전
				FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(EyeMesh->GetComponentLocation(), NearestPawn->GetActorLocation());
				FRotator TargetRotation( LookAtRotation.Pitch, LookAtRotation.Yaw, 0); // Yaw만 사용
				SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaSeconds, 2.0f));
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

void AWheatly::SetSpeakStageActor(ASpeakStageActor* InSpeakStageActor)
{
	// 기존 SpeakStage가 있다면 이벤트 바인딩 해제
	if (SpeakStageActor)
		SpeakStageActor->OnSpeakerChanged.RemoveDynamic(this, &AWheatly::OnSpeakStageSpeakerChanged);
	
	SpeakStageActor = InSpeakStageActor;

	// 새로운 SpeakStage에 이벤트 바인딩
	if (SpeakStageActor)
	{
		SpeakStageActor->OnSpeakerChanged.AddDynamic(this, &AWheatly::OnSpeakStageSpeakerChanged);
		// 초기 상태 동기화
		OnSpeakStageSpeakerChanged(SpeakStageActor->GetCurrentSpeaker());
	}
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

void AWheatly::SyncSpeakScenarioData(APlayerActor* Player, const FResponseSpeakScenario& Data)
{
	if (!HasAuthority())
		return;

	// Player가 유효한지 확인
	if (!Player)
	{
		PRINTLOG(TEXT("[AWheatly] SyncSpeakScenarioData: Player is null"));
		return;
	}

	// PlayerState에 데이터 저장
	if (auto PS = Player->GetPlayerState<ALingoPlayerState>())
	{
		PS->SpeakScenarioData = Data;
		PS->OnUpdateSpeakScenarioData();

		PRINTLOG(TEXT("[AWheatly] SyncSpeakScenarioData: Successfully synced scenario data for %s"),
			*ULingoGameHelper::GetPlayerNameFromState(PS));
	}
	else
	{
		PRINTLOG(TEXT("[AWheatly] SyncSpeakScenarioData: Failed to get PlayerState"));

		// 실패 시 SpeakStage를 종료
		if (SpeakStageActor && SpeakStageActor->GetCurrentSpeaker())
			SpeakStageActor->EndStage();
	}
}

//----------------------------------------------------------//
// Interaction System
//----------------------------------------------------------//

void AWheatly::OnInteractionTriggered(AActor* InteractingActor)
{
	if (!HasAuthority() || !SpeakStageActor)
		return;

	APlayerActor* InteractingPlayer = Cast<APlayerActor>(InteractingActor);
	if (!InteractingPlayer)
		return;

	auto PC = Cast<APlayerControl>(InteractingPlayer->GetController());
	
	// 플레이어의 SpeakQuest 완료 여부 확인
	if (auto PS = InteractingPlayer->GetPlayerState<ALingoPlayerState>())
	{
		if (PS->IsSpeakQuestCompleted())
		{
			if ( PC )
				PC->Client_ToastMessage(TEXT("Already Clear SpeakQuest"));

			return;
		}
	}

	// SpeakStage의 상태를 직접 확인
	if (ALingoPlayerState* CurrentSpeaker = SpeakStageActor->GetCurrentSpeaker())
	{
		if ( PC )
			PC->Client_ToastMessage(FString::Printf(TEXT("Current Turn is [%s]"), *ULingoGameHelper::GetPlayerNameFromState(CurrentSpeaker)));

		return;
	}

	if ( PC )
		PC->Client_RequestSpeakScenario(this);
}

void AWheatly::OnOutlineStateChanged(bool bShouldShowOutline)
{
	if (MeshComponent)
	{
		MeshComponent->SetRenderCustomDepth(bShouldShowOutline);
	}
}

void AWheatly::OnSpeakStageSpeakerChanged(APlayerState* NewSpeaker)
{
	// 눈 색상 변경은 서버에서만
	if (HasAuthority())
	{
		ReplicatedEyeColor = (NewSpeaker != nullptr)
			? FLinearColor(1.0f, 1.0f, 0.0f, 1.0f)  // Yellow: Busy
			: FLinearColor(0.0f, 0.5f, 1.0f, 1.0f); // Blue: Available
		
		OnRep_EyeColor();
	}

	// 위젯 표시 제어는 모든 클라이언트에서 실행
	if (!InteractableComp)
		return;

	// 로컬 플레이어 확인
	if ( auto LocalPawn = ULingoGameHelper::GetLocalPawn(GetWorld()) )
	{
		auto LocalPlayerState = LocalPawn->GetPlayerState<ALingoPlayerState>();
		
		// 로컬 플레이어가 현재 발화자인지 확인
		if (LocalPlayerState && NewSpeaker && LocalPlayerState == NewSpeaker)
		{
			// 발화자이면 위젯 숨김
			InteractableComp->SetWidgetVisibility(false);
			PRINTLOG(TEXT("[AWheatly] Local player is speaker - hiding widget"));
		}
		else
		{
			// 발화자가 아니면, DetectionRange 내에 있을 때만 위젯 표시
			if (InteractableComp->DetectionRange)
			{
				if ( IsInRange(LocalPawn))
				{
					InteractableComp->SetWidgetVisibility(true);
					PRINTLOG(TEXT("[AWheatly] Local player not speaker and in range - showing widget"));
				}
			}
		}
	}
}

bool AWheatly::IsInRange(const APawn* LocalPawn) const
{
	TArray<AActor*> OverlappingActors;
	InteractableComp->DetectionRange->GetOverlappingActors(OverlappingActors, ACharacter::StaticClass());
					
	for (AActor* Actor : OverlappingActors)
	{
		if (Actor == LocalPawn)
			return true;
	}
	return false;
}

void AWheatly::OnRep_EyeColor()
{
	ChangeEyeColor(ReplicatedEyeColor);
}

void AWheatly::OnRep_EyeSightState()
{
	ApplyEyeSight();
}

void AWheatly::ApplyEyeSight()
{
	if (!bEyeSightVisible)
	{
		EyeSightComp->SetVisibility(false);
		return;
	}

	const FVector StartLocation = EyeMesh->GetComponentLocation();
	UpdateEyeSight(StartLocation, ReplicatedEyeSightEnd);
}

//----------------------------------------------------------//
// Visual System
//----------------------------------------------------------//

void AWheatly::ChangeEyeColor(FLinearColor newColor)
{
	if (!EyeMaterial)
		return;

	EyeMaterial->SetVectorParameterValue(TEXT("EyeColor"), newColor);
	EyeTraceMaterial->SetVectorParameterValue(TEXT("EyeColor"), newColor);
}

void AWheatly::UpdateEyeSight(const FVector& Start, const FVector& End)
{
	FVector Delta = End - Start;
	float Length = Delta.Size();
	if (Length <= KINDA_SMALL_NUMBER)
	{
		EyeSightComp->SetVisibility(false);
		return;
	}

	FVector Midpoint = Start + (Delta * 0.5f);
	EyeSightComp->SetWorldLocation(Midpoint);

	FVector Direction = Delta / Length;
	FRotator Rotation = FRotationMatrix::MakeFromZ(Direction).Rotator();
	EyeSightComp->SetWorldRotation(Rotation);

	const float LengthScale = Length / FMath::Max(IndicatorBaseLength, KINDA_SMALL_NUMBER);
	const float TargetThickness = 10.f; // Desired thickness
	const float MeshDiameter = FMath::Max(IndicatorBaseRadius * 2.0f, KINDA_SMALL_NUMBER);
	const float RadiusScale = (TargetThickness / MeshDiameter)*2.5f;

	EyeSightComp->SetWorldScale3D(FVector(RadiusScale, RadiusScale, LengthScale));
	EyeSightComp->SetVisibility(true);
}