// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "luggage.h"
#include "InteractableComponent.h"
#include "ALingoGameMode.h"
#include "ALingoPlayerState.h"
#include "APlayerActor.h"
#include "ULuggageInfoWidget.h"
#include "GameLogging.h"
#include "UGameDataManager.h"
#include "UHookComponent.h"
#include "UKLingoNetworkSystem.h"
#include "ULingoGameHelper.h"

#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

#define LUGGAGE_INTERACT_WIDGET_PATH TEXT("/Game/CustomContents/UI/Widgets/WBP_InteractWidget_Luggage.WBP_InteractWidget_Luggage_C")

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
	InteractableComp->InteractionPrompt = TEXT("Pick Up");

	HookComp = CreateDefaultSubobject<UHookComponent>(TEXT("Hook"));
	
	// Initial settings
	Mesh1Comp->SetSimulatePhysics(true);
	Mesh1Comp->SetEnableGravity(true);
	Mesh1Comp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh1Comp->SetCollisionProfileName(TEXT("PhysicsActor"));

	// 무게 설정
	Mesh1Comp->SetMassOverrideInKg(NAME_None, 5.f, true);

	// 물리 복제 설정
	Mesh1Comp->SetIsReplicated(true);

	// Replication
	bReplicates = true;
	SetNetUpdateFrequency(100.0f);		// 높은 업데이트 빈도로 부드러운 네트워크 동기화
	SetMinNetUpdateFrequency(33.0f);	// 최소 30fps 업데이트 보장

	WidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComp"));
	ConstructorHelpers::FClassFinder<ULuggageInfoWidget> boxWidgetRef(LUGGAGE_INTERACT_WIDGET_PATH);
	if (boxWidgetRef.Succeeded())
	{
		WidgetComp->SetWidgetClass(boxWidgetRef.Class);
		WidgetComp->SetupAttachment(GetRootComponent());
		WidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
		WidgetComp->SetDrawSize(FVector2D(2048.0f, 1024.0f));
	}
}

void Aluggage::BeginPlay()
{
	Super::BeginPlay();

	SetReplicateMovement(true);

	// 델리게이트 바인딩
	InteractableComp->InitWidget(WidgetComp);
}

void Aluggage::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 위젯이 아직 초기화되지 않았고, ColorIdx와 PatternIdx가 유효하다면
	if (!bWidgetInitialized && ColorIdx >= 0 && PatternIdx >= 0)
	{
		if (auto InfoWidget = Cast<ULuggageInfoWidget>(WidgetComp->GetWidget()))
		{
			InfoWidget->InitLuggage(Pattern, Color);
			bWidgetInitialized = true; // 한 번만 실행
		}
	}

	// BillboardInfoWidget();

	// Pattern 이름을 luggage 위에 표시

	// if (!Pattern.IsEmpty())
	// {
	// 	FVector TextLocation = GetActorLocation() + FVector(0, 0, 100);
	// 	DrawDebugString(GetWorld(), TextLocation, Pattern, nullptr, FColor::White, 0.f, true);
	// }
}

void Aluggage::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(Aluggage, ColorIdx);
	DOREPLIFETIME(Aluggage, PatternIdx);

	/**
	 * [문제] 기존에는 여러 플레이어가 동시에 같은 Luggage를 훅할 수 있었음
	 * [해결] bIsBeingHooked와 HookedBy 복제로 중복 훅 방지
	 */
	DOREPLIFETIME(Aluggage, bIsBeingHooked);
	DOREPLIFETIME(Aluggage, HookedBy);
	DOREPLIFETIME(Aluggage, bCollisionEnabled);
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

/**
 * @brief bIsBeingHooked 복제 시 호출되는 콜백
 * @details [문제] 기존에는 훅 상태 변화 시 비주얼 피드백이 없었음
 *          [해결] 훅 중일 때 Outline 표시로 시각적 피드백 제공
 */
void Aluggage::OnRep_IsBeingHooked()
{
	if (bIsBeingHooked)
	{
		// 훅 중 - Outline 켜기
		OutlineOn();
		PRINTLOG(TEXT("OnRep_IsBeingHooked: %s is being hooked by %s"),
			*GetName(), HookedBy ? *HookedBy->GetName() : TEXT("Unknown"));
	}
	else
	{
		// 훅 해제 - Outline 상태는 InteractableComponent나 다른 시스템이 관리
		// 여기서는 명시적으로 끄지 않음 (픽업 상태 등을 고려)
		PRINTLOG(TEXT("OnRep_IsBeingHooked: %s hook released"), *GetName());
	}
}

void Aluggage::OnRep_CollisionEnabled()
{
	ApplyCollisionState(bCollisionEnabled);
}

void Aluggage::ApplyColorToMesh(int32 InColorIdx)
{
	ColorIdx = InColorIdx;

	FColorData ColorData;
	if (UGameDataManager::Get(GetWorld())->GetColorData(InColorIdx, ColorData))
	{
		// Color 변수 설정 (모든 클라이언트에서 데이터 테이블에서 가져옴)
		Color = ColorData.Desc;

		// if ( auto InfoWidget = Cast<ULuggageInfoWidget>(WidgetComp->GetWidget()))
		// 	InfoWidget->UpdateType1Data(ColorData.Desc);

		UMaterialInterface* OriginalMaterial = Mesh3Comp->GetMaterial(0);
		if (OriginalMaterial)
		{
			UMaterialInstanceDynamic* NewMaterial = UMaterialInstanceDynamic::Create(OriginalMaterial, this);
			if (NewMaterial && Mesh3Comp)
			{
				// BaseColorFactor로 변경
				NewMaterial->SetVectorParameterValue(FName("BaseColorFactor"), ColorData.GetLinearColor());
				Mesh3Comp->SetMaterial(0, NewMaterial);
			}
		}
	}
}

void Aluggage::ApplyPatternToMesh(int32 InPatternIdx)
{
	PatternIdx = InPatternIdx;

	// 데칼 바꾸기

	FReadData ReadData;
	UGameDataManager::Get(GetWorld())->GetReadData(InPatternIdx, ReadData);

	// Pattern 변수 설정 (모든 클라이언트에서 데이터 테이블에서 가져옴)
	Pattern = ReadData.Word;

	// if ( auto InfoWidget = Cast<ULuggageInfoWidget>(WidgetComp->GetWidget()))
	// 	InfoWidget->UpdateType2Data(ReadData.Word);

	UTexture2D* LoadedTexture = nullptr;
	if (ReadData.Texture.IsValid())
		LoadedTexture = ReadData.Texture.Get();
	else
		LoadedTexture = ReadData.Texture.LoadSynchronous();

	if ( UMaterialInterface* OriginalMaterial = Mesh2Comp->GetMaterial(0) )
	{
		UMaterialInstanceDynamic* NewMaterial = UMaterialInstanceDynamic::Create(OriginalMaterial, this);
		if (NewMaterial)
		{
			// BaseColorFactor로 변경
			NewMaterial->SetTextureParameterValue(FName("TextureParam"), LoadedTexture);
			Mesh2Comp->SetMaterial(0, NewMaterial);
		}
	}
}


void Aluggage::UpdateWidget()
{
	// Tick에서 호출되는 지연 초기화 로직으로 대체됨
	// 필요시 수동으로 위젯을 업데이트할 때만 사용
	if (auto InfoWidget = Cast<ULuggageInfoWidget>(WidgetComp->GetWidget()))
	{
		InfoWidget->InitLuggage(Pattern, Color);
	}
}

void Aluggage::OutlineOn()
{
	Mesh3Comp->SetRenderCustomDepth(true);
}

void Aluggage::OutlineOff()
{
	Mesh3Comp->SetRenderCustomDepth(false);
}

void Aluggage::ApplyCollisionState(bool bEnable)
{
	if (bEnable)
	{
		// 충돌 활성화
		if (BoxComp)
		{
			BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}

		if (Mesh1Comp)
		{
			Mesh1Comp->SetSimulatePhysics(true);
			Mesh1Comp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		}

		if (Mesh2Comp)
		{
			Mesh2Comp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}

		if (Mesh3Comp)
		{
			Mesh3Comp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}

		// InteractableComp의 DetectionRange 활성화
		if (InteractableComp && InteractableComp->DetectionRange)
		{
			InteractableComp->DetectionRange->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			InteractableComp->DetectionRange->SetGenerateOverlapEvents(true);
		}
	}
	else
	{
		// 충돌 비활성화
		if (BoxComp)
		{
			BoxComp->SetSimulatePhysics(false);
			BoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}

		if (Mesh1Comp)
		{
			Mesh1Comp->SetSimulatePhysics(false);
			Mesh1Comp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}

		if (Mesh2Comp)
		{
			Mesh2Comp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}

		if (Mesh3Comp)
		{
			Mesh3Comp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}

		// InteractableComp의 DetectionRange 비활성화 (위젯 노출 방지)
		if (InteractableComp && InteractableComp->DetectionRange)
		{
			InteractableComp->DetectionRange->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			InteractableComp->DetectionRange->SetGenerateOverlapEvents(false);
		}

		PRINTLOG(TEXT("Aluggage::SetAllCollision - Collisions disabled for %s"), *GetName());
	}
}

void Aluggage::SetAllCollision(bool bEnable)
{
	ApplyCollisionState(bEnable);

	if (HasAuthority())
	{
		bCollisionEnabled = bEnable;
	}
}

// void Aluggage::InfoWidgetOn()
// {
// 	WidgetComp->GetWidget()->SetVisibility(ESlateVisibility::Visible);
// }
//
// void Aluggage::InfoWidgetOff()
// {
// 	WidgetComp->GetWidget()->SetVisibility(ESlateVisibility::Hidden);
// }
//
// void Aluggage::BillboardInfoWidget()
// {
// 	// 카메라 가져오기
// 	AActor* cam = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
//
// 	if ( cam == nullptr)
// 		return;
// 	
// 	// 카메라 backward 벡터, up 벡터를 이용하여 Rotator 계산
// 	FRotator rot = UKismetMathLibrary::MakeRotFromXZ(-cam->GetActorForwardVector(), cam->GetActorUpVector());
// 	rot.Pitch = 0;
// 	// 회전값으로 설정
// 	WidgetComp->SetWorldRotation(rot);
// }


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
		GameMode->HandleLuggageSelection(Player, this);
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



void Aluggage::PlayTTSAudio()
{
	RequestListenAudio( FString::Printf(TEXT("%s %s"), *Color, *Pattern) );
}

void Aluggage::RequestListenAudio(const FString& AudioText)
{
	if (bIsRequest)
		return;

	if (auto KLingoNetwork = UKLingoNetworkSystem::Get(GetWorld()))
	{
		bIsRequest = true;

		KLingoNetwork->RequestListenAudio(
			AudioText,
			FResponseListenAudioDelegate::CreateUObject(this, &Aluggage::OnResponseListenAudio)
		);
	}
}

void Aluggage::OnResponseListenAudio(FResponseListenAudio& ResponseData, bool bWasSuccessful)
{
	bIsRequest = false;

	if (bWasSuccessful)
	{
		if (auto PlayerActor = ULingoGameHelper::GetPlayerActor(this))
			PlayerActor->PlayTTSAudio(ResponseData.audio_base64);
	}
}
