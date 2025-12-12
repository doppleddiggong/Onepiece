// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "AWeightSwitch.h"

#include "ALingoGameState.h"
#include "APlayerActor.h"
#include "GameLogging.h"
#include "luggage.h"
#include "Popup_Result.h"
#include "UBroadcastManager.h"
#include "UKLingoNetworkSystem.h"
#include "UPopupManager.h"
#include "UPopup_MsgBox.h"
#include "UTweenAnimInstance.h"
#include "Components/BoxComponent.h"

AWeightSwitch::AWeightSwitch()
{
	PrimaryActorTick.bCanEverTick = true;

	/**
	 * [개선] Replication 설정 추가
	 * - Multicast RPC가 작동하려면 Actor가 복제되어야 함
	 * - bReplicates: Actor를 네트워크로 복제
	 * - bAlwaysRelevant: 모든 클라이언트에 항상 관련성 유지 (중요한 게임플레이 Actor)
	 */
	bReplicates = true;
	bAlwaysRelevant = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	SwitchBody = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SwitchBody"));
	SwitchBody->SetupAttachment(RootComponent);
	SwitchBody->SetRelativeRotation(FRotator(0.f, 0.f, 90.0f));
	SwitchBody->SetRelativeLocation(FVector::ZeroVector);
	SwitchBody->SetRelativeScale3D(FVector(2.f));
	SwitchBody->SetMobility(EComponentMobility::Movable);

	SwitchCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("SwitchCollision"));
	SwitchCollision->SetupAttachment(SwitchBody);
	SwitchCollision->SetBoxExtent(FVector(40.f, 16.f, 50.f));
	SwitchCollision->SetRelativeLocation(FVector::ZeroVector);
	SwitchCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SwitchCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SwitchCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SwitchCollision->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
}

void AWeightSwitch::BeginPlay()
{
	Super::BeginPlay();

	this->TriggerDelay = Duration;
	this->DetectTarget = false;
	this->ElapsedTime = 0;
	this->bActivateState = false;
	
	SwitchCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeightSwitch::OnBeginOverlap);
	SwitchCollision->OnComponentEndOverlap.AddDynamic(this, &AWeightSwitch::OnEndOverlap);

	UBroadcastManager::Get(GetWorld())->OnWeightSwitch.AddDynamic(this, &AWeightSwitch::OnWeightSwitch);
	
	InitSwitch();
}

void AWeightSwitch::InitSwitch()
{
	if (!SwitchBody)
	{
		PRINTLOG( TEXT("SwitchBody is null"));
		return;
	}

	// 1. 머티리얼 얻기
	UMaterialInterface* Material = SwitchBody->GetMaterial(0);
	if (!Material)
	{
		PRINTLOG( TEXT("Material is null"));
		return;
	}

	// 2. 다이나믹 머티리얼 인스턴스 생성
	UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(Material, this);
	if (!DynMaterial)
	{
		PRINTLOG( TEXT("Failed to create dynamic material"));
		return;
	}

	// 3. 변수에 저장
	EmissiveMaterial = DynMaterial;

	// 4. 메시에 적용 (이 단계가 누락되어 있었음!)
	SwitchBody->SetMaterial(0, EmissiveMaterial);

	// 5. 파라미터 설정
	EmissiveMaterial->SetVectorParameterValue(EmissiveParam, DeactivateColor);

	UAnimInstance* AnimInstance = SwitchBody->GetAnimInstance();
	if (!AnimInstance)
	{
		PRINTLOG( TEXT("AnimInstance is null"));
		return;
	}

	// 7. 캐스팅 및 변수 설정
	UTweenAnimInstance* CubeButtonAnim = Cast<UTweenAnimInstance>(AnimInstance);
	if (CubeButtonAnim)
	{
		AnimBlueprint = CubeButtonAnim;
	}
	else
	{
		PRINTLOG( TEXT("Failed to cast to UABP_CubeButton"));
	}
}

void AWeightSwitch::SetActivate(bool State)
{
	if (!EmissiveMaterial)
	{
		PRINTLOG( TEXT("EmissiveMaterial is null"));
		return;
	}

	// 1. 색상 선택 (State가 true면 ActivateColor, false면 DeactivateColor)
	FLinearColor SelectedColor = State ? ActivateColor : DeactivateColor;

	// 2. 머티리얼 파라미터 설정
	EmissiveMaterial->SetVectorParameterValue(EmissiveParam, SelectedColor);

	// 3. 애님 블루프린트의 ChangeState 호출
	if (AnimBlueprint)
	{
		AnimBlueprint->ChangeState(State);
	}
	else
	{
		PRINTLOG( TEXT("AnimBlueprint is null"));
	}

	OnActivate(State);
}

void AWeightSwitch::OnActivate_Implementation(const bool State)
{
	if ( State )
	{
		PRINT_STRING(TEXT("CLICK ON"));
	}
	else
	{
		PRINT_STRING(TEXT("CLICK OFF"));
	}	
}

void AWeightSwitch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if ( DetectTarget == false)
		return;

	ElapsedTime += GetWorld()->GetDeltaSeconds();
	
	if( ActivateTrigger() )
	{
		// 물건이 TriggerDelay 타임 이상 올라가면 발동
		UBroadcastManager::Get(GetWorld())->SendWeightSwitch(ButtonIndex, true);
	}
}

bool AWeightSwitch::ActivateTrigger()
{
	TriggerDelay -= GetWorld()->GetDeltaSeconds();

	if ( TriggerDelay < 0 )
	{
		TriggerDelay = Duration;
		return true;
	}

	return false;
}

/**
 * @brief WeightSwitch Overlap 시작 처리
 * @details [문제] 클라이언트에서도 OnBeginOverlap이 호출되어 정답 판정 시도
 *                 - GetGameState()가 클라이언트에서 nullptr이거나 복제 지연 가능
 *                 - 정답 판정은 서버에서만 수행해야 함
 *          [해결] HasAuthority() 체크로 서버 전용 실행
 *                 GameState nullptr 체크 추가
 */
void AWeightSwitch::OnBeginOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor)
		return;

	// [개선] 서버에서만 정답 판정 수행
	if (!HasAuthority())
	{
		PRINTLOG(TEXT("OnBeginOverlap: Client detected overlap, skipping (server will handle)"));
		return;
	}

	// 조건 1 : 플레이어일 경우 오픈
	if ( IsPlayerDetect )
	{
		if (Cast<APlayerActor>(OtherActor))
		{
			// 리스트에 추가 (중복 방지)
			OverlappingActors.AddUnique(OtherActor);

			// 첫 번째 물체가 올라갔을 때만 타이머 시작
			if (OverlappingActors.Num() == 1)
			{
				this->DetectTarget = true;
				this->ElapsedTime = 0.0;
			}
		}
	}

	// 조건 2 : 정답 캐리어일 경우 오픈
	if (Aluggage* Luggage = Cast<Aluggage>(OtherActor))
	{
		// [개선] GameState nullptr 체크
		ALingoGameState* GS = Cast<ALingoGameState>(GetWorld()->GetGameState());
		if (!GS)
		{
			PRINTLOG(TEXT("OnBeginOverlap: GameState is null! Cannot validate answer."));
			return;
		}

		// [개선] ScenarioData 유효성 체크
		if (GS)
		{
			const int32 CorrectIdx = GS->GetReadScenarioData().correct_answer_index;

			PRINTLOG(TEXT("[WeightSwitch] Server validating: LuggageIdx=%d, CorrectIdx=%d"),
				Luggage->GetSpawnIdx(), CorrectIdx);

			if (CorrectIdx == Luggage->GetSpawnIdx())
			{
				// 리스트에 추가 (중복 방지)
				OverlappingActors.AddUnique(OtherActor);

				// 첫 번째 물체가 올라갔을 때만 타이머 시작
				if (OverlappingActors.Num() == 1)
				{
					this->DetectTarget = true;
					this->ElapsedTime = 0.0;
				}

				if (AnswerFound) return;

				AnswerFound = true;


				// [개선] Multicast RPC로 모든 클라이언트에 정답 팝업 표시 (정답 인덱스 전달)
				FTimerHandle TimerHandle;
				GetWorldTimerManager().SetTimer(TimerHandle, [this, GS, Luggage]
				{
					int32 LuggageIdx = Luggage->GetSpawnIdx();
					// 모든 클라이언트에 정답 인덱스와 함께 결과 팝업 표시
					Multicast_ShowResultPopup(LuggageIdx);

					// 오답 캐리어 로그 (서버에서만)
					TArray<int32> WrongList = GS->WrongReadAnswerList;
					if (WrongList.Num() == 0)
						return;

					PRINTLOG( TEXT("[AWeightSwitch] Wrong luggage :"));
					for (auto Wrong : WrongList)
					{
						PRINTLOG( TEXT("%d, "), Wrong);
					}
				}, 0.5f, false);
			}
			else
			{
				// [개선] Multicast RPC로 모든 클라이언트에 오답 팝업 표시
				FTimerHandle TimerHandle;
				GetWorldTimerManager().SetTimer(TimerHandle, [this, GS, Luggage]
				{
					FString LuggageColor = Luggage->GetColor();
					FString LuggagePattern = Luggage->GetPattern();
					int32 LuggageIdx = Luggage->GetSpawnIdx();

					// 모든 클라이언트에 오답 메시지 표시
					Multicast_ShowWrongPopup(LuggageColor, LuggagePattern);

					// 오답 목록에 인덱스 추가 (서버에서만)
					GS->AddWrongReadAnswer(LuggageIdx);

					// 큐브 소거 (서버에서만, 자동 복제됨)
					Luggage->Destroy();
				}, 0.5f, false);
			}
			
		}
	}
}

/**
 * @brief WeightSwitch Overlap 종료 처리
 * @details [개선] 서버에서만 실행
 */
void AWeightSwitch::OnEndOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor)
		return;

	// [개선] 서버에서만 실행
	if (!HasAuthority())
		return;

	// 리스트에서 제거
	OverlappingActors.Remove(OtherActor);

	// 모든 물체가 내려갔을 때만 비활성화
	if (OverlappingActors.Num() == 0)
	{
		this->DetectTarget = false;
		this->ElapsedTime = 0.0;

		// 물건이 모두 떨어지면 해제
		UBroadcastManager::Get(GetWorld())->SendWeightSwitch(ButtonIndex, false);
	}
}


void AWeightSwitch::OnWeightSwitch(int InButtonIndex, bool InActive)
{
	if ( InButtonIndex != ButtonIndex)
		return;

	if (this->bActivateState == InActive)
		return;

	this->bActivateState = InActive;

	SetActivate(bActivateState);
}

/**
 * @brief [Multicast RPC] 모든 클라이언트에 정답 결과 팝업 표시
 * @details [문제] 서버에서만 팝업을 표시하여 클라이언트에서 보이지 않음
 *          [해결] Multicast RPC로 모든 머신에 팝업 전달
 */
void AWeightSwitch::Multicast_ShowResultPopup_Implementation(int32 CorrectAnswerIndex)
{
	// 모든 클라이언트에서 로컬 GameState에 정답 인덱스 추가
	if (ALingoGameState* GS = Cast<ALingoGameState>(GetWorld()->GetGameState()))
	{
		GS->AddWrongReadAnswer(CorrectAnswerIndex);
		
		PRINTLOG(TEXT("[Multicast_ShowResultPopup] Added correct answer index %d to local GameState"), CorrectAnswerIndex);
	}

	// 팝업 표시
	if (auto Popup = UPopupManager::ShowPopupAs<UPopup_Result>(GetWorld(), EPopupType::Result))
	{
		Popup->InitPopup(EQuestType::Read);
	}
	
	// // 모든 클라이언트(호스트 포함)에서 정답 팝업 표시
	// if (UPopupManager* PopupMgr = UPopupManager::Get(GetWorld()))
	// {
	// 	PopupMgr->ShowResult();
	// 	PRINTLOG(TEXT("[WeightSwitch] Showing result popup on %s"),
	// 		HasAuthority() ? TEXT("Server") : TEXT("Client"));
	// }
}

/**
 * @brief [Multicast RPC] 모든 클라이언트에 오답 메시지 표시
 * @details [문제] 서버에서만 팝업을 표시하여 클라이언트에서 보이지 않음
 *          [해결] Multicast RPC로 모든 머신에 팝업 전달
 * @param LuggageColor 선택한 Luggage 색상
 * @param LuggagePattern 선택한 Luggage 무늬
 */
void AWeightSwitch::Multicast_ShowWrongPopup_Implementation(const FString& LuggageColor, const FString& LuggagePattern)
{
	// 모든 클라이언트(호스트 포함)에서 오답 메시지 표시
	if (UPopupManager* PopupMgr = UPopupManager::Get(GetWorld()))
	{
		// FString Title = TEXT("Wrong Answer!");
		FString Message = FString::Printf(TEXT("Wrong Answer\nThis is not the correct Answer.\n\nColor: %s\nPattern: %s"),
			*LuggageColor, *LuggagePattern);

		if (auto DM = UBroadcastManager::Get(this))
			DM->SendTutorMessage(FText::FromString(Message));

		// PopupMgr->ShowMsgBox(Title, Message, EMsgBoxType::OK,
		// 	FOnMsgBoxOkDelegate::CreateLambda([]() {
		// 		// 확인 버튼 클릭 시 아무 작업도 하지 않음 (팝업만 닫힘)
		// 	}));
		//
		// PRINTLOG(TEXT("[WeightSwitch] Showing wrong popup on %s (Color: %s, Pattern: %s)"),
		// 	HasAuthority() ? TEXT("Server") : TEXT("Client"), *LuggageColor, *LuggagePattern);
	}
}