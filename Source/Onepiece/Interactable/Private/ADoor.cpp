// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "ADoor.h"

#include "FComponentHelper.h"
#include "UBroadcastManager.h"

#define DOOR_FRAME_PATH		TEXT("/Game/CustomContents/Platfrom/Assets/Portal_Door/portal_door_Frame.portal_door_Frame")
#define DOOR_LEFT_PATH		TEXT("/Game/CustomContents/Platfrom/Assets/Portal_Door/portal_door_Right.portal_door_Right")
#define DOOR_RIGHT_PATH		TEXT("/Game/CustomContents/Platfrom/Assets/Portal_Door/portal_door_left.portal_door_left")

ADoor::ADoor()
{
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	SM_Frame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM_Frame"));
	SM_Frame->SetupAttachment(SceneRoot);
	SM_Frame->SetStaticMesh(FComponentHelper::LoadAsset<UStaticMesh>(DOOR_FRAME_PATH));
	
	SM_Right = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM_Right"));
	SM_Right->SetupAttachment(SceneRoot);
	SM_Right->SetStaticMesh(FComponentHelper::LoadAsset<UStaticMesh>(DOOR_RIGHT_PATH));
	
	SM_Left = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM_Left"));
	SM_Left->SetupAttachment(SceneRoot);
	SM_Left->SetStaticMesh(FComponentHelper::LoadAsset<UStaticMesh>(DOOR_LEFT_PATH));
}

void ADoor::BeginPlay()
{
	Super::BeginPlay();

	UBroadcastManager::Get(GetWorld())->OnDoorMessage.AddDynamic(this, &ADoor::OnDoorMessage);

	// 맵 배치 시 설정한 초기 상태 적용
	if (bStartOpened)
	{
		Alpha = 1.0f;
		UpdateDoor(Alpha);
	}
	else
	{
		Alpha = 0.0f;
		UpdateDoor(Alpha);
	}
}

void ADoor::UpdateDoor(float InAlpha)
{
	Alpha = InAlpha; // Blueprint에서 SET 한 것과 대응

	// Right
	{
		const FVector ClosedPos = FVector::ZeroVector;
		const FVector OpenPos   = RightOpen;

		const FVector NewPos = FMath::Lerp(ClosedPos, OpenPos, Alpha);
		SM_Right->SetRelativeLocation(NewPos, false, nullptr, ETeleportType::None);
	}

	// Left
	{
		const FVector ClosedPos = FVector::ZeroVector;
		const FVector OpenPos   = LeftOpen;

		const FVector NewPos = FMath::Lerp(ClosedPos, OpenPos, Alpha);
		SM_Left->SetRelativeLocation(NewPos, false, nullptr, ETeleportType::None);
	}
}

void ADoor::OnDoorMessage(int32 InDoorIndex, bool bInOpen, AActor* EventInstigator)
{
	if (InDoorIndex != DoorIndex)
		return;

	CurCount += (bInOpen ? 1 : -1);
	CurCount = FMath::Clamp(CurCount, 0, ReqCount); // 안전

	if (CurCount >= ReqCount)
		OpenDoor();
	else
		CloseDoor();
}

void ADoor::OpenDoor_Implementation()
{
}

void ADoor::CloseDoor_Implementation()
{
}