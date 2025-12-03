// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "NPCExaminer.h"

#include "AOwlPlayer.h"
#include "Components/SphereComponent.h"
#include "Onepiece/Onepiece.h"
#include "ASpeakStageActor.h"
#include "GameLogging.h"


// Sets default values
ANPCExaminer::ANPCExaminer()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	ConstructorHelpers::FObjectFinder<UMaterialInterface> materialRef(TEXT("/Script/Engine.MaterialInstanceConstant'/Game/CustomContents/Character/Asset/Robot14/Materials/MI_Robot14_2_Inst4_NowUsing.MI_Robot14_2_Inst4_NowUsing'"));
	if (materialRef.Succeeded())
	{
		Material2 = materialRef.Object;
	}
	
	PlayerDetectSphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("PlayerDetectSphereComp"));
	PlayerDetectSphereComp->SetupAttachment(GetRootComponent());
	PlayerDetectSphereComp->SetSphereRadius(200.f);
}

// Called when the game starts or when spawned
void ANPCExaminer::BeginPlay()
{
	Super::BeginPlay();
	
	// Set Dynamic Material
	DynamicMaterial2 = UMaterialInstanceDynamic::Create(Material2, this);
	GetMesh()->SetMaterial(0, DynamicMaterial2);
	
	// Bind Overlap Event
	PlayerDetectSphereComp->OnComponentBeginOverlap.AddDynamic(this, &ANPCExaminer::OnSphereBeginOverlap);
	PlayerDetectSphereComp->OnComponentEndOverlap.AddDynamic(this, &ANPCExaminer::OnSphereEndOverlap);
}

// Called every frame
void ANPCExaminer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ANPCExaminer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ANPCExaminer::ChangeEyeColor()
{
	if (!DynamicMaterial2)
	{
		UE_LOG(LogTemp, Warning, TEXT("Fail to ChangeEyeColor because DynamicMaterial2 is Null!!!"));
		return;
	}
	
	DynamicMaterial2->SetVectorParameterValue(EyeColorName, EyeColor);
}

void ANPCExaminer::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	bIsPlayerNear = true;
	DetectedPlayer = Cast<AOwlPlayer>(OtherActor);
}

void ANPCExaminer::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	bIsPlayerNear = false;
	DetectedPlayer = nullptr;
}

//----------------------------------------------------------//
// Speak Stage System
//----------------------------------------------------------//

void ANPCExaminer::SetSpeakStage(ASpeakStageActor* InSpeakStage)
{
	SpeakStage = InSpeakStage;
	PRINTLOG(TEXT("[NPCExaminer] SpeakStage Connected: %s"), SpeakStage ? TEXT("Success") : TEXT("Failed"));
}

FString ANPCExaminer::GetCurrentQuestion() const
{
	if (SpeakStage)
	{
		return SpeakStage->GetCurrentQuestion();
	}

	return TEXT("");
}

