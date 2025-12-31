// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "MiniOwlBot.h"

#include "APlayerActor.h"
#include "GameLogging.h"
#include "TutorSpeechWidget.h"
#include "Components/WidgetComponent.h"


// Sets default values


AMiniOwlBot::AMiniOwlBot()
{
	rootSceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("rootSceneComp"));
	SetRootComponent(rootSceneComp);
	
	speechWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("speechWidget"));
	speechWidget->SetupAttachment(rootSceneComp);
	speechWidget->SetRelativeLocation(FVector(0.f, 0.f, 40.f));
	speechWidget->SetWidgetSpace(EWidgetSpace::Screen);
	speechWidget->SetDrawAtDesiredSize(true);
	speechWidget->SetVisibility(false);
	ConstructorHelpers::FClassFinder<UTutorSpeechWidget> speechWidgetClassRef(TEXT("/Game/CustomContents/UI/Widgets/OwlTutorBot/WBP_TutorSpeechWidget.WBP_TutorSpeechWidget_C"));
	if (speechWidgetClassRef.Succeeded())
	{
		speechWidget->SetWidgetClass(speechWidgetClassRef.Class);
	}
	
	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("meshComp"));
	meshComp->SetupAttachment(rootSceneComp);
	ConstructorHelpers::FObjectFinder<UStaticMesh> meshRef(TEXT("/Script/Engine.StaticMesh'/Game/CustomContents/Character/Asset/MiniOwl/MiniOwlbot.MiniOwlbot'"));
	if (meshRef.Succeeded())
	{
		meshComp->SetStaticMesh(meshRef.Object);
	}
}

// Called when the game starts or when spawned
void AMiniOwlBot::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMiniOwlBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
void AMiniOwlBot::UpdateLocation(float DeltaTime)
{
	time += DeltaTime;
	
	// up down
	FVector targetPos = FVector::ZeroVector;
	targetPos.Z = amplitude * FMath::Sin(time * frequency * 2 * PI);
	
	SetActorRelativeLocation(targetPos);
}

void AMiniOwlBot::UpdateText(const FString& text)
{
	ServerRPC_UpdateText(text);
}

void AMiniOwlBot::ServerRPC_UpdateText_Implementation(const FString& text)
{
	MulticastRPC_UpdateText(text);
}

void AMiniOwlBot::MulticastRPC_UpdateText_Implementation(const FString& text)
{
	speechWidget->SetVisibility(true);
	Cast<UTutorSpeechWidget>(speechWidget->GetWidget())->SetInputText(text);
	
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
	{
		speechWidget->SetVisibility(false);
	}, 5.f, false);
}
