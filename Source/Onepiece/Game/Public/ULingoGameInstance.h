// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ALingoPlayerState.h"
#include "ULingoGameInstance.generated.h"

/**
 * Custom GameInstance for managing persistent player data across map transitions
 * Stores player role selections from Lobby to HouseMap
 */
UCLASS()
class ONEPIECE_API ULingoGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	ULingoGameInstance();

protected:
	// Player의 UniqueNetId를 Key로, 선택한 Role을 Value로 저장
	// ServerTravel 시 PlayerController 인스턴스가 바뀌어도 NetId는 유지됨
	UPROPERTY()
	TMap<FString, EPlayerRole> PlayerRoleMap;

public:
	/**
	 * Lobby에서 플레이어가 Role을 선택했을 때 호출
	 * @param PlayerController 역할을 선택한 플레이어 컨트롤러
	 * @param Role 선택한 역할 (Firefighter 또는 Citizen)
	 */
	UFUNCTION(BlueprintCallable, Category = "Player")
	void SetPlayerRole(APlayerController* PlayerController, EPlayerRole Role);

	/**
	 * HouseMap에서 Pawn 생성 시 PlayerController의 Role을 조회
	 * @param PlayerController 역할을 조회할 플레이어 컨트롤러
	 * @return 저장된 역할. 없으면 EPlayerRole::None 반환
	 */
	UFUNCTION(BlueprintCallable, Category = "Player")
	EPlayerRole GetPlayerRole(APlayerController* PlayerController) const;

	/**
	 * 디버깅용: 현재 저장된 모든 Role 출력
	 */
	UFUNCTION(BlueprintCallable, Category = "Player")
	void DebugPrintPlayerRoles() const;

	/**
	 * GameInstance 초기화 시 호출 (맵 전환 시에도 유지됨)
	 */
	virtual void Init() override;
};
