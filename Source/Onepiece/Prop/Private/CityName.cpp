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

	OnRep_FoodCourtInfo();
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

void ACityName::OnRep_FoodCourtInfo()
{
	UUserWidget* CityNameWidget = WidgetComp->GetWidget();
	if (UCityNameWidget* CNW = Cast<UCityNameWidget>(CityNameWidget))
	{
		CNW->SetCityName(CityName);
	}
}

