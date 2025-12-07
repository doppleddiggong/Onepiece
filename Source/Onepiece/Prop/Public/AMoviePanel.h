// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AMoviePanel.generated.h"

UCLASS()
class ONEPIECE_API AMoviePanel : public AActor
{
	GENERATED_BODY()

public:
	AMoviePanel();

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "TEST")
	void PlayMedia();

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void HandleMediaOpened(FString OpenedUrl);

	UFUNCTION()
	void HandleMediaEndReached();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Media")
	TObjectPtr<class UStaticMeshComponent> Screen;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Media")
	TObjectPtr<class UMediaPlayer> MediaPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Media")
	TObjectPtr<class UMediaSource> MediaSource;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Media")
	TObjectPtr<class UMaterialInstanceDynamic> MediaMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Media")
	TObjectPtr<class UMediaTexture> MediaTexture;

	
	// Parameters
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Media")
	FName ColorParam  = TEXT("Color");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Media")
	FName TextureParam  = TEXT("MediaTexture");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Media")
	FName TileParam  = TEXT("Tile");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Media")
	float TileCount = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Media")
	int32 Group = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Media")
	bool bOneShot = false;
};