// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "InteractableComponent.h"

#include "APlayerActor.h"
#include "ALingoPlayerState.h"
#include "AWheatly.h"
#include "ASpeakStageActor.h"
#include "ULingoGameHelper.h"
#include "GameLogging.h"
#include "luggage.h"
#include "UInteractWidget.h"
#include "Net/UnrealNetwork.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "UInteractionSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


UInteractableComponent::UInteractableComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	bIsPickedUp = false;
	bOriginalSimulatePhysics = false;
	OriginalCollisionType = ECollisionEnabled::NoCollision;

	// 생성자에서 생성해서 자동 복제
	DetectionRange = CreateDefaultSubobject<UBoxComponent>(TEXT("DetectionRange"));
	if (DetectionRange)
	{
		DetectionRange->SetBoxExtent(FVector(250.f)); // DetectionDistance 기본값
		DetectionRange->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		DetectionRange->SetCollisionResponseToAllChannels(ECR_Ignore);
		DetectionRange->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	}

	SetIsReplicatedByDefault(true);
}

void UInteractableComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		PRINTLOG( TEXT("UInteractableComponent::BeginPlay - GetOwner() returned nullptr!"));
		return;
	}

	this->InitDetectionRange();
}

void UInteractableComponent::InitDetectionRange()
{
	// 1. 컴포넌트 등록                                                                                                                                         
	if (!DetectionRange->IsRegistered())
	{
		DetectionRange->RegisterComponent();
	}

	AActor* Owner = GetOwner();
	
	// 2. Owner의 RootComponent에 부착                                                                                                                                         
	if (!DetectionRange->IsAttachedTo(Owner->GetRootComponent()))
	{
		DetectionRange->AttachToComponent(
			Owner->GetRootComponent(),
			FAttachmentTransformRules::KeepRelativeTransform
		);
	}
	
	// BoxExtent 설정 업데이트                                                                                                         
	FVector BoxExtent(DetectionDistance, DetectionDistance, DetectionDistance);
	DetectionRange->SetBoxExtent(BoxExtent);

	// Overlap 콜백 바인딩                                                                                                                     
	DetectionRange->OnComponentBeginOverlap.AddDynamic(this, &UInteractableComponent::OnDetectionBeginOverlap);
	DetectionRange->OnComponentEndOverlap.AddDynamic(this, &UInteractableComponent::OnDetectionEndOverlap);
}

void UInteractableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 상호작용 위젯 빌보드화
	BillboardInteractWidget();

	// AWheatly인 경우 현재 발화자 확인하여 위젯 동적 제어
	if (WidgetComp && GetOwner() && GetOwner()->IsA(AWheatly::StaticClass()))
	{
		if (UWorld* World = GetWorld())
		{
			// 로컬 플레이어 확인
			if (APlayerController* LocalPC = World->GetFirstPlayerController())
			{
				if (APawn* LocalPawn = LocalPC->GetPawn())
				{
					if (LocalPawn->IsLocallyControlled())
					{
						// SpeakStage에서 현재 발화자 확인
						if (auto SpeakStage = ULingoGameHelper::GetSpeakStageActor(World))
						{
							auto LocalPlayerState = LocalPawn->GetPlayerState<ALingoPlayerState>();
							auto CurrentSpeaker = SpeakStage->GetCurrentSpeaker();
							
							// 현재 발화자이면 위젯 숨김
							if (LocalPlayerState && CurrentSpeaker && LocalPlayerState == CurrentSpeaker)
							{
								if (WidgetComp->IsVisible())
								{
									WidgetComp->SetVisibility(false);
								}
								// 발화자는 위젯이 계속 숨겨져야 하므로 여기서 return
								return;
							}
						}
					}
				}
			}
		}
	}

	// 디버그 표시
	if (bShowDetectionDebug && DetectionRange)
	{
		AActor* Owner = GetOwner();
		if (Owner && IsValid(Owner))
		{
			FVector Center = Owner->GetActorLocation();
			FVector BoxExtent = DetectionRange->GetScaledBoxExtent();
			FQuat Rotation = Owner->GetActorQuat();

			// DetectionRange 박스 그리기
			DrawDebugBox(
				GetWorld(),
				Center,
				BoxExtent,
				Rotation,
				bCanInteract ? FColor::Green : FColor::Yellow,
				false,
				0.0f,
				0,
				2.0f
			);

			// 거리 텍스트 표시
			FString DebugText = FString::Printf(
				TEXT("Detection: %.0f cm\n%s"),
				DetectionDistance,
				*InteractionPrompt
			);

			DrawDebugString(
				GetWorld(),
				Center + FVector(0, 0, BoxExtent.Z + 20.f),
				DebugText,
				nullptr,
				FColor::White,
				0.0f,
				true,
				1.0f
			);
		}
	}
}

void UInteractableComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInteractableComponent, HoldingOwner);

	/**
	 * [문제] bIsPickedUp이 복제되지 않아 클라이언트마다 다른 상태를 가질 수 있었음
	 * [해결] bIsPickedUp 복제 추가로 모든 머신에서 동일한 픽업 상태 유지
	 */
	DOREPLIFETIME(UInteractableComponent, bIsPickedUp);
}

void UInteractableComponent::OnRep_HoldingOwner()
{
	if (HoldingOwner)
	{
		APlayerActor* MyPlayer = Cast<APlayerActor>(HoldingOwner);
		if (MyPlayer)
		{
			// 여기서 GetOwner() == 컴포넌트가 붙어있는 액터
			GetOwner()->AttachToComponent(MyPlayer->HoldPosition, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		}
	}
	else
	{
		GetOwner()->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	}
}

/**
 * @brief bIsPickedUp 복제 시 호출되는 콜백
 * @details [문제] 기존에는 픽업/드롭 시 비주얼 업데이트가 Server_PickUp/Drop에서만 수행되어
 *                 클라이언트에서 상태 동기화 문제 발생
 *          [해결] OnRep으로 모든 클라이언트에서 동일하게 비주얼 업데이트
 */
void UInteractableComponent::OnRep_IsPickedUp()
{
	Aluggage* luggage = Cast<Aluggage>(GetOwner());
	if (!luggage)
		return;

	if (bIsPickedUp)
	{
		// 픽업 상태 - Outline/Widget 끄기
		luggage->OutlineOff();
		HideInteractWidget();

		PRINTLOG(TEXT("OnRep_IsPickedUp: %s picked up (visual update)"), *GetOwner()->GetName());
	}
	else
	{
		// 드롭 상태 - Outline 켜기 (Widget은 거리에 따라 OnDetectionOverlap에서 처리)
		luggage->OutlineOn();

		PRINTLOG(TEXT("OnRep_IsPickedUp: %s dropped (visual update)"), *GetOwner()->GetName());
	}
}

void UInteractableComponent::PickUp(AActor* NewHoldingOwner)
{
	// 클라이언트 측 기본 검증 (빠른 피드백용)
	if (!NewHoldingOwner)
	{
		PRINTLOG(TEXT("InteractableComponent::PickUp - NewHoldingOwner is null"));
		return;
	}

	if (bIsPickedUp)
	{
		PRINTLOG(TEXT("InteractableComponent::PickUp - Already picked up"));
		return;
	}

	// 서버 RPC 호출 (서버에서 재검증)
	Server_PickUp(NewHoldingOwner);
}

/**
 * @brief [서버 RPC Validation] 픽업 요청 검증
 * @details [문제] 기존에는 클라이언트가 제공한 NewHoldingOwner를 무검증으로 신뢰
 *                 - 다른 플레이어의 액터를 지정하여 소유권 탈취 가능
 *                 - 거리 밖 오브젝트도 픽업 가능
 *                 - 중복 픽업 방지 부족
 *          [해결] 서버에서 5가지 검증 수행
 *                 1. NewHoldingOwner 유효성
 *                 2. PlayerController 제어 여부
 *                 3. 중복 픽업 체크
 *                 4. 기존 소유자 존재 체크
 *                 5. 거리 검증 (네트워크 지연 고려 1.5배 여유)
 */
bool UInteractableComponent::Server_PickUp_Validate(AActor* NewHoldingOwner)
{
	// 1. NewHoldingOwner가 유효한지
	if (!NewHoldingOwner)
	{
		PRINTLOG(TEXT("Server_PickUp_Validate: NewHoldingOwner is null"));
		return false;
	}

	// 2. NewHoldingOwner가 실제 PlayerController에 의해 제어되는지
	APlayerActor* PlayerActor = Cast<APlayerActor>(NewHoldingOwner);
	if (!PlayerActor || !PlayerActor->GetController())
	{
		PRINTLOG(TEXT("Server_PickUp_Validate: Not a valid PlayerActor or no Controller"));
		return false;
	}

	// 3. 이미 픽업된 상태인지
	if (bIsPickedUp)
	{
		PRINTLOG(TEXT("Server_PickUp_Validate: Already picked up"));
		return false;
	}

	// 4. HoldingOwner가 이미 있는지 (중복 소유 방지)
	if (HoldingOwner && HoldingOwner != NewHoldingOwner)
	{
		PRINTLOG(TEXT("Server_PickUp_Validate: Already held by %s"), *HoldingOwner->GetName());
		return false;
	}

	// 5. 거리 검증 (서버 기준)
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		PRINTLOG(TEXT("Server_PickUp_Validate: Owner is null"));
		return false;
	}

	float Distance = FVector::Dist(Owner->GetActorLocation(), NewHoldingOwner->GetActorLocation());
	float MaxDistance = DetectionDistance * 1.5f;  // 네트워크 지연 고려 1.5배 여유

	if (Distance > MaxDistance)
	{
		PRINTLOG(TEXT("Server_PickUp_Validate: Too far! Distance: %.2f, Max: %.2f"), Distance, MaxDistance);
		return false;
	}

	return true;
}

/**
 * @brief [서버 RPC] 픽업 실행
 * @details Validation 통과 후 실행됨
 */
void UInteractableComponent::Server_PickUp_Implementation(AActor* NewHoldingOwner)
{
	// Validation 통과 후 실행
	HoldingOwner = NewHoldingOwner;

	// Owner actor의 PrimitiveComponent 찾기
	UPrimitiveComponent* PrimComp = GetOwnerPrimitiveComponent();
	if (!PrimComp)
		return;

	// 원래 상태 저장 (드롭 시 복원용)
	bOriginalSimulatePhysics = PrimComp->IsSimulatingPhysics();
	OriginalCollisionType = PrimComp->GetCollisionEnabled();

	// 물리 끄기
	PrimComp->SetSimulatePhysics(false);

	// Attach (OnRep_HoldingOwner에서 처리)
	OnRep_HoldingOwner();

	// 상태 변경 (복제됨 → OnRep_IsPickedUp 자동 호출)
	bIsPickedUp = true;

	PRINTLOG(TEXT("Server_PickUp: %s picked up by %s"), *GetOwner()->GetName(), *NewHoldingOwner->GetName());
}

void UInteractableComponent::Drop()
{
	// 클라이언트 측 기본 검증 (빠른 피드백용)
	if (!bIsPickedUp)
	{
		PRINTLOG(TEXT("InteractableComponent::Drop - Not picked up"));
		return;
	}

	// 서버 RPC 호출
	Server_Drop();
}

/**
 * @brief [서버 RPC Validation] 드롭 요청 검증
 * @details [문제] 기존에는 Validation 없이 드롭 허용
 *          [해결] 실제로 픽업된 상태인지, 소유자가 유효한지 검증
 */
bool UInteractableComponent::Server_Drop_Validate()
{
	// 1. 실제로 픽업된 상태인지
	if (!bIsPickedUp)
	{
		PRINTLOG(TEXT("Server_Drop_Validate: Not picked up"));
		return false;
	}

	// 2. HoldingOwner가 유효한지
	if (!HoldingOwner)
	{
		PRINTLOG(TEXT("Server_Drop_Validate: HoldingOwner is null"));
		return false;
	}

	return true;
}

/**
 * @brief [서버 RPC] 드롭 실행
 * @details Validation 통과 후 실행됨
 */
void UInteractableComponent::Server_Drop_Implementation()
{
	// Detach (OnRep_HoldingOwner에서 처리됨)
	GetOwner()->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	// 물리 복원
	UPrimitiveComponent* PrimComp = GetOwnerPrimitiveComponent();
	if (PrimComp)
	{
		PrimComp->SetSimulatePhysics(bOriginalSimulatePhysics);
		PrimComp->SetCollisionEnabled(OriginalCollisionType);
	}

	// 상태 변경 (복제됨 → OnRep_IsPickedUp, OnRep_HoldingOwner 자동 호출)
	bIsPickedUp = false;
	HoldingOwner = nullptr;

	PRINTLOG(TEXT("Server_Drop: %s dropped"), *GetOwner()->GetName());
}


UPrimitiveComponent* UInteractableComponent::GetOwnerPrimitiveComponent() const
{
	AActor* Owner = GetOwner();
	if (!Owner)
		return nullptr;

	UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(Owner->GetRootComponent());

	// RootComponent가 PrimitiveComponent가 아니면, 자식 중에서 찾기
	if (!PrimComp)
	{
		TArray<UActorComponent*> Components;
		Owner->GetComponents(UPrimitiveComponent::StaticClass(), Components);

		if (Components.Num() > 0)
		{
			// 첫 번쩨 primitive component 반환
			PrimComp = Cast<UPrimitiveComponent>(Components[0]);
		}
	}

	return PrimComp;
}

void UInteractableComponent::ShowDebugInfo(AActor* ViewerActor)
{
	if (!GetOwner() || !ViewerActor) return;

	// 객체 위치 (약간 위쪽에 표시)
	FVector DebugLocation = GetOwner()->GetActorLocation() + FVector(0.f, 0.f, 100.f);
	
	// InteractionPrompt 사용
	FString DebugMessage = InteractionPrompt;
	
	// 3D 공간에 디버그 문자열 표시
	DrawDebugString(
		GetWorld(),
		DebugLocation,
		DebugMessage,
		nullptr,
		bIsPickedUp ? FColor::Yellow : FColor::Green,
		0.0f,
		true,
		1.2f
	);
}

void UInteractableComponent::TriggerInteraction(AActor* Interactor)
{
	if (!bCanInteract || !Interactor)
		return;

	// 델리게이트 브로드캐스트
	OnInteractionTriggered.Broadcast(Interactor);

	PRINTLOG( TEXT("InteractableComponent::TriggerInteraction - %s triggered by %s"), *GetOwner()->GetName(), *Interactor->GetName());
}

void UInteractableComponent::OnDetectionBeginOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor)
		return;

	// Player 캐릭터인지 확인
	if ( auto Character = Cast<ACharacter>(OtherActor) )
	{
		if ( Character->IsPlayerControlled())
		{
			// InteractionSystem 찾기 및 등록
			if (auto InteractionSystem = Character->FindComponentByClass<UInteractionSystem>())
				InteractionSystem->RegisterInteractable(this);

			bool ShowState = IsWidgetShowEnable(Character);
			if ( ShowState)
				ShowInteractWidget();
		
			// PickUp 타입이면 Luggage의 custom widget 사용
			if (auto luggage = Cast<Aluggage>(GetOwner()))
			{
				if (!bIsPickedUp)
					luggage->OutlineOn();
			}

			PRINTLOG( TEXT("InteractableComponent: Player entered detection range - %s"), *GetOwner()->GetName());
		}
	}
}

bool UInteractableComponent::IsWidgetShowEnable(const ACharacter* Character) const
{
	// 로컬 플레이어일 때만 위젯 표시 (멀티플레이어 버그 수정)
	if (!Character->IsLocallyControlled())
		return false;
	
	if (GetOwner()->IsA(AWheatly::StaticClass()))
	{
		// SpeakStage에서 현재 발화자 확인
		if (auto SpeakStage = ULingoGameHelper::GetSpeakStageActor(GetWorld()))
		{
			auto CurrentSpeaker = SpeakStage->GetCurrentSpeaker();
			auto PS = Character->GetPlayerState<ALingoPlayerState>();
				
			if (PS && CurrentSpeaker && PS == CurrentSpeaker)
			{
				// 현재 발화자이면 위젯을 표시하지 않음
				PRINTLOG(TEXT("[InteractableComponent] Local player is current speaker - hiding widget"));
				return false;
			}
			else
			{
				return true;
			}
		}
	}

	return true;
}

void UInteractableComponent::OnDetectionEndOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor)
		return;

	// Player 캐릭터인지 확인
	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (Character && Character->IsPlayerControlled())
	{
		// InteractionSystem에서 등록 해제
		UInteractionSystem* InteractionSystem = Character->FindComponentByClass<UInteractionSystem>();
		if (InteractionSystem)
		{
			InteractionSystem->UnregisterInteractable(this);
		}

		PRINTLOG( TEXT("InteractableComponent: Player left detection range - %s"), *GetOwner()->GetName());

		// Widget 끄기
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this]
		{
			HideInteractWidget();
			
			Aluggage* luggage = Cast<Aluggage>(GetOwner());
			if (luggage && !bIsPickedUp)
			{
				luggage->OutlineOff();
				// luggage->InfoWidgetOff();
			}
		}), 0.1f, false);
	}
}

#pragma region Widget
void UInteractableComponent::InitWidget(UWidgetComponent* InWidgetComp)
{
	this->WidgetComp = InWidgetComp;

	if (!WidgetComp)
		return;

	WidgetComp->SetVisibility(false); // 처음엔 숨겨두고

	if (auto InteractWidget = Cast<UInteractWidget>(WidgetComp->GetWidget()))
		InteractWidget->InitInfo(TEXT("E"), InteractionPrompt);
}

void UInteractableComponent::ShowInteractWidget()
{
	if (!WidgetComp)
		return;

	if (!WidgetComp->GetWidget())
		return;

	WidgetComp->SetVisibility(true);
}

void UInteractableComponent::HideInteractWidget()
{
	if (!WidgetComp)
		return;

	WidgetComp->SetVisibility(false);
}

void UInteractableComponent::BillboardInteractWidget()
{
	// 위젯이 없으면 빌보드화 안 함
	if (!WidgetComp)
		return;

	// Visibility 체크 - 보이지 않으면 빌보드화 안 함
	if (!WidgetComp->IsVisible())
		return;

	// 카메라 가져오기
	AActor* Camera = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	if (!Camera)
		return;

	// 카메라를 향하도록 회전 계산
	FRotator Rotation = UKismetMathLibrary::MakeRotFromXZ( -Camera->GetActorForwardVector(), Camera->GetActorUpVector() );
	Rotation.Pitch = 0;

	// 위젯 회전 설정
	WidgetComp->SetWorldRotation(Rotation);
}

void UInteractableComponent::UpdateInteractPrompt(const FString& NewPrompt)
{
	// InteractionPrompt 업데이트
	InteractionPrompt = NewPrompt;

	// 위젯이 없으면 리턴
	if (!WidgetComp)
		return;

	// UInteractWidget의 Txt_Desc 업데이트
	if (auto InteractWidget = Cast<UInteractWidget>(WidgetComp->GetWidget()))
		InteractWidget->UpdateDesc(NewPrompt);
}
#pragma endregion