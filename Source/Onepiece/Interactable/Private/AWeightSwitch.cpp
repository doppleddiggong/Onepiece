// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "AWeightSwitch.h"
#include "GameLogging.h"
#include "UBroadcastManager.h"
#include "UTweenAnimInstance.h"
#include "Components/BoxComponent.h"

AWeightSwitch::AWeightSwitch()
{
	PrimaryActorTick.bCanEverTick = true;

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

void AWeightSwitch::OnBeginOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor)
		return;

	// 리스트에 추가 (중복 방지)
	OverlappingActors.AddUnique(OtherActor);

	// 첫 번째 물체가 올라갔을 때만 타이머 시작
	if (OverlappingActors.Num() == 1)
	{
		this->DetectTarget = true;
		this->ElapsedTime = 0.0;
	}
}

void AWeightSwitch::OnEndOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor)
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