// Fill out your copyright notice in the Description page of Project Settings.


#include "ULingoGameInstance.h"

#include "GameLogging.h"

ULingoGameInstance::ULingoGameInstance()
{

}

void ULingoGameInstance::Init()
{
	Super::Init();

	PRINTLOG( TEXT("[FireGameInstance] Initialized. This instance will persist across map transitions."));
}

void ULingoGameInstance::SetPlayerRole(APlayerController* PlayerController, EPlayerRole Role)
{
	if (!PlayerController)
	{
		PRINTLOG(TEXT("[FireGameInstance] SetPlayerRole: Invalid PlayerController"));
		return;
	}

	// PlayerState의 PlayerName을 Key로 사용 (맵 전환 시에도 유지됨)
	APlayerState* PS = PlayerController->GetPlayerState<APlayerState>();
	if (!PS)
	{
		PRINTLOG( TEXT("[FireGameInstance] SetPlayerRole: PlayerState not found!"));
		return;
	}

	FString PlayerKey = PS->GetPlayerName();

	// PlayerName이 비어있으면 PlayerId 사용 (Fallback)
	if (PlayerKey.IsEmpty())
	{
		PlayerKey = FString::Printf(TEXT("Player_%d"), PS->GetPlayerId());
		PRINTLOG( TEXT("[FireGameInstance] PlayerName empty, using PlayerId: %s"), *PlayerKey);
	}

	PlayerRoleMap.Add(PlayerKey, Role);

	PRINTLOG( TEXT("[FireGameInstance] Player %s (Key: %s) selected role: %s"),
		*PlayerController->GetName(),
		*PlayerKey,
		*UEnum::GetValueAsString(Role));
}

EPlayerRole ULingoGameInstance::GetPlayerRole(APlayerController* PlayerController) const
{
	if (!PlayerController)
	{
		PRINTLOG( TEXT("[FireGameInstance] GetPlayerRole: Invalid PlayerController"));
		return EPlayerRole::None;
	}

	// PlayerState의 PlayerName을 Key로 사용 (맵 전환 시에도 유지됨)
	APlayerState* PS = PlayerController->GetPlayerState<APlayerState>();
	if (!PS)
	{
		UE_LOG(LogTemp, Error, TEXT("[FireGameInstance] GetPlayerRole: PlayerState not found!"));
		return EPlayerRole::None;
	}

	FString PlayerKey = PS->GetPlayerName();

	// PlayerName이 비어있으면 PlayerId 사용 (Fallback)
	if (PlayerKey.IsEmpty())
	{
		PlayerKey = FString::Printf(TEXT("Player_%d"), PS->GetPlayerId());
		PRINTLOG( TEXT("[FireGameInstance] PlayerName empty, using PlayerId: %s"), *PlayerKey);
	}

	// TMap에서 Role 조회
	const EPlayerRole* FoundRole = PlayerRoleMap.Find(PlayerKey);

	if (FoundRole)
	{
		PRINTLOG( TEXT("[FireGameInstance] Found role for %s (Key: %s): %s"),
			*PlayerController->GetName(),
			*PlayerKey,
			*UEnum::GetValueAsString(*FoundRole));
		return *FoundRole;
	}
	else
	{
		PRINTLOG( TEXT("[FireGameInstance] No role found for %s (Key: %s), returning None"),
			*PlayerController->GetName(),
			*PlayerKey);
		return EPlayerRole::None;
	}
}

void ULingoGameInstance::DebugPrintPlayerRoles() const
{
	PRINTLOG( TEXT("=== [FireGameInstance] Current Player Roles ==="));

	if (PlayerRoleMap.Num() == 0)
	{
		PRINTLOG( TEXT("  (No players registered)"));
		return;
	}

	for (const auto& Entry : PlayerRoleMap)
	{
		PRINTLOG( TEXT("  Key: %s -> Role: %s"),
			*Entry.Key,
			*UEnum::GetValueAsString(Entry.Value));
	}

	PRINTLOG( TEXT("==========================================="));
}
