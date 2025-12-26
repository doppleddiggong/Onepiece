// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file APlayerActor.cpp
 * @brief Provides Doxygen annotations for APlayerActor implementation.
 */

#include "APlayerActor.h"

#include "UMainWidget.h"
#include "FComponentHelper.h"

// Shared
#include "AContactTrigger.h"
#include "AEvaluationTrigger.h"
#include "GameLogging.h"
#include "InputCoreTypes.h"
#include "UDialogManager.h"
#include "UVoiceConversationSystem.h"
#include "UInteractionSystem.h"
#include "UHookSystem.h"
#include "ULingoGameHelper.h"
#include "UPopupManager.h"
#include "UPopup_ReadQuest.h"
#include "ALingoGameState.h"
#include "ALuggageHolder.h"
#include "APlayerControl.h"
#include "ASpeakStageActor.h"
#include "ATeleportOut.h"
#include "ATeleportTrigger.h"
#include "AWheatly.h"
#include "CompassWidget.h"
#include "HairStrandsInterface.h"
#include "OrderKiosk.h"
#include "QuestionnaireKiosk.h"
#include "UToastWidget.h"
#include "UBroadcastManager.h"
#include "UFadeWidget.h"
#include "UKLingoNetworkSystem.h"
#include "UPopup_SpeakJudes.h"
#include "UQuestInfoWidget.h"
#include "UVoiceRecording.h"

#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/KismetMathLibrary.h"
#include "Onepiece/Onepiece.h"

#define MAINWIDGET_PATH TEXT("/Game/CustomContents/UI/WBP_MainWidget.WBP_MainWidget_C")
#define TOASTWIDGET_PATH TEXT("/Game/CustomContents/UI/WBP_ToastWidget.WBP_ToastWidget_C")

#define HOOKMESHPATH_PATH TEXT("/Game/CustomContents/Character/Asset/MiniOwl/MiniOwlbot.MiniOwlbot")

APlayerActor::APlayerActor()
{
	PrimaryActorTick.bCanEverTick = true;
	
	GetCapsuleComponent()->InitCapsuleSize(45.f, 102.0f);
	GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -100), FRotator(0, -90, 0));
	GetMesh()->SetRelativeScale3D(FVector(1.5));
	
	// Movement
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;
	
	// SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	// SpringArmComp->SetupAttachment(GetMesh(), TEXT("Head"));
	// SpringArmComp->TargetArmLength = 0;
	// SpringArmComp->bUsePawnControlRotation = false;
	// SpringArmComp->SetRelativeLocationAndRotation(FVector(0,-14.285715,21.428572), FRotator(90, 90, 0));
	// // SpringArmComp->bInheritPitch = false;
	// SpringArmComp->bInheritRoll = false;
	// SpringArmComp->bInheritPitch = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	// FollowCamera->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	FollowCamera->SetupAttachment(GetMesh(), TEXT("Head"));
	FollowCamera->SetRelativeLocationAndRotation(FVector(0.235242,-14.285715,24.304751), FRotator(90, 90, 0));
	FollowCamera->bUsePawnControlRotation = true;

	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	HoldPosition = CreateDefaultSubobject<USceneComponent>(TEXT("HoldPosition"));
	HoldPosition->SetupAttachment(FollowCamera);
	HoldPosition->SetRelativeLocation(FVector(80.952382,0,-14.285714));

	LookPitch = 0.f;
	AnotherValue = 0.f;

	// System Component
	InteractionSystem = CreateDefaultSubobject<UInteractionSystem>(TEXT("InteractionSystem"));
	HookSystem = CreateDefaultSubobject<UHookSystem>(TEXT("HookSystem"));
	VoiceConversationSystem = CreateDefaultSubobject<UVoiceConversationSystem>(TEXT("VoiceConversationSystem"));

	// Hook
	HookCable = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HookCable"));
	HookCable->SetupAttachment(GetMesh());
	HookCable->SetVisibility(false);
	HookCable->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HookCable->SetMobility(EComponentMobility::Movable);
	HookCable->SetUsingAbsoluteLocation(true);
	HookCable->SetUsingAbsoluteRotation(true);
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (CylinderMesh.Succeeded())
		HookCable->SetStaticMesh(CylinderMesh.Object);
	HookCable->SetRelativeScale3D(FVector(0.05f, 0.05f, 1.0f));

	HookProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HookProjectileMesh"));
	HookProjectileMesh->SetupAttachment(GetMesh());
	HookProjectileMesh->SetVisibility(false);
	HookProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> HookMesh(HOOKMESHPATH_PATH);
	if (HookMesh.Succeeded())
	{
		HookProjectileMesh->SetStaticMesh(HookMesh.Object);
		HookProjectileMesh->SetRelativeScale3D(FVector(1.0f));
		HookProjectileMesh->SetRelativeRotation(FRotator(0, 90, 0));
	}

	// MainWidget 클래스 자동 로드
	MainWidgetClass = FComponentHelper::LoadClass<UMainWidget>(MAINWIDGET_PATH);

	ToastWidgetClass = FComponentHelper::LoadClass<UMainWidget>(TOASTWIDGET_PATH);

	// Compass 마커
	MarkerType = ECompassMarkerType::OtherPlayer;
	bShowOnCompass = true;
	
	// 3인칭 메쉬는 플레이어에게 보이지 않도록
	GetMesh()->SetOwnerNoSee(true);
}

void APlayerActor::BeginPlay()
{
	Super::BeginPlay();

	MoveComp = this->GetCharacterMovement();

	VoiceConversationSystem->InitSystem(this);
	HookSystem->InitSystem(HookCable, HookProjectileMesh);

	// 텔레포트 이벤트 구독
	if (auto DM = UBroadcastManager::Get(GetWorld()))
	{
		DM->OnTeleport.RemoveDynamic(this, &APlayerActor::OnTeleportAllPlayers);
		DM->OnTeleport.AddDynamic(this, &APlayerActor::OnTeleportAllPlayers);

		// DM->OnUpdateQuestRole.RemoveDynamic(this, &APlayerActor::OnUpdateQuestRole);
		// DM->OnUpdateQuestRole.AddDynamic(this, &APlayerActor::OnUpdateQuestRole);
	}

	if (auto GS = ULingoGameHelper::GetLingoGameState(GetWorld()))
	{
		// GS->OnQuestScenarioDataUpdated.RemoveDynamic(this, &APlayerActor::OnUpdateQuestInfo);
		// GS->OnQuestScenarioDataUpdated.AddDynamic(this, &APlayerActor::OnUpdateQuestInfo);
		//
		// GS->OnReadResultUpdated.RemoveDynamic(this, &APlayerActor::OnReadResultUpdated);
		// GS->OnReadResultUpdated.AddDynamic(this, &APlayerActor::OnReadResultUpdated);
		//
		// GS->OnListenResultUpdated.RemoveDynamic(this, &APlayerActor::OnListenResultUpdated);
		// GS->OnListenResultUpdated.AddDynamic(this, &APlayerActor::OnListenResultUpdated);

		GS->OnRoomIdUpdated.RemoveDynamic(this, &APlayerActor::OnRoomIdUpdated);
		GS->OnRoomIdUpdated.AddDynamic(this, &APlayerActor::OnRoomIdUpdated);

		GS->OnRoomLevelUpdated.RemoveDynamic(this, &APlayerActor::OnRoomLevelUpdated);
		GS->OnRoomLevelUpdated.AddDynamic(this, &APlayerActor::OnRoomLevelUpdated);

		// 방향계 업데이트
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this, GS]
		{
			GS->SetAllCompassVisibility(false);
			GS->SetCompassVisibilityByTag("ReadQuestStart", true);
			
		}), 0.5f, false);
	}

	
	if (IsLocallyControlled())
	{
		CreateMainWidget();
		CreateToastWidget();

		if ( IsMainMap() )
			ULingoGameHelper::HideMouseCursor(this);
	}
}

void APlayerActor::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerActor, LookPitch);
	DOREPLIFETIME(APlayerActor, AnotherValue);
}

bool APlayerActor::IsControlEnabled() const
{
	// 팝업이 열려있고 조작을 차단해야 하면 false 반환
	if (UPopupManager* PopupMgr = UPopupManager::Get(GetWorld()))
	{
		if (PopupMgr->ShouldBlockPlayerControl())
			return false;
	}

	// 팝업이 없거나 조작 허용 팝업이면 true 반환
	return true;
}

bool APlayerActor::IsMainMap()
{
	FString MapName = GetWorld()->GetMapName();
	MapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

	if ( MapName.Contains(TEXT("Map1")) )
		return true;
	else if ( MapName.Contains(TEXT("Game")) )
		return true;
	else if ( MapName.Contains(TEXT("Person")) )
		return true;

	return false;
}

void APlayerActor::CreateMainWidget()
{
	if (!MainWidgetClass)
		return;

	if(MainWidget)
		return;

	auto PC = Cast<APlayerControl>(GetController());
	if ( !PC)
		return;
	
	MainWidget = CreateWidget<UMainWidget>(PC, MainWidgetClass);

	if (MainWidget)
	{
		MainWidget->AddToViewport();

		if ( IsMainMap())
		{
			if (auto GS = ULingoGameHelper::GetLingoGameState(GetWorld()))
				MainWidget->UpdateRoomWidget( GS->GetRoomLevel(), GS->GetRoomId());

			MainWidget->CompassWidget->SetVisibility( ESlateVisibility::Visible );
			MainWidget->VoiceRecording->SetVisibility( ESlateVisibility::Visible );

			// UserInfo가 로드되었으면 즉시 업데이트, 아니면 재시도
			if (PC->HasUserInfo() )
			{
				MainWidget->UpdateStateWidget( PC->GetUserId(), PC->GetUserName());
				MainWidget->UpdateChatWidget();
				return;
			}

			// UserInfo 로드 대기 후 재시도 (0.5초 후 한 번)
			FTimerHandle RetryTimer;
			GetWorld()->GetTimerManager().SetTimer(RetryTimer, [this, PC]()
			{
				if ( PC->GetUserId())
				{
					MainWidget->UpdateStateWidget( PC->GetUserId(), PC->GetUserName());
				}
				MainWidget->UpdateChatWidget();
			}, 0.5f, false);
		}
	}
}

void APlayerActor::CreateToastWidget()
{
	if (!ToastWidgetClass)
		return;

	if(ToastWidget)
		return;

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC)
		return;

	ToastWidget = CreateWidget<UToastWidget>(PC, ToastWidgetClass);
	if (ToastWidget)
		ToastWidget->AddToViewport(GameLayer::Toast);
}

void APlayerActor::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	APlayerState* PS = GetPlayerState();
	if (!PS)
		return;

	// 플레이어 인덱스 확인 (GameState의 PlayerArray 사용)
	int32 PlayerIndex = -1;
	if (AGameStateBase* GS = GetWorld()->GetGameState())
		PlayerIndex = GS->PlayerArray.IndexOfByKey(PS);

	// 2P에게 Another = 1 적용 (1P는 0)
	AnotherValue = (PlayerIndex == 0) ? 0.0f : 1.0f;

	ApplyAnotherValue();
}

void APlayerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MainWidget && MainWidget->CompassWidget)
	{
		UCompassWidget* Compass = MainWidget->CompassWidget;
		float CameraRotationZ = FollowCamera->GetComponentRotation().Yaw;
		
		Compass->RotateCompass(CameraRotationZ);

		UpdateCompassMarkers();
	}
}

void APlayerActor::OnRep_Controller()
{
	Super::OnRep_Controller();

	if (IsLocallyControlled())
	{
		CreateMainWidget();
		CreateToastWidget();
	}
}

void APlayerActor::OnRep_LookPitch()
{
	// if (SpringArmComp)
	// {
	// 	// 다른 클라이언트에서 보이는 서버 캐릭터의 복제본의 설정 변경
	// 	SpringArmComp->bUsePawnControlRotation = false;
	//
	// 	FRotator CurrentRotation = SpringArmComp->GetRelativeRotation();
	// 	CurrentRotation.Pitch = LookPitch;
	// 	SpringArmComp->SetRelativeRotation(CurrentRotation);
	// }
}

void APlayerActor::OnRep_AnotherValue()
{
	ApplyAnotherValue();
}

void APlayerActor::ApplyAnotherValue()
{
	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp)
		return;

	// Dynamic Material Instance 생성 및 파라미터 설정
	for (int32 i = 0; i < MeshComp->GetNumMaterials(); ++i)
	{
		UMaterialInterface* Material = MeshComp->GetMaterial(i);
		if (!Material)
			continue;

		UMaterialInstanceDynamic* DynamicMaterial = Cast<UMaterialInstanceDynamic>(Material);
		if (!DynamicMaterial)
			DynamicMaterial = MeshComp->CreateAndSetMaterialInstanceDynamic(i);

		if (DynamicMaterial)
			DynamicMaterial->SetScalarParameterValue(FName("Another"), AnotherValue);
	}


	auto PS = GetPlayerState<ALingoPlayerState>();
	if (PS)
	{
		PS->RefreshQuestState();
	}
}

void APlayerActor::RecoveryMovementMode(const EMovementMode InMovementMode)
{
	if ( InMovementMode == MOVE_None)
		return;
	
	auto Movement = this->GetCharacterMovement();

	Movement->SetMovementMode( InMovementMode );
	this->bUseControllerRotationYaw = false;
	this->bUseControllerRotationPitch = false;
	Movement->bOrientRotationToMovement = true;
}

void APlayerActor::PlayTTSAudio(const TArray<uint8>& AudioData)
{
	VoiceConversationSystem->PlayVoiceAudio(AudioData);
}

void APlayerActor::Cmd_StopMove_Implementation()
{
	ServerRPC_StopMove();
}

void APlayerActor::Cmd_Run_Implementation()
{
	if (!IsControlEnabled())
		return;

	ServerRPC_DoRun();
}

void APlayerActor::Cmd_Move_Implementation(const FVector2D& Axis)
{
	if (!IsControlEnabled())
		return;

	if ( !Controller)
	{
		return;
	}

	// Use the controller's rotation to determine movement direction.
	const FRotator ControlRotation = Controller->GetControlRotation();
	const EMovementMode CurrentMovementMode = GetCharacterMovement()->MovementMode;

	if (CurrentMovementMode == MOVE_Walking || CurrentMovementMode == MOVE_Falling)
	{
		// For ground movement, only use the Yaw rotation to prevent pitching into the ground.
		const FRotator YawRotation(0.0f, ControlRotation.Yaw, 0.0f);

		// Calculate forward and right vectors based on the Yaw rotation.
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, Axis.Y);
		AddMovementInput(RightDirection, Axis.X);
	}
}

void APlayerActor::Cmd_Look_Implementation(const FVector2D& Axis)
{
	if (!IsControlEnabled())
		return;

	AddControllerYawInput(Axis.X);
	AddControllerPitchInput(Axis.Y);
}

void APlayerActor::Cmd_Jump_Implementation()
{
	if (!IsControlEnabled())
		return;

	ServerRPC_DoJumpStart();
}

void APlayerActor::Cmd_RecordStart_Implementation()
{
	if (!IsControlEnabled())
		return;

	VoiceConversationSystem->StartRecording();
}

void APlayerActor::Cmd_RecordEnd_Implementation()
{
	VoiceConversationSystem->StopRecording();
}

void APlayerActor::Cmd_Info_Implementation()
{
	if (!IsControlEnabled())
		return;

	
	auto GS = ULingoGameHelper::GetLingoGameState(GetWorld());
	auto PS = GetPlayerState<ALingoPlayerState>();

	if ( PS->bReadQuestIng &&
		!PS->bReadQuestCompleted )
	{
		if (auto Popup = UPopupManager::ShowPopupAs<UPopup_ReadQuest>(GetWorld(), EPopupType::ReadQuest))
			Popup->InitRead(GS->ReadScenarioData);
	}
	else if (
		PS->bListenQuestIng &&
		!PS->bListenQuestCompleted )
	{
		auto QuestRole = GetQuestRole();

		if ( QuestRole == EQuestRole::Both)
			RequestListenAudio( GS->ListenScenarioData.full_data.Kor); 
		else if ( QuestRole == EQuestRole::OnlyQuestion1)
			RequestListenAudio( GS->ListenScenarioData.word_data1.Kor); 
		else if ( QuestRole == EQuestRole::OnlyQuestion2)
			RequestListenAudio( GS->ListenScenarioData.word_data2.Kor); 
	}
	else if ( PS->bSpeakQuestIng &&
		!PS->bSpeakQuestCompleted )
	{
		if (auto SpeakStageActor = ULingoGameHelper::GetSpeakStageActor(GetWorld()))
		{
			if ( SpeakStageActor->IsMyTurn(PS) )
			{
				PlaySpeakInfo( SpeakStageActor->GetCurrentStepIndex() );
			}
		}
	}
}

void APlayerActor::PlaySpeakInfo(int32 StepIndex)
{
	auto PS = GetPlayerState<ALingoPlayerState>();

	FSpeakStageQuestion CurrentSpeakQuestion;
	if (PS->GetCurrentSpeakQuestion(StepIndex, CurrentSpeakQuestion))
	{
		if (UDialogManager* DM = UDialogManager::Get(GetWorld()))
			DM->ShowToast(*CurrentSpeakQuestion.GetQuestionMessage());

		RequestSpeakAudio(CurrentSpeakQuestion.kor);
	}
}

EQuestRole APlayerActor::GetQuestRole()
{
	ALingoPlayerState* PS = GetPlayerState<ALingoPlayerState>();
	return PS->QuestRole;
}

void APlayerActor::RequestListenAudio(const FString& AudioText)
{
	if (bIsRequest)
		return;

	if (auto KLingoNetwork = UKLingoNetworkSystem::Get(GetWorld()))
	{
		bIsRequest = true;

		KLingoNetwork->RequestListenAudio(
			AudioText,
			FResponseListenAudioDelegate::CreateUObject(this, &APlayerActor::OnResponseListenAudio)
		);
	}
}

void APlayerActor::OnResponseListenAudio(FResponseListenAudio& ResponseData, bool bWasSuccessful)
{
	bIsRequest = false;

	if (bWasSuccessful)
	{
		this->PlayTTSAudio(ResponseData.audio_base64);
		UDialogManager::Get(GetWorld())->ShowToast(*ResponseData.audio_text);
	}
}


void APlayerActor::RequestSpeakAudio(const FString& AudioText)
{
	if (bIsRequest)
		return;

	if (auto KLingoNetwork = UKLingoNetworkSystem::Get(GetWorld()))
	{
		bIsRequest = true;

		KLingoNetwork->RequestListenAudio(
			AudioText,
			FResponseListenAudioDelegate::CreateUObject(this, &APlayerActor::OnResponseSpeakAudio)
		);
	}
}

void APlayerActor::OnResponseSpeakAudio(FResponseListenAudio& ResponseData, bool bWasSuccessful)
{
	bIsRequest = false;

	if (bWasSuccessful)
	{
		this->PlayTTSAudio(ResponseData.audio_base64);
	}
}


void APlayerActor::ServerRPC_StopMove_Implementation()
{
	MulticastRPC_StopMove();
}

void APlayerActor::MulticastRPC_StopMove_Implementation()
{
	bIsRunning = false;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void APlayerActor::ServerRPC_DoJumpStart_Implementation()
{
	MulticastRPC_DoJumpStart();
}

void APlayerActor::MulticastRPC_DoJumpStart_Implementation()
{
	bIsJumpStart = true;
}

void APlayerActor::ServerRPC_DoJump_Implementation()
{
	MulticastRPC_DoJump();
}

void APlayerActor::MulticastRPC_DoJump_Implementation()
{
	bIsJumpStart = false;
	Jump();
}

void APlayerActor::ServerRPC_DoRun_Implementation()
{
	MulticastRPC_DoRun();
}

void APlayerActor::MulticastRPC_DoRun_Implementation()
{
	if (bIsJumpStart || GetMovementComponent()->IsFalling()) return;
	
	if (bIsRunning)
	{
		bIsRunning = false;
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}
	else
	{
		bIsRunning = true;
		GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
	}
}

void APlayerActor::OnGameMessage(const FString& Message)
{
	PRINT_STRING(TEXT("Event Received: %s"), *Message);

	// 서버에서 호출되면 클라이언트 RPC로 전달
	if (HasAuthority())
	{
		ClientRPC_ShowGameMessage(Message);
	}
	else
	{
		ClientRPC_ShowGameMessage_Implementation(Message);
	}
}

void APlayerActor::ClientRPC_ShowGameMessage_Implementation(const FString& Message)
{
	// 클라이언트에서만 실행됨 - LocalPlayerSubsystem 사용 가능
	if (UDialogManager* DialogManager = UDialogManager::Get(GetWorld()))
	{
		DialogManager->ShowToast(Message);
	}
}

void APlayerActor::OnTeleportAllPlayers(FTransform TargetTransform)
{
	// 서버에 텔레포트 요청
	ServerRPC_Teleport(TargetTransform);

	// 로컬 플레이어만 페이드 처리
	if (!IsLocallyControlled())
		return;

	PRINTLOG(TEXT("APlayerActor::OnTeleportAllPlayers - Start teleport to %s"), *TargetTransform.ToString());

	// 목표 위치 저장
	PendingTeleportTransform = TargetTransform;

	// FadeWidget 가져오기
	if (!MainWidget)
	{
		PRINTLOG(TEXT("APlayerActor::OnTeleportAllPlayers - MainWidget is null"));
		return;
	}

	UFadeWidget* FadeWidget = MainWidget->GetFadeWidget();
	if (!FadeWidget)
	{
		PRINTLOG(TEXT("APlayerActor::OnTeleportAllPlayers - FadeWidget is null"));
		return;
	}

	// FadeOut 완료 시 텔레포트 실행
	FadeWidget->OnFadeOutComplete.AddDynamic(this, &APlayerActor::OnFadeOutCompleteForTeleport);

	// FadeOut 시작
	MainWidget->FadeOut(0.5f);
}

void APlayerActor::ServerRPC_Teleport_Implementation(FTransform TargetTransform)
{	
	SetActorLocation(TargetTransform.GetLocation());
	if (APlayerControl* PC = Cast<APlayerControl>(Controller))
	{
		PC->SetControlRotation(PendingTeleportTransform.Rotator());
	}
}

void APlayerActor::Server_NotifySpeakJudgeComplete_Implementation(const FResponseSpeakingJudes& Response)
{
	if (!HasAuthority())
		return;

	// PlayerState에 평가 결과 저장
	if (auto PS = GetPlayerState<ALingoPlayerState>())
	{
		PS->AddSpeakJudes(Response);

		// 클라이언트에게 팝업 표시 지시 (Client RPC)
		Client_ShowSpeakJudesPopup(Response);
	}
}

void APlayerActor::Server_NotifyConfirmSpeakJudes_Implementation()
{
	if (!HasAuthority())
		return;

	// SpeakStage에 답변 완료 알림
	if (auto PS = GetPlayerState<ALingoPlayerState>())
	{
		if (auto SpeakStage = ULingoGameHelper::GetSpeakStageActor(GetWorld()))
		{
			SpeakStage->NotifyAnswerComplete(PS);
		}
	}
}

void APlayerActor::Client_ShowSpeakJudesPopup_Implementation(const FResponseSpeakingJudes& Response)
{
	// 클라이언트에서 팝업 표시
	if (auto Popup = UPopupManager::ShowPopupAs<UPopup_SpeakJudes>(GetWorld(), EPopupType::SpeakJudes))
	{
		// Confirm 버튼 클릭 시 서버에 알리는 델리게이트 설정
		FOnSpeakJudesConfirmDelegate OnConfirm;
		OnConfirm.BindLambda([this]()
		{
			Server_NotifyConfirmSpeakJudes();
		});

		Popup->InitPopup(Response, OnConfirm);
	}
}

void APlayerActor::OnFadeOutCompleteForTeleport()
{
	// 텔레포트 실행
	SetActorLocation(PendingTeleportTransform.GetLocation());
	if (APlayerControl* PC = Cast<APlayerControl>(Controller))
	{
		PC->SetControlRotation(PendingTeleportTransform.Rotator());
	}
	
	// FadeWidget 가져오기
	if (!MainWidget)
		return;

	UFadeWidget* FadeWidget = MainWidget->GetFadeWidget();
	if (!FadeWidget)
		return;

	// FadeOut 델리게이트 해제
	FadeWidget->OnFadeOutComplete.RemoveDynamic(this, &APlayerActor::OnFadeOutCompleteForTeleport);

	// FadeIn 시작
	MainWidget->FadeIn(0.5f);
}

void APlayerActor::UpdateCompassMarkers()
{
	if (!MainWidget || !MainWidget->CompassWidget)
		return;

	UCompassWidget* Compass = MainWidget->CompassWidget;

	// 1. 현재 월드의 모든 트래킹 대상 수집
	TArray<AActor*> TrackedActors;
	// GetAllActorsOfClass 시 배열 안에 있는건 clear됨. 임시배열 필요
	TArray<AActor*> TempActors;

	// AContactTrigger
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AContactTrigger::StaticClass(), TempActors);
	TrackedActors.Append(TempActors);

	// ALuggageHolder
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALuggageHolder::StaticClass(), TempActors);
	TrackedActors.Append(TempActors);

	// AOrderKiosk
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOrderKiosk::StaticClass(), TempActors);
	TrackedActors.Append(TempActors);

	// Wheatly
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWheatly::StaticClass(), TempActors);
	TrackedActors.Append(TempActors);

	// Teleporter
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATeleportTrigger::StaticClass(), TempActors);
	TrackedActors.Append(TempActors);

	// QuestionnaireKiosk
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AQuestionnaireKiosk::StaticClass(), TempActors);
	TrackedActors.Append(TempActors);

	// Evaluation Trigger
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEvaluationTrigger::StaticClass(), TempActors);
	TrackedActors.Append(TempActors);

	// Other player
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerActor::StaticClass(), TempActors);
	for (AActor* OtherActor : TempActors)
	{
		if (OtherActor != this)
			TrackedActors.Add(OtherActor);
	}

	// 2. 각 TrackedActor에 대해 마커 생성 or 업데이트                                                                                                                
	for (AActor* TrackedActor : TrackedActors)
	{
		if (!TrackedActor) continue;

		// Interface로 캐스팅해서 타입 정보 가져오기
		ICompassTargetInterface* Target = Cast<ICompassTargetInterface>(TrackedActor);
		if (!Target) continue;

		if (!Target->ShouldShowOnCompass())
		{
			if (UImage* ExistingMarker = CompassMarkerMap.FindRef(TrackedActor))
			{
				ExistingMarker->SetVisibility(ESlateVisibility::Hidden);
			}
			continue;
		}

		// Interface 함수로 마커 타입 확인
		ECompassMarkerType CompassMarkerType = Target->GetCompassMarkerType();
		
		// 2-1. 마커가 없으면 생성
		UImage* Marker = CompassMarkerMap.FindRef(TrackedActor);
		if (!Marker)
		{
			Marker = Compass->AddCompassMarker(CompassMarkerType);
			CompassMarkerMap.Add(TrackedActor, Marker);
		}
		else
		{
			// 기존 마커 텍스처 업데이트
			UTexture2D* NewTexture = Compass->GetTextureForMarkerType(CompassMarkerType);
			if (NewTexture)
			{
				Marker->SetBrushFromTexture(NewTexture);
			}

			// 다시 보이게 설정 (Hidden 상태였을 수 있음)
			Marker->SetVisibility(ESlateVisibility::Visible);
		}

		// 2-2. 상대 회전 계산 (Yaw만 필요)
		FRotator RelativeRotation = FindRelativeRotationAtTarget(TrackedActor);
		float TargetYaw = RelativeRotation.Yaw;

		// 2-3. 마커 위치 업데이트 (bSideLock은 false로 가정)                                                                                                         
		Compass->SetMarkerPosition(Marker, TargetYaw, false);
	}

	// 제거된 액터의 마커 정리
	TArray<AActor*> ActorsToRemove;
	for (auto& Pair : CompassMarkerMap)
	{
		if (!Pair.Key || !TrackedActors.Contains(Pair.Key))
		{
			if (Pair.Value)
			{
				Pair.Value->SetVisibility(ESlateVisibility::Hidden);	
			}
			ActorsToRemove.Add(Pair.Key);
		}
	}

	for (AActor* Actor : ActorsToRemove)
	{
		CompassMarkerMap.Remove(Actor);
	}
}

FRotator APlayerActor::FindRelativeRotationAtTarget(AActor* Target)
{
	// Get World Location (Capsule Component)
	FVector CapsuleLocation = GetCapsuleComponent()->GetComponentLocation();

	// Get World Rotation (Capsule Component)
	FRotator CapsuleRotation = GetCapsuleComponent()->GetComponentRotation();

	// Get World Location (Root Component)
	FVector TargetLocation = Target->GetActorLocation();

	// Find Relative Look at Rotation
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(
		CapsuleLocation, TargetLocation);

	// 상대 회전 계산
	FRotator RelativeRotation = UKismetMathLibrary::NormalizedDeltaRotator(
		LookAtRotation,CapsuleRotation);

	// Return Value (Roll, Pitch, Yaw)
	return RelativeRotation;
}

void APlayerActor::OnRoomIdUpdated(int64 NewRoomId)
{
	if (!IsLocallyControlled())
		return;

	if (MainWidget)
	{
		if (auto GS = ULingoGameHelper::GetLingoGameState(GetWorld()))
		{
			const int RoomLevel = GS->GetRoomLevel();
			const int RoomId = NewRoomId;
			
			MainWidget->UpdateRoomWidget(RoomLevel, RoomId);
		}
	}
}

void APlayerActor::OnRoomLevelUpdated(int32 NewRoomLevel)
{
	if (!IsLocallyControlled())
		return;

	if (MainWidget)
	{
		if (auto GS = ULingoGameHelper::GetLingoGameState(GetWorld()))
		{
			const int RoomLevel = NewRoomLevel;
			const int RoomId = GS->GetRoomId();
			
			MainWidget->UpdateRoomWidget(RoomLevel, RoomId);
		}
	}
}

// void APlayerActor::OnUpdateQuestInfo()
// {
// 	if (!IsLocallyControlled())
// 		return;
// 	MainWidget->GetQuestInfoWidget()->InitQuestInfo();
// }
//
// void APlayerActor::OnUpdateQuestRole(EQuestRole QuestRole)
// {
// 	if (!IsLocallyControlled())
// 		return;
// 	MainWidget->GetQuestInfoWidget()->InitQuestInfo();
// }
//

// void APlayerActor::OnReadResultUpdated(const FResponseReadResult& Result)
// {
// 	if (!IsLocallyControlled())
// 		return;
// 	
// 	MainWidget->GetQuestInfoWidget()->SetVisibility(ESlateVisibility::Collapsed);
// }
//
// void APlayerActor::OnListenResultUpdated( const FResponseListenResult& Result)
// {
// 	if (!IsLocallyControlled())
// 		return;
// 	
// 	MainWidget->GetQuestInfoWidget()->SetVisibility(ESlateVisibility::Collapsed);
// }