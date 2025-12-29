// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "CityName.h"

#include "CityNameWidget.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ACityName::ACityName()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComp"));
	SetRootComponent(WidgetComp);
}

// Called when the game starts or when spawned
void ACityName::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this]
	{
		SetDefaultText();
	}), 0.5f, false);
}

// Called every frame
void ACityName::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACityName::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACityName, CityName);
}

void ACityName::SetDefaultText()
{
	if (Index == 0)	CityName = TEXT("Food Here");
	else if (Index == 1) CityName = TEXT("City Here");

	if (HasAuthority())
	{
		OnRep_CityName();
	}
}

void ACityName::SetChecked()
{
	CityName = TEXT("OK!");

	if (HasAuthority())
	{
		OnRep_CityName();
	}
}

void ACityName::OnRep_CityName()
{
	UUserWidget* CityNameWidget = WidgetComp->GetWidget();
	if (UCityNameWidget* CNW = Cast<UCityNameWidget>(CityNameWidget))
	{
		CNW->SetCityName(CityName);

		// 텍스트 색 설정
		if (CityName == TEXT("OK!"))
		{
			CNW->SetTextColor(FLinearColor::Green);
		}
		else
		{
			CNW->SetTextColor(FLinearColor::White);
		}
	}
}

