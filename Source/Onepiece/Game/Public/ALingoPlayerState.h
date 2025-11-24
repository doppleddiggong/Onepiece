// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ALingoPlayerState.generated.h"

/**
 * 
 */

// UENUM(BlueprintType)
// enum class EPlayerRole : uint8
// {
// 	None			UMETA(DisplayName = "None"),
// 	Firefighter		UMETA(DisplayName = "Firefighter"),
// 	Citizen			UMETA(DisplayName = "Citizen")
// };
//
// UENUM(BlueprintType)
// enum class EGamePhase :uint8
// {
// 	WaitingToStart		UMETA(DisplayName = "Waiting"),
// 	ChapterStart		UMETA(DisplayName = "Game Start"),
// 	MissionStart		UMETA(DisplayName = "Mission"),
// 	MissionComplete		UMETA(DisplayName = "Mission Complete"),
// 	Result				UMETA(DisplayName = "Result"),
// 	ChapterComplete		UMETA(DisplayName = "Chapter Complete"),
// };

UCLASS()
class ONEPIECE_API ALingoPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ALingoPlayerState();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	void SetToken(FString InToken);
	FString GetToken()
	{
		return AccessToken; 
	}

private:
	FString AccessToken;
};
