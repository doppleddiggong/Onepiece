// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "NPCExaminer.h"


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
}

// Called when the game starts or when spawned
void ANPCExaminer::BeginPlay()
{
	Super::BeginPlay();
	
	DynamicMaterial2 = UMaterialInstanceDynamic::Create(Material2, this);
	GetMesh()->SetMaterial(0, DynamicMaterial2);
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

