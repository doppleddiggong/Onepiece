// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "MiniOwlBot.h"

#include "APlayerActor.h"
#include "GameLogging.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
AMiniOwlBot::AMiniOwlBot()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMiniOwlBot::BeginPlay()
{
	Super::BeginPlay();
	
	
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerActor::StaticClass(), targets);
	PRINTLOG(TEXT("targets : %d"), targets.Num());
	
	if (!targets.IsEmpty())
	{
		currTarget = Cast<APlayerActor>(targets[0]);
	}
}

// Called every frame
void AMiniOwlBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	time += DeltaTime;
	baseHeight = currTarget->GetActorLocation().Z;
	
	UpdateRotation();
	UpdateLocation();
}

void AMiniOwlBot::UpdateRotation()
{
	// forward vector가 카메라 중심을 향하도록 회전
	FVector targetDir = currTarget->GetCameraPosition() - GetActorLocation();
	
	FRotator rot = FRotationMatrix::MakeFromX(targetDir).Rotator();
	SetActorRotation(rot);
}

void AMiniOwlBot::UpdateLocation()// float DeltaTime
{
	// up down
	float valZ = amplitude * FMath::Sin(time * frequency * 2 * PI) + targetHeight + baseHeight;
	float valX = currTarget->GetActorLocation().X;
	float valY = currTarget->GetActorLocation().Y;
	
	// 카메라 범위 밖으로 벗어나면
	// if (CheckAngleOutofCamera())
	{
		// 특정 위치로 오게 하기
		FVector rotatedDir = radius * currTarget->GetActorForwardVector().RotateAngleAxis(angle, FVector::UpVector);
		valX += rotatedDir.X;
		valY += rotatedDir.Y;
	}
	
	SetActorLocation(FVector(valX, valY, valZ));
}

// 이 함수 현재 이상함
bool AMiniOwlBot::CheckAngleOutofCamera()
{
	// target의 카메라 방향 벡터
	FVector2D targetCameraDir(currTarget->GetCameraForwardVector().X, currTarget->GetCameraForwardVector().Y);
	targetCameraDir.Normalize();
	
	// 자신의 방향벡터
	FVector2D botDir(GetActorForwardVector().X, GetActorForwardVector().Y);
	botDir.Normalize();
	
	// 각도 구하기
	float dotP = FVector2D::DotProduct(targetCameraDir, botDir);
	PRINTLOG(TEXT("dotP angle : %f, %f"), FMath::RadiansToDegrees(FMath::Acos(dotP)), angle);
	return FMath::RadiansToDegrees(FMath::Acos(dotP)) <= angle;
}
