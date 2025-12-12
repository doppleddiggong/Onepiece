// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.


#include "AMoviePanel.h"

#include "FComponentHelper.h"
#include "Components/StaticMeshComponent.h"
#include "MediaPlayer.h"
#include "MediaSource.h"
#include "MediaTexture.h"
#include "GameLogging.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

#define SCREEN_PATH						TEXT("/Engine/BasicShapes/Cube.Cube")
#define MEDIA_PLAYER_PATH				TEXT("/Game/CustomContents/Media/MP_InGame.MP_InGame")
#define MEDIA_SOURCE_PATH				TEXT("/Game/CustomContents/Media/Source/Duolingo.Duolingo")
#define MEDIA_PLAYER_TEXTURE_PATH		TEXT("/Game/CustomContents/Media/MPT_InGame.MPT_InGame")

AMoviePanel::AMoviePanel()
{
	PrimaryActorTick.bCanEverTick = false;

	Screen = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Screen"));
	RootComponent = Screen;

	Screen->SetStaticMesh(FComponentHelper::LoadAsset<UStaticMesh>(SCREEN_PATH));
	Screen->SetRelativeScale3D(FVector(16.0f, 9.0f, 0.1f));
	
	// 각 인스턴스마다 고유한 MediaPlayer와 MediaTexture 생성
	MediaPlayer = CreateDefaultSubobject<UMediaPlayer>(TEXT("MediaPlayer"));
	MediaTexture = CreateDefaultSubobject<UMediaTexture>(TEXT("MediaTexture"));
	
	// MediaPlayer의 출력을 MediaTexture로 설정
	if (MediaPlayer && MediaTexture)
	{
		MediaPlayer->SetLooping(true);
		MediaPlayer->PlayOnOpen = false;
	}
	
	MediaSource = FComponentHelper::LoadAsset<UMediaSource>(MEDIA_SOURCE_PATH);
}

void AMoviePanel::BeginPlay()
{
	Super::BeginPlay();

	if (!Screen || !Screen->GetMaterial(0))
		return;

	// MediaTexture를 MediaPlayer에 연결
	if (MediaTexture && MediaPlayer)
	{
		MediaTexture->SetMediaPlayer(MediaPlayer);
		MediaTexture->UpdateResource();
	}

	MediaMaterial = UMaterialInstanceDynamic::Create(Screen->GetMaterial(0), this);
	if (!MediaMaterial)
		return;

	MediaMaterial->SetVectorParameterValue( ColorParam, FLinearColor( 0, 0,  0, 1.0f ) );
	MediaMaterial->SetTextureParameterValue(TextureParam, MediaTexture);
	MediaMaterial->SetScalarParameterValue(TileParam, TileCount);

	Screen->SetMaterial(0, MediaMaterial);

	MediaPlayer->OnEndReached.Clear();
	MediaPlayer->OnEndReached.AddDynamic(this, &AMoviePanel::HandleMediaEndReached);

	PlayMedia();
}

void AMoviePanel::PlayMedia()
{
	if (!MediaPlayer || !MediaSource)
	{
		PRINTLOG( TEXT("AMoviePanel::PlayMedia - MediaPlayer or MediaSource is null"));
		return;
	}

	MediaMaterial->SetVectorParameterValue(ColorParam, FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));

	// OnMediaOpened 이벤트 바인딩
	MediaPlayer->OnMediaOpened.Clear();
	MediaPlayer->OnMediaOpened.AddDynamic(this, &AMoviePanel::HandleMediaOpened);

	// 미디어 소스 열기
	if (!MediaPlayer->OpenSource(MediaSource))
	{
		PRINTLOG(TEXT("AMoviePanel::PlayMedia - Failed to open media source"));
	}
}

void AMoviePanel::HandleMediaOpened(FString OpenedUrl)
{
	if (MediaPlayer)
	{
		MediaPlayer->Play();
		PRINTLOG(TEXT("AMoviePanel::HandleMediaOpened - Media opened: %s"), *OpenedUrl);
	}
}

void AMoviePanel::HandleMediaEndReached()
{
	if (bOneShot)
	{
		MediaMaterial->SetVectorParameterValue(ColorParam, FLinearColor(0.0f, 0.0f, 0.0f, 1.0f));
		MediaPlayer->Pause();
	}
}