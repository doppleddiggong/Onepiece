// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file APlayerControl.cpp
 * @brief APlayerControl 구현에 대한 Doxygen 주석을 제공합니다.
 */
#include "APlayerControl.h"

#include "APlayerActor.h"
#include "IControllable.h"
#include "UMainWidget.h"
#include "AWheatly.h"
#include "UKLingoNetworkSystem.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

#include "InputMappingContext.h"
#include "InputAction.h"

#include "FComponentHelper.h"
#include "UBroadcastManager.h"
#include "UInteractionSystem.h"
#include "UHookSystem.h"
#include "ULingoGameInstanceSubsystem.h"
#include "ADropper.h"
#include "ALingoGameState.h"
#include "luggage.h"
#include "EngineUtils.h"
#include "GameLogging.h"
#include "OrderKiosk.h"
#include "TutorialComponent.h"
#include "UDialogManager.h"
#include "ULingoGameHelper.h"
#include "UPopupManager.h"
#include "UPopup_SpeakQuest.h"
#include "UPopup_SpeakResult.h"

#define IMC_DEFAULT_PATH			TEXT("/Game/CustomContents/Input/IMC_Game_Player.IMC_Game_Player")
#define IA_MOVE_PATH				TEXT("/Game/CustomContents/Input/IA_Game_Movement.IA_Game_Movement")
#define IA_LOOK_PATH				TEXT("/Game/CustomContents/Input/IA_Game_LookAround.IA_Game_LookAround")
#define IA_JUMP_PATH				TEXT("/Game/CustomContents/Input/IA_Game_Jump.IA_Game_Jump")
#define IA_RECORD_PATH				TEXT("/Game/CustomContents/Input/IA_Game_Record.IA_Game_Record")
#define IA_GRAB_PATH				TEXT("/Game/CustomContents/Input/IA_Game_Grab.IA_Game_Grab")
#define IA_INTERACT_PATH			TEXT("/Game/CustomContents/Input/IA_Game_Interact.IA_Game_Interact")
#define IA_RUN_PATH					TEXT("/Game/CustomContents/Input/IA_Game_Run.IA_Game_Run")
#define IA_INFO_PATH				TEXT("/Game/CustomContents/Input/IA_Game_Info.IA_Game_Info")
#define IA_HOOK_PATH				TEXT("/Game/CustomContents/Input/IA_Game_Hook.IA_Game_Hook")
#define IA_CHAT_PATH				TEXT("/Game/CustomContents/Input/IA_EnterChat.IA_EnterChat")


APlayerControl::APlayerControl()
{
	IMC_Default = FComponentHelper::LoadAsset<UInputMappingContext>(IMC_DEFAULT_PATH);

	IA_Move = FComponentHelper::LoadAsset<UInputAction>(IA_MOVE_PATH);
	IA_Look = FComponentHelper::LoadAsset<UInputAction>(IA_LOOK_PATH);
	IA_Jump = FComponentHelper::LoadAsset<UInputAction>(IA_JUMP_PATH);
	IA_Record = FComponentHelper::LoadAsset<UInputAction>(IA_RECORD_PATH);
	IA_Grab = FComponentHelper::LoadAsset<UInputAction>(IA_GRAB_PATH);
	IA_Interact = FComponentHelper::LoadAsset<UInputAction>(IA_INTERACT_PATH);
	IA_Run = FComponentHelper::LoadAsset<UInputAction>(IA_RUN_PATH);
	IA_Info = FComponentHelper::LoadAsset<UInputAction>(IA_INFO_PATH);
	IA_Hook = FComponentHelper::LoadAsset<UInputAction>(IA_HOOK_PATH);
	IA_Chat = FComponentHelper::LoadAsset<UInputAction>(IA_CHAT_PATH);

	TutorialComponent = CreateDefaultSubobject<UTutorialComponent>(TEXT("TutorialComponent"));
}

void APlayerControl::BeginPlay()
{
	Super::BeginPlay();

	if (auto LP = GetLocalPlayer())
	{
		if (auto SubSystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (IMC_Default)
			{
				SubSystem->ClearAllMappings();
				SubSystem->AddMappingContext(IMC_Default, 0);
			}
		}
	}
	if (IsLocalController())
	{
		UserInfo = ULingoGameInstanceSubsystem::Get(GetWorld())->GetUserInfo();
		Server_SetUserInfo(UserInfo);
	}
}

void APlayerControl::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EIC->BindAction(IA_Move, ETriggerEvent::Triggered,  this, &APlayerControl::OnMove);
		EIC->BindAction(IA_Move, ETriggerEvent::Completed,  this, &APlayerControl::OnStopMove);
		EIC->BindAction(IA_Look, ETriggerEvent::Triggered,  this, &APlayerControl::OnLook);
		
		EIC->BindAction(IA_Jump, ETriggerEvent::Started,    this, &APlayerControl::OnJump);

		EIC->BindAction(IA_Record, ETriggerEvent::Started, this, &APlayerControl::OnRecordPressed);
		EIC->BindAction(IA_Record, ETriggerEvent::Completed, this, &APlayerControl::OnRecordReleased);

		EIC->BindAction(IA_Grab, ETriggerEvent::Started, this, &APlayerControl::OnGrab);
		EIC->BindAction(IA_Grab, ETriggerEvent::Completed, this, &APlayerControl::OnGrabRelease);

		EIC->BindAction(IA_Interact, ETriggerEvent::Started, this, &APlayerControl::OnInteract);
		EIC->BindAction(IA_Run, ETriggerEvent::Started, this, &APlayerControl::OnRun);

		EIC->BindAction(IA_Info, ETriggerEvent::Started, this, &APlayerControl::OnInfo);
		
		EIC->BindAction(IA_Hook, ETriggerEvent::Started, this, &APlayerControl::OnHook);
		
		EIC->BindAction(IA_Chat, ETriggerEvent::Started, this, &APlayerControl::OnChat);
	}
}

void APlayerControl::UpdateQuestRole(EQuestRole QuestRole)
{
	UBroadcastManager::Get(GetWorld())->SendUpdateQuestRole(QuestRole);
}

IControllable* APlayerControl::GetControllable() const
{
	APawn* P = GetPawn();
	if (!P)
		return nullptr;

	// UObject 기반 UInterface 라면 Cast 가능
	if (IControllable* C = Cast<IControllable>(P))
		return C;

	return nullptr;
}

bool APlayerControl::ShouldSkipTutorial() const
{
	const int32 UserId = ULingoGameHelper::GetUserId(GetWorld());
	const FString ConfigSection = TEXT("/Script/Onepiece.TutorialSystem");
	const FString ConfigKey = FString::Printf(TEXT("TutorialCompleted_%d"), UserId);

	FString bCompleted;
	if (GConfig->GetString(*ConfigSection, *ConfigKey, bCompleted, GGameUserSettingsIni))
	{
		if (bCompleted == TEXT("true"))
		{
			PRINTLOG(TEXT("[Tutorial] User %d already completed tutorial"), UserId);
			return true;  // 스킵
		}
	}

	return false; // 튜토리얼 진행
}

void APlayerControl::StartTutorialManually()
{
	if (TutorialComponent)
	{
		TutorialComponent->StartTutorial();
		PRINTLOG(TEXT("[Tutorial] Starting tutorial manually"));
	}
}

void APlayerControl::OnTutorialCompleted()
{
	// 세이브 시스템에 여부 저장
	const int32 UserId = ULingoGameHelper::GetUserId(GetWorld());
	const FString ConfigSection = TEXT("/Script/Onepiece.TutorialSystem");
	const FString ConfigKey = FString::Printf(TEXT("TutorialCompleted_%d"), UserId);

	GConfig->SetString(
				*ConfigSection,
				*ConfigKey,
				TEXT("true"),
				GGameUserSettingsIni
			);
	GConfig->Flush(false, GGameUserSettingsIni);
}

void APlayerControl::OnMove(const FInputActionValue& Value)
{
	if (IControllable* C = GetControllable())
		C->Cmd_Move(Value.Get<FVector2D>());
}

void APlayerControl::OnLook(const FInputActionValue& Value)
{
	if (IControllable* C = GetControllable())
		C->Cmd_Look(Value.Get<FVector2D>());
}

void APlayerControl::OnStopMove(const FInputActionValue& Value)
{
	if (IControllable* C = GetControllable())
	{
		C->Cmd_StopMove();
	}
}

void APlayerControl::OnJump(const FInputActionValue&)
{
	if (IControllable* C = GetControllable())
		C->Cmd_Jump();

	//this->TEST_DropperDropProcess();
	// this->TEST_AddItemToBoxList();
}


void APlayerControl::OnRun(const FInputActionValue& Value)
{
	if (IControllable* C = GetControllable())
	{
		C->Cmd_Run();
	}
}

void APlayerControl::OnRecordPressed(const FInputActionValue& Value)
{
	if (IControllable* C = GetControllable())
		C->Cmd_RecordStart();
}

void APlayerControl::OnRecordReleased(const FInputActionValue& Value)
{
	if (IControllable* C = GetControllable())
		C->Cmd_RecordEnd();
}

void APlayerControl::OnInfo(const FInputActionValue& Value)
{
	if (IControllable* C = GetControllable())
	{
		C->Cmd_Info();
	}
}

void APlayerControl::OnGrab(const FInputActionValue& Value)
{
	Server_OnGrab();
}

void APlayerControl::OnGrabRelease(const FInputActionValue& Value)
{
	Server_OnGrabRelease();
}

void APlayerControl::OnInteract(const FInputActionValue& Value)
{
	Server_OnInteract();
}

void APlayerControl::Server_OnGrab_Implementation()
{
	APlayerActor* MyPlayer = Cast<APlayerActor>(GetPawn());
	if (MyPlayer && MyPlayer->InteractionSystem)
	{
		MyPlayer->InteractionSystem->TryPickUp();
	}
}

void APlayerControl::Server_OnGrabRelease_Implementation()
{
	APlayerActor* MyPlayer = Cast<APlayerActor>(GetPawn());
	if (MyPlayer && MyPlayer->InteractionSystem)
	{
		MyPlayer->InteractionSystem->TryDrop();
	}
}


void APlayerControl::Server_OnInteract_Implementation()
{
	APlayerActor* MyPlayer = Cast<APlayerActor>(GetPawn());
	if (MyPlayer && MyPlayer->InteractionSystem)
	{
		MyPlayer->InteractionSystem->TryInteract();
	}
}

void APlayerControl::OnHook(const FInputActionValue& Value)
{
	Server_OnHook();
}

void APlayerControl::OnChat(const FInputActionValue& Value)
{
	// GameAndUI로??
	FInputModeUIOnly uiInputMode;
	SetInputMode(uiInputMode);
	
	APlayerActor* player = Cast<APlayerActor>(GetPawn());
	player->GetMainWidget()->SetFocusOnChat();
}

void APlayerControl::Server_OnHook_Implementation()
{
	APlayerActor* MyPlayer = Cast<APlayerActor>(GetPawn());
	if (MyPlayer && MyPlayer->HookSystem)
	{
		MyPlayer->HookSystem->TryHook();
	}
}

void APlayerControl::Server_SetUserInfo_Implementation(const FResponseUserMe& InUserInfo)
{
	UserInfo = InUserInfo;
}

void APlayerControl::RequestDrop(APlayerControl* Requester)
{
	if (auto World = GetWorld())
	{
		for (TActorIterator<ADropper> It(World); It; ++It)
		{
			ADropper* Dropper = *It;
			if (Dropper)
			{
				if (Dropper->IsBusy() )
				{
					Requester->Client_ToastMessage( TEXT("Dropper is Busy") );
					return;
				}
				
				FLuggageData tmpData;
				tmpData.word1 = tmpData.word1.GetRandomAnimal();
				tmpData.word2 = tmpData.word2.GetRandomColor();
			
				Dropper->SetSpawnData(tmpData);
				Dropper->SetSpawnClass( LoadClass<AActor>(nullptr, TEXT("/Game/CustomContents/Blueprints/Interactables/BP_Luggage.BP_Luggage_C")));
				Dropper->RequestSpawn();
				return;
			}
		}
	}
}

void APlayerControl::Server_RequestDrop_Implementation()
{
	RequestDrop(this);
}

void APlayerControl::Client_ToastMessage_Implementation(const FString& Message)
{
	UDialogManager::Get(GetWorld())->ShowToast(Message);
}

void APlayerControl::Client_UpdateSpeakQuest_Implementation(int32 StepIndex)
{
	ALingoPlayerState* PS = GetPlayerState<ALingoPlayerState>();
	if (!PS)
		return;

	if (StepIndex == 0)
	{
		// 첫 번째 질문일 경우 MessageBox 표시
		if (auto Popup = UPopupManager::ShowPopupAs<UPopup_SpeakQuest>(GetWorld(), EPopupType::SpeakQuest))
		{
			// MessageBox OK 버튼 클릭 시 질문 표시
			FOnMsgBoxOkDelegate OnOkDelegate;
			
			Popup->InitPopup( FOnMsgBoxOkDelegate().CreateLambda([this, StepIndex]()
			{
				if (APlayerActor* PlayerActor = Cast<APlayerActor>(GetPawn()))
				{
					PlayerActor->PlaySpeakInfo(StepIndex);
				}
				UpdateSpeakWidget(StepIndex);
			}));
		}
	}
	else
	{
		if (APlayerActor* PlayerActor = Cast<APlayerActor>(GetPawn()))
			PlayerActor->PlaySpeakInfo(StepIndex);

		UpdateSpeakWidget(StepIndex);
	}
}

void APlayerControl::Client_EndSpeakQuest_Implementation()
{
	// Quest 완료 시에는 StepIndex를 -1로 전달하여 Widget을 숨김
	UpdateSpeakWidget(-1);

	// if (auto PopupManager = UPopupManager::Get(GetWorld()))
	// 	PopupManager->ShowMsgBox(TEXT("SpeakQuest"), TEXT("QUEST COMPLETE"), EMsgBoxType::OK, FOnMsgBoxOkDelegate());
	RequestSpeakResult();
}

void APlayerControl::Client_RequestSpeakScenario_Implementation(AWheatly* Wheatly)
{
	if (!Wheatly)
	{
		PRINTLOG(TEXT("[APlayerControl] Client_RequestSpeakScenario: Wheatly is null"));
		return;
	}

	// 현재 조종중인 PlayerActor 획득
	APlayerActor* PlayerActor = Cast<APlayerActor>(GetPawn());
	if (!PlayerActor)
	{
		PRINTLOG(TEXT("[APlayerControl] Client_RequestSpeakScenario: PlayerActor is null"));
		return;
	}

	// Client에서 네트워크 요청 수행
	if (auto KLingoNetwork = UKLingoNetworkSystem::Get(GetWorld()))
	{
		// Lambda를 사용하여 응답 처리
		// this 캡처: APlayerControl의 Server RPC 호출을 위해
		KLingoNetwork->RequestSpeakScenario( FResponseSpeakScenarioDelegate::CreateLambda(
				[this, Wheatly](FResponseSpeakScenario& ResponseData, bool bWasSuccessful)
				{
					if (bWasSuccessful && Wheatly)
					{
						// 성공 시 자신의 Server RPC 호출 (PlayerControl은 Client 소유!)
						Server_SyncSpeakScenarioData(Wheatly, ResponseData);
						PRINTLOG(TEXT("[APlayerControl] Client successfully received scenario data, syncing to server"));
					}
					else
					{
						// 실패 시 에러는 이미 ShowNetworkErrorPopup으로 표시됨
						PRINTLOG(TEXT("[APlayerControl] Client failed to receive scenario data"));
					}
				}
			)
		);
	}
	else
	{
		PRINTLOG(TEXT("[APlayerControl] Client_RequestSpeakScenario: Failed to get KLingoNetworkSystem"));
	}
}

void APlayerControl::Server_SyncSpeakScenarioData_Implementation(AWheatly* Wheatly, const FResponseSpeakScenario& Data)
{
	if (!Wheatly)
	{
		PRINTLOG(TEXT("[APlayerControl] Server_SyncSpeakScenarioData: Wheatly is null"));
		return;
	}

	// 현재 조종중인 PlayerActor 획득
	APlayerActor* PlayerActor = Cast<APlayerActor>(GetPawn());
	if (!PlayerActor)
	{
		PRINTLOG(TEXT("[APlayerControl] Server_SyncSpeakScenarioData: PlayerActor is null"));
		return;
	}

	// Wheatly에 데이터 전달 (Server에서 실행됨)
	Wheatly->SyncSpeakScenarioData(PlayerActor, Data);

	PRINTLOG(TEXT("[APlayerControl] Server successfully synced scenario data to Wheatly"));
}

void APlayerControl::UpdateSpeakWidget(int32 StepIndex)
{
	if (APlayerActor* PlayerActor = Cast<APlayerActor>(GetPawn()))
	{
		if (UMainWidget* MainWidget = PlayerActor->GetMainWidget())
		{
			MainWidget->UpdateSpeakWidget(StepIndex);
		}
	}
}

void APlayerControl::RequestSpeakResult()
{
	if ( auto GS = Cast<ALingoGameState>(GetWorld()->GetGameState()) )
	{
		if (auto KLingoNetwork = UKLingoNetworkSystem::Get(GetWorld()))
		{
			FRequestSpeakResult SpeakRequest;
			SpeakRequest.room_id = GS->GetRoomId();
			SpeakRequest.user_id = GetUserId();
			SpeakRequest.scenario_id = 1;
			SpeakRequest.stage_type = ULingoGameHelper::GetStageTypeIndex(EQuestType::Speak);
			SpeakRequest.state_type = 0;
			KLingoNetwork->RequestSpeakResult(SpeakRequest,
					FResponseSpeakResultDelegate::CreateUObject(this, &APlayerControl::OnResponseSpeakResult));
		}	
	}
}

void APlayerControl::OnResponseSpeakResult(FResponseSpeakResult& ResponseData, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if ( auto PS = GetPlayerState<ALingoPlayerState>() )
		{
			// PS에 결과 데이터를 저장한다
			PS->SpeakResult = ResponseData;

			if ( auto Popup = UPopupManager::ShowPopupAs<UPopup_SpeakResult>(GetWorld(), EPopupType::SpeakResult) )
				Popup->InitPopup(ResponseData);
		}
	}
	else
	{
		PRINTLOG(TEXT("[Result] Quest result Failed"));
	}
}

void APlayerControl::TEST_DropperDropProcess()
{
	// 테스트 : 맵에 있는 ADropper를 찾아서 ALuggage를 스폰
	if ( HasAuthority())
		RequestDrop(this);
	else
		Server_RequestDrop();
}

void APlayerControl::TEST_AddItemToBoxList()
{
	// 테스트: 랜덤 아이템 추가
	if (auto BM = UBroadcastManager::Get(GetWorld()))
	{
		TArray<FResultStatData> TestItems;

		// 랜덤하게 1~3개의 아이템 생성
		int32 ItemCount = FMath::RandRange(1, 3);
		for (int32 i = 0; i < ItemCount; ++i)
		{
			FResultStatData Item;

			// 랜덤 위젯 타입 선택
			int32 RandomType = FMath::RandRange(0, 3);
			switch (RandomType)
			{
			case 0: // Grade
				Item.WidgetType = EResultItemWidgetType::Grade;
				Item.GradeTextureType = static_cast<EResourceTextureType>(FMath::RandRange(0, 4)); // Rarity_D ~ Rarity_S
				Item.TitleText = FText::FromString(TEXT("등급"));
				break;

			case 1: // Score
				Item.WidgetType = EResultItemWidgetType::Score;
				Item.ScoreValue = FMath::RandRange(100, 9999);
				Item.TitleText = FText::FromString(TEXT("점수"));
				break;

			case 2: // Rate
				Item.WidgetType = EResultItemWidgetType::Rate;
				Item.RatePercent = FMath::FRandRange(0.0f, 1.0f);
				Item.TitleText = FText::FromString(TEXT("비율"));
				break;

			case 3: // Symbol
				Item.WidgetType = EResultItemWidgetType::Symbol;
				Item.SymbolValue = FString::SanitizeFloat( FMath::FRandRange(0.0f, 1.0f) );
				Item.TitleText = FText::FromString(TEXT("심볼"));
				break;
			}

			// 랜덤 색상 선택
			Item.ColorType = static_cast<EColorStyleType>(FMath::RandRange(0, 7)); // Green ~ Gray

			TestItems.Add(Item);
		}

		// BroadcastManager를 통해 전송
		BM->SendAddItemToBoxList(TestItems);
	}
}

void APlayerControl::ServerRPC_SendChat_Implementation(const FText& inMessage)
{
	if (auto* GS = GetWorld()->GetGameState<ALingoGameState>())
	{
		// PRINTLOG(TEXT("[SendChat] APlayerControl::ServerRPC_SendChat: %s"), *inMessage.ToString());
		GS->MulticastRPC_SendChat(UserInfo, inMessage);
	}
}
