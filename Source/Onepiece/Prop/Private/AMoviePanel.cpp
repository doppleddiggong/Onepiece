// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "AMoviePanel.h"

#include "FComponentHelper.h"
#include "Components/StaticMeshComponent.h"
#include "MediaPlayer.h"
#include "MediaSource.h"
#include "GameLogging.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

#define SCREEN_PATH						TEXT("/Engine/BasicShapes/Cube.Cube")
#define MEDIA_PLAYER_PATH				TEXT("/Game/CustomContents/Media/MP_InGame.MP_InGame")
#define MEMIA_SOURCE_PATH				TEXT("/Game/CustomContents/Media/Source/Duolingo.Duolingo")
#define MEDIA_PLAYER_TEXTURE_PATH		TEXT("/Game/CustomContents/Media/MPT_InGame.MPT_InGame")

AMoviePanel::AMoviePanel()
{
	PrimaryActorTick.bCanEverTick = false;

	Screen = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Screen"));
	RootComponent = Screen;

	Screen->SetStaticMesh(FComponentHelper::LoadAsset<UStaticMesh>(SCREEN_PATH));
	Screen->SetRelativeScale3D(FVector(16.0f, 9.0f, 0.1f));

	MediaPlayer = FComponentHelper::LoadAsset<UMediaPlayer>(MEDIA_PLAYER_PATH);
	MediaSource = FComponentHelper::LoadAsset<UMediaSource>(MEMIA_SOURCE_PATH);
	MediaTexture = FComponentHelper::LoadAsset<UTexture>(MEDIA_PLAYER_TEXTURE_PATH);
}

void AMoviePanel::BeginPlay()
{
	Super::BeginPlay();

	if (!Screen || !Screen->GetMaterial(0))
		return;

	// Dynamic Material 생성
	MediaMaterial = UMaterialInstanceDynamic::Create(Screen->GetMaterial(0), this);
	if (!MediaMaterial)
		return;

	MediaMaterial->SetVectorParameterValue( ColorParam, FLinearColor( 0, 0,  0, 1.0f ) );
	MediaMaterial->SetTextureParameterValue(TextureParam, MediaTexture);
	MediaMaterial->SetScalarParameterValue(TileParam, TileCount);

	Screen->SetMaterial(0, MediaMaterial);

	MediaPlayer->OnEndReached.Clear();
	MediaPlayer->OnEndReached.AddDynamic(this, &AMoviePanel::HandleMediaEndReached);
}

void AMoviePanel::PlayMedia()
{
	if (!MediaPlayer || !MediaSource)
	{
		PRINTLOG( TEXT("AMoviePanel::PlayMedia - MediaPlayer or MediaSource is null"));
		return;
	}

	MediaMaterial->SetVectorParameterValue(ColorParam, FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));

	// 미디어 재생
	MediaPlayer->OpenSource(MediaSource);
	MediaPlayer->Play();
}

void AMoviePanel::HandleMediaEndReached()
{
	if (bOneShot)
	{
		MediaMaterial->SetVectorParameterValue(ColorParam, FLinearColor(0.0f, 0.0f, 0.0f, 1.0f));
		MediaPlayer->Pause();
	}
}