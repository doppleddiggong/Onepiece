// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "APlatformSwitch.h"

#include "FComponentHelper.h"
#include "FMaterialHelper.h"
#include "Kismet/KismetMathLibrary.h"

#define SWITCH_BUTTON_PATH			TEXT("Mesh_SwitchButton")
#define SWITCH_COLLISION_PATH		TEXT("Collision")

APlatformSwitch::APlatformSwitch()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APlatformSwitch::BeginPlay()
{
	Super::BeginPlay();

	this->TriggerDelay = Duration;
	this->DetectTarget = false;
	this->ElapsedTime = 0;
	this->bActivateState = false;

	SwitchButton = FComponentHelper::FindComponentByNameRecursive<UStaticMeshComponent>(this, SWITCH_BUTTON_PATH);
	SwitchCollision = FComponentHelper::FindComponentByNameRecursive<UPrimitiveComponent>(this, SWITCH_COLLISION_PATH);

	if( SwitchButton != nullptr)
	{
		MaterialButton = SwitchButton->CreateDynamicMaterialInstance(0);
		MaterialButton->SetVectorParameterValue( ColorParam, IdleColor );
		OriginVector = SwitchButton->GetRelativeLocation();

		SwitchCollision->OnComponentBeginOverlap.AddDynamic(this, &APlatformSwitch::OnBeginOverlap);
		SwitchCollision->OnComponentEndOverlap.AddDynamic(this, &APlatformSwitch::OnEndOverlap);
	}
}

void APlatformSwitch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if ( DetectTarget == false)
		return;

	AddElapsedTime();

	const auto AlphaValue = LerpAlpha();
	// ULOG(Warning, "LerpAlpha : %f", LerpAlpha() );

	{
		const auto Color_A = FMaterialHelper::GetVectorParameterValueSafe(MaterialButton, ColorParam);
		const auto Color_B = WarningColor;
		const auto Color_Result = UKismetMathLibrary::LinearColorLerpUsingHSV(Color_A, Color_B, AlphaValue);
		
		MaterialButton->SetVectorParameterValue( ColorParam, Color_Result );
	}

	{
		const auto Vector_A = SwitchButton->GetRelativeLocation();
		const auto Vector_B = EndVector;
		const auto Vector_Result = UKismetMathLibrary::VLerp( Vector_A, Vector_B, AlphaValue);

		SwitchButton->SetRelativeLocation(Vector_Result);
	}

	if( ActivateTrigger() )
	{
		// 물건이 TriggerDelay 타임 이상 올라가면 발동
		ChangeActivateState(true);
	}
}

void APlatformSwitch::AddElapsedTime()
{
	ElapsedTime += GetWorld()->GetDeltaSeconds();
}

float APlatformSwitch::LerpAlpha() const
{
	return UKismetMathLibrary::FClamp( ElapsedTime / Duration, 0.0f, 1.0f );
}

bool APlatformSwitch::ActivateTrigger()
{
	TriggerDelay -= GetWorld()->GetDeltaSeconds();

	if ( TriggerDelay < 0 )
	{
		TriggerDelay = Duration;
		return true;
	}

	return false;
}

void APlatformSwitch::OnBeginOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	this->InOtherActor = OtherActor;

	this->DetectTarget = true;
	this->ElapsedTime = 0.0;
}

void APlatformSwitch::OnEndOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	this->InOtherActor = nullptr;

	this->DetectTarget = false;
	this->ElapsedTime = 0.0;

	MaterialButton->SetVectorParameterValue( ColorParam, IdleColor );
	SwitchButton->SetRelativeLocation(OriginVector);

	// 물건이 떨어지면 올라가면 해제
	this->ChangeActivateState(false);
}

#pragma region SwitchEvent
void APlatformSwitch::ChangeActivateState(bool State)
{
	if (this->bActivateState == State)
		return;

	this->bActivateState = State;

	OnChangeSwitchState.Broadcast(bActivateState);
}
#pragma endregion	