// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ALingoPlayerState.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "ULingoGameInstance.generated.h"

UENUM(BlueprintType)
enum class EPlayerRole : uint8
{
	// Use the default value specified by the native class or config value.
	UseConfigDefault,
	// Always update overlap state on initialization.
	AlwaysUpdate,
	// Only update if root component has Movable mobility.
	OnlyUpdateMovable,
	// Never update overlap state on initialization.
	NeverUpdate
};
/**
 * Custom GameInstance for managing persistent player data across map transitions
 * Stores player role selections from Lobby to HouseMap
 */

// 세션 검색 완료시 호출되는 함수 등록하는 Delegate
DECLARE_DELEGATE_TwoParams(FFindComplete, int32, FString);

UCLASS()
class ONEPIECE_API ULingoGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	ULingoGameInstance();
	
// 	// GameInstance 초기화 시 호출 (맵 전환 시에도 유지됨)
 	virtual void Init() override;
//
// protected:
// 	// Player의 UniqueNetId를 Key로, 선택한 Role을 Value로 저장
// 	// ServerTravel 시 PlayerController 인스턴스가 바뀌어도 NetId는 유지됨
// 	UPROPERTY()
// 	TMap<FString, EPlayerRole> PlayerRoleMap;
//
// public:
// 	/**
// 	 * Lobby에서 플레이어가 Role을 선택했을 때 호출
// 	 * @param PlayerController 역할을 선택한 플레이어 컨트롤러
// 	 * @param Role 선택한 역할 (Firefighter 또는 Citizen)
// 	 */
// 	UFUNCTION(BlueprintCallable, Category = "Player")
// 	void SetPlayerRole(APlayerController* PlayerController, EPlayerRole Role);
//
// 	/**
// 	 * HouseMap에서 Pawn 생성 시 PlayerController의 Role을 조회
// 	 * @param PlayerController 역할을 조회할 플레이어 컨트롤러
// 	 * @return 저장된 역할. 없으면 EPlayerRole::None 반환
// 	 */
// 	UFUNCTION(BlueprintCallable, Category = "Player")
// 	EPlayerRole GetPlayerRole(APlayerController* PlayerController) const;
//
// 	/**
// 	 * 디버깅용: 현재 저장된 모든 Role 출력
// 	 */
// 	UFUNCTION(BlueprintCallable, Category = "Player")
// 	void DebugPrintPlayerRoles() const;

public:
	/*===================================
	 * 세션 생성 & 조회
	 ===================================*/
	// 세션의 모든 처리를 진행 하는 객체
	IOnlineSessionPtr sessionInterface;

	// 세션 생성 관련
	// 현재 세션 이름
	FName currSessionName;
	// 세션 생성 함수
	UFUNCTION(BlueprintCallable)
	void CreateMySession(FString displayName);
	// 세션 생성 완료 함수
	void OnCreateSessionComplete(FName sessionName, bool bWasSuccessful);

	// 세션 조회 관련
	// 세션 조회할 때 사용하는 객체
	TSharedPtr<FOnlineSessionSearch> sessionSearch;
	// 세션 조회 완료시 세션 갯수 만큼 호출하는 Delegate
	FFindComplete onFindComplete;
	// 세션 조회 함수
	UFUNCTION(BlueprintCallable)
	void FindOtherSession();
	// 세션 조회 완료 함수
	void OnFindSessionComplete(bool bWasSuccessful);

	// 세션 참여 관련
	// 세션 참여 함수
	UFUNCTION(BlueprintCallable)
	void JoinOtherSession(int32 sessionIdx);
	// 세션 참여 완료 함수
	void OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result);

	// 문자열을 UTF-8 --> base64 로 Encode 하는 함수
	FString StringBase64Encode(FString str);	
	// 문자열을 base64 --> UTF-8 로 Decode 하는 함수
	FString StringBase64Decode(FString str);

	// 어떤 캐릭터 선택했는지
	UPROPERTY(Transient)
	TMap<FString, int32> selectCharacter;

	void SetSelectCharacter(FString userName, int32 characterIdx);
	int32 GetSelectCharacter(FString userName);
};
