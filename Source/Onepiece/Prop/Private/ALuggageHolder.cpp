// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "ALuggageHolder.h"
#include "luggage.h"
#include "GameLogging.h"
#include "ANetworkBroadcastActor.h"
#include "ALingoGameState.h"
#include "Popup_Result.h"
#include "UBroadcastManager.h"
#include "UPopupManager.h"
#include "Onepiece/Onepiece.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimationAsset.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

ALuggageHolder::ALuggageHolder()
{
	PrimaryActorTick.bCanEverTick = true;

	// Replication
	bReplicates = true;

	// Root component
	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	RootComponent = Root;

	// Mesh component
	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);

	// Box collision component
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxCollision->SetupAttachment(MeshComponent);
	BoxCollision->SetGenerateOverlapEvents(true);
	BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxCollision->SetCollisionResponseToAllChannels(ECR_Overlap);

	// HoldPos component
	HoldPos = CreateDefaultSubobject<USceneComponent>(TEXT("HoldPos"));
	HoldPos->SetupAttachment(MeshComponent);
	
	WidgetGuideComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetGuideComp"));
	WidgetGuideComp->SetupAttachment(RootComponent);
	WidgetGuideComp->SetRelativeLocation(FVector(0.f, 0.f, 180.f));
	WidgetGuideComp->SetDrawAtDesiredSize(true);
	ConstructorHelpers::FClassFinder<UUserWidget> widgetGuideClassRef(TEXT("/Game/CustomContents/UI/Widgets/WBP_LuggageSlotWidget.WBP_LuggageSlotWidget_C"));
	if (widgetGuideClassRef.Succeeded())
	{
		WidgetGuideComp->SetWidgetClass(widgetGuideClassRef.Class);
	}

	MarkerType = ECompassMarkerType::QuestEnd;
}

void ALuggageHolder::BeginPlay()
{
	Super::BeginPlay();

	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ALuggageHolder::OnBoxOverlapBegin);

	// 머티리얼 파라미터 초기화 (비활성화 상태)
	UpdateActivateState(false);
}

void ALuggageHolder::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// CurrentLuggage가 유효하고 활성화된 상태라면 회전
	if (bIsActivated && CurTarget)
	{
		FRotator CurrentRotation = CurTarget->GetActorRotation();
		CurrentRotation.Yaw += RotationSpeed * DeltaTime;
		CurTarget->SetActorRotation(CurrentRotation);
	}
	
	BillboardInteractWidget();
}

void ALuggageHolder::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALuggageHolder, bIsActivated);
	DOREPLIFETIME(ALuggageHolder, CurTarget);
}

void ALuggageHolder::OnRep_IsActivated()
{
	// bIsActivated가 복제될 때 머티리얼 업데이트
	UpdateActivateState(bIsActivated);
}

void ALuggageHolder::OnRep_CurTarget()
{
	// CurTarget이 복제될 때 클라이언트에서도 충돌 비활성화
	if (CurTarget)
	{
		if (Aluggage* Luggage = Cast<Aluggage>(CurTarget))
		{
			Luggage->SetAllCollision(false);
			PRINTLOG(TEXT("AHolder::OnRep_CurTarget - Disabled collision for Luggage on client"));
		}
	}
}

void ALuggageHolder::OnBoxOverlapBegin(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!OtherActor)
		return;

	if (!HasAuthority())
		return;

	if ( bIsActivated )
		return;

	ALingoGameState* GS = Cast<ALingoGameState>(GetWorld()->GetGameState());
	if (!GS)
		return;
	
	// Luggage인지 확인
	if (auto Luggage = Cast<Aluggage>(OtherActor))
	{
		// Hook 중인 Luggage는 무시 (PlayerActor가 들고 있는 상태)
		if (Luggage->bIsBeingHooked)
			return;

		const bool bSuccess = CheckLuggage(Luggage, GS->GetReadScenarioData().correct_answer_index );

		// 블루프린트 이벤트 호출
		OnActivate(bSuccess);

		if (bSuccess)
		{
			// 정답인 경우
			int32 CorrectIdx = Luggage->GetSpawnIdx();

			// 마커 표시
			GS->SetAllCompassVisibility(false);
			GS->SetCompassVisibilityByTag("ReadQuestEnd", true);
			
			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle, [this, CorrectIdx]
			{
				// 모든 클라이언트에 정답 인덱스와 함께 결과 팝업 표시
				Multicast_ShowResultPopup(CorrectIdx);
				
			}, 0.5f, false);
		}
		else
		{
			// 오답인 경우
			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle, [this, Luggage, GS]
			{
				GS->AddWrongReadAnswer(Luggage->GetSpawnIdx());

				// 모든 클라이언트에 오답 메시지 표시
				Multicast_ShowWrongPopup(Luggage->GetColor(), Luggage->GetPattern());

				// Dissolve 재생
				GetWorldTimerManager().SetTimer(DestroyTimerHandle, [this, Luggage]
				{
					if (Luggage->UpdateDissolve())
					{
						GetWorldTimerManager().ClearTimer(DestroyTimerHandle);
						// 큐브 소거 (서버에서만, 자동 복제됨)
						Luggage->Destroy();
					}
				},GetWorld()->DeltaTimeSeconds, true);
				
			}, 0.5f, false);
		}
	}
}

bool ALuggageHolder::CheckLuggage(Aluggage* TargetLuggage, int CorrectIndex)
{
	if (!TargetLuggage)
		return false;

	// Luggage의 모든 충돌 비활성화 (pickup, hook 등 모든 상호작용 차단)
	TargetLuggage->SetAllCollision(false);

	// ReadQuest 정답 인덱스 가져오기
	if (CorrectIndex == TargetLuggage->GetSpawnIdx())
	{
		// Success: Luggage를 HoldPos 위치보다 살짝 위에 배치
		FVector ActivatedLocation = HoldPos->GetComponentLocation();
		ActivatedLocation.Z += ActivatedHeightOffset;
		TargetLuggage->SetActorLocation(ActivatedLocation);
		TargetLuggage->SetActorRotation(HoldPos->GetComponentRotation());

		// Activate 상태로 전환
		bIsActivated = true;
		CurTarget = TargetLuggage;

		// 서버에서도 머티리얼 업데이트 (클라이언트는 OnRep_IsActivated에서 호출됨)
		UpdateActivateState(true);

		return true;
	}
	else
	{
		// Fail: 서버에서 머티리얼 업데이트 (오답)
		bIsActivated = false;
		UpdateActivateState(false);

		return false;
	}
}

void ALuggageHolder::UpdateActivateState(bool State)
{
	// 머티리얼 파라미터 설정 (비활성화)
	if (MeshComponent && MeshComponent->GetNumMaterials() > 0)
	{
		UMaterialInstanceDynamic* DynamicMaterial = Cast<UMaterialInstanceDynamic>(MeshComponent->GetMaterial(0));
		if (!DynamicMaterial)
			DynamicMaterial = MeshComponent->CreateDynamicMaterialInstance(0);

		if (DynamicMaterial)
			DynamicMaterial->SetScalarParameterValue(FName("Activate"), State ? 1.0f : 0.0f);
	}
}

void ALuggageHolder::BillboardInteractWidget()
{
	// 위젯이 없으면 빌보드화 안 함
	if (!WidgetGuideComp)
		return;

	// Visibility 체크 - 보이지 않으면 빌보드화 안 함
	if (!WidgetGuideComp->IsVisible())
		return;

	// 카메라 가져오기
	AActor* Camera = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	if (!Camera)
		return;

	// 카메라를 향하도록 회전 계산
	FRotator Rotation = UKismetMathLibrary::MakeRotFromXZ( -Camera->GetActorForwardVector(), Camera->GetActorUpVector() );
	Rotation.Pitch = 0;

	// 위젯 회전 설정
	WidgetGuideComp->SetWorldRotation(Rotation);
}

void ALuggageHolder::SetCompassMarkerInto(ECompassMarkerType InMarkerType)
{
	MarkerType = InMarkerType;
}

/**
 * @brief [Multicast RPC] 모든 클라이언트에 정답 결과 팝업 표시
 * @details [문제] 서버에서만 팝업을 표시하여 클라이언트에서 보이지 않음
 *          [해결] Multicast RPC로 모든 머신에 팝업 전달
 */
void ALuggageHolder::Multicast_ShowResultPopup_Implementation(int32 CorrectAnswerIndex)
{
	// 모든 클라이언트에서 로컬 GameState에 정답 인덱스 추가
	if (ALingoGameState* GS = Cast<ALingoGameState>(GetWorld()->GetGameState()))
	{
		GS->AddWrongReadAnswer(CorrectAnswerIndex);
	}

	// 팝업 표시
	if (auto Popup = UPopupManager::ShowPopupAs<UPopup_Result>(GetWorld(), EPopupType::Result))
	{
		Popup->InitPopup(EQuestType::Read);
	}
}

/**
 * @brief [Multicast RPC] 모든 클라이언트에 오답 메시지 표시
 * @details [문제] 서버에서만 팝업을 표시하여 클라이언트에서 보이지 않음
 *          [해결] Multicast RPC로 모든 머신에 팝업 전달
 * @param LuggageColor 선택한 Luggage 색상
 * @param LuggagePattern 선택한 Luggage 무늬
 */
void ALuggageHolder::Multicast_ShowWrongPopup_Implementation(const FString& LuggageColor, const FString& LuggagePattern)
{
	// 모든 클라이언트(호스트 포함)에서 오답 메시지 표시
	FString StyledMessage = FString::Printf(
			TEXT("WRONG ANSWER\n")
			TEXT("------------------\n")
			TEXT("The selected item does not match.\n")
			TEXT("Color  : %s\n")
			TEXT("Pattern: %s\n")
			TEXT("------------------\n")
			TEXT("Please try another luggage."),
			*LuggageColor, *LuggagePattern
			);

	if (auto DM = UBroadcastManager::Get(this))
		DM->SendTutorMessage(FText::FromString(StyledMessage));
}