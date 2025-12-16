// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "FoodHolder.h"

#include "ADoor.h"
#include "ALingoGameState.h"
#include "Food.h"
#include "GameLogging.h"
#include "ANetworkBroadcastActor.h"
#include "FoodCourtManager.h"
#include "Popup_Result.h"
#include "UBroadcastManager.h"
#include "UPopupManager.h"
#include "Onepiece/Onepiece.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

AFoodHolder::AFoodHolder()
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
}

void AFoodHolder::BeginPlay()
{
	Super::BeginPlay();

	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AFoodHolder::OnFoodBoxOverlapBegin);

	// 머티리얼 파라미터 초기화 (비활성화 상태)
	UpdateActivateState(false);
}

void AFoodHolder::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// CurTarget이 유효하고 활성화된 상태라면 회전
	if (bIsActivated && CurTarget)
	{
		FRotator CurrentRotation = CurTarget->GetActorRotation();
		CurrentRotation.Yaw += RotationSpeed * DeltaTime;
		CurTarget->SetActorRotation(CurrentRotation);
	}
}

void AFoodHolder::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFoodHolder, bIsActivated);
	DOREPLIFETIME(AFoodHolder, CurTarget);
	DOREPLIFETIME(AFoodHolder, TryIdx);
}

void AFoodHolder::OnRep_IsActivated()
{
	// bIsActivated가 복제될 때 머티리얼 업데이트
	UpdateActivateState(bIsActivated);
}

void AFoodHolder::OnRep_CurTarget()
{
	// CurTarget이 복제될 때 클라이언트에서도 충돌 비활성화
	if (CurTarget)
	{
		if (AFood* Food = Cast<AFood>(CurTarget))
		{
			if (Food->Mesh)
			{
				Food->Mesh->SetSimulatePhysics(false);
				Food->Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
			PRINTLOG(TEXT("AFoodHolder::OnRep_CurTarget - Disabled collision for Food on client"));
		}
	}
}

void AFoodHolder::SetAnswerFoodIndex(int32 InAnswerFoodIndex)
{
	this->AnswerFoodIndex = InAnswerFoodIndex;
}

void AFoodHolder::OnFoodBoxOverlapBegin(
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

	if (bIsActivated)
		return;

	ALingoGameState* GS = Cast<ALingoGameState>(GetWorld()->GetGameState());
	if (!GS)
		return;

	TryIdx++;

	// Food인지 확인
	if (AFood* Food = Cast<AFood>(OtherActor))
	{
		const bool bSuccess = CheckFood(Food);

		FScenarioTargetData TempData;
		TempData.word1 = Food->CurrentFoodData.word1;
		TempData.word2 = Food->CurrentFoodData.word2;
		GS->TryListenAnswerData.target_data.Add(TempData);
		
		// 블루프린트 이벤트 호출
		OnActivate(bSuccess);

		if (bSuccess)
		{
			UE_LOG(LogTemp, Warning, TEXT("[FoodHolder] Correct"));
		
			// 정답인 경우
			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle, [this]
			{
				ADoor* Door = FindDoorToOpen();
				if (Door) Door->OpenDoor();
				
				// 모든 클라이언트에 정답 인덱스와 함께 결과 팝업 표시
				Multicast_ShowResultPopup(TryIdx);
			}, 0.5f, false);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[FoodHolder] Wrong"));
		
			// 오답인 경우
			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle, [this, Food, GS]
			{
				GS->WrongListenAnswerList.Add(TryIdx);
		
				// 모든 클라이언트에 오답 메시지 표시
				Multicast_ShowWrongPopup(Food->CurrentFoodData.word1.name);
		
				// Food 소거 (서버에서만, 자동 복제됨)
				Food->Destroy();

				// 새 FoodContainer 생성
				AActor* FoodContainerManager = UGameplayStatics::GetActorOfClass(GetWorld(), AFoodCourtManager::StaticClass());
				if (AFoodCourtManager* FCManager = Cast<AFoodCourtManager>(FoodContainerManager))
				{
					FCManager->SpawnFoodContainer();
				}
				
			}, 0.5f, false);
		}
	}
}

bool AFoodHolder::CheckFood(AFood* TargetFood)
{
	if (!TargetFood) return false;

	// Food의 모든 충돌 비활성화
	if (TargetFood->Mesh)
	{
		TargetFood->Mesh->SetSimulatePhysics(false);
		TargetFood->Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// ListenQuest 정답 인덱스 가져오기
	ALingoGameState* GS = Cast<ALingoGameState>(GetWorld()->GetGameState());
	if (!GS) return false;

	const int32 CorrectIdx = GS->GetListenScenarioData().correct_answer_index;
	const TArray<FScenarioTargetData>& ScenarioData = GS->GetListenScenarioData().target_data;
	
	FString CorrectCityName = ScenarioData[CorrectIdx].word1.name;
	FString CorrectFoodName = ScenarioData[CorrectIdx].word2.name;
	
	if (CorrectFoodName == TargetFood->CurrentFoodData.word2.name
		&& CorrectCityName == TargetFood->CurrentFoodData.word1.name)
	{
		// Success: Food를 HoldPos 위치보다 살짝 위에 배치
		FVector ActivatedLocation = HoldPos->GetComponentLocation();
		ActivatedLocation.Z += ActivatedHeightOffset;
		TargetFood->SetActorLocation(ActivatedLocation);
		TargetFood->SetActorRotation(HoldPos->GetComponentRotation());
	
		// Activate 상태로 전환
		bIsActivated = true;
		CurTarget = TargetFood;
	
		// 서버에서도 머티리얼 업데이트 (클라이언트는 OnRep_IsActivated에서 호출됨)
		UpdateActivateState(true);
	
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[CheckFood] Returning FALSE"));
	
		// Fail: 서버에서 머티리얼 업데이트 (오답)
		bIsActivated = false;
		UpdateActivateState(false);
	
		return false;
	}
}

void AFoodHolder::UpdateActivateState(bool State)
{
	// 머티리얼 파라미터 설정
	if (MeshComponent && MeshComponent->GetNumMaterials() > 0)
	{
		UMaterialInstanceDynamic* DynamicMaterial = Cast<UMaterialInstanceDynamic>(MeshComponent->GetMaterial(0));
		if (!DynamicMaterial)
			DynamicMaterial = MeshComponent->CreateDynamicMaterialInstance(0);

		if (DynamicMaterial)
			DynamicMaterial->SetScalarParameterValue(FName("Activate"), State ? 1.0f : 0.0f);
	}
}

class ADoor* AFoodHolder::FindDoorToOpen()
{
	TArray<AActor*> CityNames;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADoor::StaticClass(), CityNames);

	for (auto CityName : CityNames)
	{
		if (ADoor* CN = Cast<ADoor>(CityName))
		{
			if (CN->DoorIndex == Index)
			{
				return CN;
			}
		}
	}
	return nullptr;
}

/**
 * @brief [Multicast RPC] 모든 클라이언트에 정답 결과 팝업 표시
 * @details [문제] 서버에서만 팝업을 표시하여 클라이언트에서 보이지 않음
 *          [해결] Multicast RPC로 모든 머신에 팝업 전달
 */
void AFoodHolder::Multicast_ShowResultPopup_Implementation(int32 CorrectAnswerIndex)
{
	// 모든 클라이언트에서 로컬 GameState에 정답 인덱스 추가
	if (ALingoGameState* GS = Cast<ALingoGameState>(GetWorld()->GetGameState()))
	{
		// 중복 체크 후 추가
		if (!GS->WrongListenAnswerList.Contains(CorrectAnswerIndex))
		{
			GS->WrongListenAnswerList.Add(CorrectAnswerIndex);
			PRINTLOG(TEXT("[Multicast_ShowResultPopup] Added correct answer index %d to local GameState"), CorrectAnswerIndex);
		}
	}

	// 팝업 표시
	if (auto Popup = UPopupManager::ShowPopupAs<UPopup_Result>(GetWorld(), EPopupType::Result))
	{
		Popup->InitPopup(EQuestType::Listen);
	}
}

/**
 * @brief [Multicast RPC] 모든 클라이언트에 오답 메시지 표시
 * @details [문제] 서버에서만 팝업을 표시하여 클라이언트에서 보이지 않음
 *          [해결] Multicast RPC로 모든 머신에 팝업 전달
 * @param FoodName 선택한 Food 이름
 */
void AFoodHolder::Multicast_ShowWrongPopup_Implementation(const FString& FoodName)
{
	// 모든 클라이언트(호스트 포함)에서 오답 메시지 표시
	FString Message = FString::Printf(TEXT("Wrong Answer\nThis is not the correct Answer.\n\nFood: %s"),
		*FoodName);

	if (auto DM = UBroadcastManager::Get(this))
		DM->SendTutorMessage(FText::FromString(Message));
}
