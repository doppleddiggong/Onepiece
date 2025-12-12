// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "AWheatly.h"

#include "APlayerActor.h"
#include "APlayerControl.h"
#include "ASpeakStageActor.h"
#include "FComponentHelper.h"
#include "GameLogging.h"
#include "UInteractWidget.h"

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
	InteractableComp->DetectionDistance = 500.0f
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

	PlayAnimation(EWheatlyAnim::PowerOn);
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

	UAnimInstance* AnimInstance = MeshComponent->GetAnimInstance();
	if (!AnimInstance)
	{
		PRINTLOG(TEXT("[AWheatly] Multicast_PlayAnimation - AnimInstance is null"));
		return;
	}

	UAnimMontage* Montage = AnimMontage.FindRef(AnimType);
	if (!Montage)
	{
		PRINTLOG(TEXT("[AWheatly] Multicast_PlayAnimation - Montage not found for type: %d"), static_cast<int32>(AnimType));
		return;
	}

	// 모든 몽타주 중지
	AnimInstance->StopAllMontages(0.0f);

	// 종료 델리게이트 바인딩
	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &AWheatly::OnMontageEnded);

	// 몽타주 재생
	AnimInstance->Montage_Play(Montage);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, Montage);

	PRINTLOG(TEXT("[AWheatly] Animation played: %d"), static_cast<int32>(AnimType));
}

void AWheatly::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (bInterrupted)
	{
		PRINTLOG(TEXT("[AWheatly] Animation interrupted"));
		return;
	}

	// 반복 재생 (서버에서만)
	if (HasAuthority())
	{
		Multicast_PlayAnimation(AnimType);
	}
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

//----------------------------------------------------------//
// Interaction System
//----------------------------------------------------------//

void AWheatly::OnInteractionTriggered(AActor* InteractingActor)
{
	if (!HasAuthority())
	{
		PRINTLOG(TEXT("[AWheatly] OnInteractionTriggered - Not authority"));
		return;
	}

	APlayerActor* InteractingPlayer = Cast<APlayerActor>(InteractingActor);
	if (!InteractingPlayer)
	{
		PRINTLOG(TEXT("[AWheatly] OnInteractionTriggered - Not a PlayerActor"));
		return;
	}

	// bIsBusy가 true이면 토스트 메시지 표시
	if (bIsBusy)
	{
		APlayerControl* PC = Cast<APlayerControl>(InteractingPlayer->GetController());
		if (PC)
		{
			FString ToastMessage = FString::Printf(TEXT("Current Turn is [%s]"), *busyPlayerName);
			PC->Client_ToastMessage(ToastMessage);
			PRINTLOG(TEXT("[AWheatly] Interaction denied - Busy with: %s"), *busyPlayerName);
		}
		return;
	}

	// bIsBusy가 false이면 SpeakStage에 플레이어 추가 요청
	if (SpeakStage)
	{
		APlayerState* PS = InteractingPlayer->GetPlayerState();
		if (PS)
		{
			// TODO: SpeakStage->RequestJoinConversation(PS) 구현 필요
			PRINTLOG(TEXT("[AWheatly] Player interaction: %s (SpeakStage integration needed)"),
				*PS->GetPlayerName());
		}
	}
	else
	{
		PRINTLOG(TEXT("[AWheatly] OnInteractionTriggered - SpeakStage not connected"));
	}
}

void AWheatly::OnRep_bIsBusy()
{
	// Busy 상태에 따라 눈 색상 변경
	FLinearColor newColor = bIsBusy
		? FLinearColor(1.0f, 1.0f, 0.0f, 1.0f)  // Yellow: Busy
		: FLinearColor(0.0f, 0.5f, 1.0f, 1.0f); // Blue: Available

	ChangeEyeColor(newColor);

	PRINTLOG(TEXT("[AWheatly] OnRep_bIsBusy - Status: %s, Player: %s"),
		bIsBusy ? TEXT("BUSY") : TEXT("AVAILABLE"),
		*busyPlayerName);
}

//----------------------------------------------------------//
// Visual System
//----------------------------------------------------------//

void AWheatly::ChangeEyeColor(FLinearColor newColor)
{
	if (!dynamicMaterial)
	{
		PRINTLOG(TEXT("[AWheatly] ChangeEyeColor - dynamicMaterial is null"));
		return;
	}

	dynamicMaterial->SetVectorParameterValue(TEXT("EyeColor"), newColor);

	PRINTLOG(TEXT("[AWheatly] Eye color changed to: R=%.2f G=%.2f B=%.2f"),
		newColor.R, newColor.G, newColor.B);
}