// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "luggage.h"
#include "InteractableComponent.h"
#include "ALingoGameMode.h"
#include "ALingoPlayerState.h"
#include "BoxInfoWidget.h"
#include "GameLogging.h"
#include "UGameDataManager.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

Aluggage::Aluggage()
{
	PrimaryActorTick.bCanEverTick = true;


	Mesh1Comp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh1Comp"));
	SetRootComponent(Mesh1Comp);
	
	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	BoxComp->SetupAttachment(GetRootComponent());
	BoxComp->SetBoxExtent(FVector(55));
	
	Mesh2Comp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh2Comp"));
	Mesh2Comp->SetupAttachment(Mesh1Comp);
	
	Mesh3Comp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh3Comp"));
	Mesh3Comp->SetupAttachment(Mesh1Comp);

	InteractableComp = CreateDefaultSubobject<UInteractableComponent>(TEXT("Interactable"));
	InteractableComp->InteractionType = EInteractionType::PickUp;
	InteractableComp->InteractionPrompt = TEXT("Press E to Grap");
	
	// Initial settings
	Mesh1Comp->SetSimulatePhysics(true);
	Mesh1Comp->SetEnableGravity(true);
	Mesh1Comp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh1Comp->SetCollisionProfileName(TEXT("PhysicsActor"));

	// 무게 설정
	Mesh1Comp->SetMassOverrideInKg(NAME_None, 5.f, true);

	// Replication
	bReplicates = true;

	BoxInfoWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("BoxInfoWidgetComp"));
	ConstructorHelpers::FClassFinder<UBoxInfoWidget> boxWidgetRef(TEXT("/Game/CustomContents/UI/Widgets/WBP_BoxInfoWidget.WBP_BoxInfoWidget_C"));
	if (boxWidgetRef.Succeeded())
	{
		BoxInfoWidgetComp->SetWidgetClass(boxWidgetRef.Class);
		BoxInfoWidgetComp->SetupAttachment(GetRootComponent());
	}
}

void Aluggage::BeginPlay()
{
	Super::BeginPlay();

	SetReplicateMovement(true);
}

void Aluggage::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	BillboardInfoWidget();
	
	// Pattern 이름을 luggage 위에 표시
	if (!Pattern.IsEmpty())
	{
		FVector TextLocation = GetActorLocation() + FVector(0, 0, 100);
		DrawDebugString(GetWorld(), TextLocation, Pattern, nullptr, FColor::White, 0.f, true);
	}
}

void Aluggage::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(Aluggage, ColorIdx);
	DOREPLIFETIME(Aluggage, PatternIdx);
}

void Aluggage::SetLuggageInfo(int32 InIdx, FString InColor, FString InPattern)
{
	SpawnIdx = InIdx;
	Color = InColor;
	Pattern = InPattern;
}

void Aluggage::OnRep_ColorIdx()
{
	ApplyColorToMesh(ColorIdx);
}

void Aluggage::OnRep_PatternIdx()
{
	ApplyPatternToMesh(PatternIdx);
}

void Aluggage::ApplyColorToMesh(int32 InColorIdx)
{
	ColorIdx = InColorIdx;

	FColorData ColorData;
	if (UGameDataManager::Get(GetWorld())->GetColorData(InColorIdx, ColorData))
	{
		FLinearColor LinearColor = ColorData.GetLinearColor();

		UMaterialInterface* OriginalMaterial = Mesh3Comp->GetMaterial(0);
		if (OriginalMaterial)
		{
			UMaterialInstanceDynamic* NewMaterial = UMaterialInstanceDynamic::Create(OriginalMaterial, this);
			if (NewMaterial && Mesh3Comp)
			{
				// BaseColorFactor로 변경
				NewMaterial->SetVectorParameterValue(FName("BaseColorFactor"), LinearColor);
				Mesh3Comp->SetMaterial(0, NewMaterial);
			}
		}
	}
}

void Aluggage::ApplyPatternToMesh(int32 InPatternIdx)
{
	PatternIdx = InPatternIdx;

	// 데칼 바꾸기
	// ...
}

void Aluggage::OutlineOn()
{
	Mesh3Comp->SetRenderCustomDepth(true);
}

void Aluggage::OutlineOff()
{
	Mesh3Comp->SetRenderCustomDepth(false);
}

void Aluggage::InfoWidgetOn()
{
	BoxInfoWidgetComp->GetWidget()->SetVisibility(ESlateVisibility::Visible);
}

void Aluggage::InfoWidgetOff()
{
	BoxInfoWidgetComp->GetWidget()->SetVisibility(ESlateVisibility::Hidden);
}

void Aluggage::BillboardInfoWidget()
{
	// 카메라 가져오기
	AActor* cam = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	// 카메라 backward 벡터, up 벡터를 이용하여 Rotator 계산
	FRotator rot = UKismetMathLibrary::MakeRotFromXZ(-cam->GetActorForwardVector(), cam->GetActorUpVector());
	rot.Pitch = 0;
	// 회전값으로 설정
	BoxInfoWidgetComp->SetWorldRotation(rot);
}


//--------------------------------------------------------------//
// Read Quest Interaction
//--------------------------------------------------------------//

void Aluggage::OnInteract(AActor* Interactor)
{
	if (!Interactor)
	{
		PRINTLOG(TEXT("[Luggage] OnInteract - Interactor is null"));
		return;
	}

	// 플레이어 컨트롤러와 PlayerState 가져오기
	APlayerController* PC = Cast<APlayerController>(Interactor->GetInstigatorController());
	if (!PC)
	{
		PRINTLOG(TEXT("[Luggage] OnInteract - PlayerController is null"));
		return;
	}

	APlayerState* PS = PC->GetPlayerState<APlayerState>();
	if (!PS)
	{
		PRINTLOG(TEXT("[Luggage] OnInteract - PlayerState is null"));
		return;
	}

	//PRINTLOG(TEXT("[Luggage] OnInteract - Player selected luggage with Target1: %s, Target2: %s"), *Target1, *Target2);

	// 서버에 선택 알림
	ServerNotifySelection(PS);
}

void Aluggage::ServerNotifySelection_Implementation(APlayerState* Player)
{
	if (!Player)
	{
		PRINTLOG(TEXT("[Luggage] ServerNotifySelection - Player is null"));
		return;
	}

	PRINTLOG(TEXT("[Luggage] ServerNotifySelection - Processing selection for player"));

	// GameMode에 캐리어 선택 알림
	ALingoGameMode* GameMode = GetWorld()->GetAuthGameMode<ALingoGameMode>();
	if (GameMode)
	{
		GameMode->HandleCarrierSelection(Player, this);
	}
	else
	{
		PRINTLOG(TEXT("[Luggage] ServerNotifySelection - GameMode is null or not ALingoGameMode"));
	}
}

bool Aluggage::ServerNotifySelection_Validate(APlayerState* Player)
{
	// 기본적인 검증: Player가 유효한지 확인
	return Player != nullptr;
}

