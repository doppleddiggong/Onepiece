// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file UGameDataManager.h
 * @brief UGameDataManager 클래스를 선언합니다.
 */
#pragma once

#include "CoreMinimal.h"
#include "Macro.h"

#include "FHitStopData.h"
#include "FKnockbackData.h"
#include "FCharacterInfoData.h"
#include "FCharacterAssetData.h"

#include "Subsystems/GameInstanceSubsystem.h"
#include "UGameDataManager.generated.h"

/**
 * @brief 데이터 테이블(.csv)에서 게임 데이터를 로드하고 캐시하여 런타임에 빠르게 접근할 수 있도록 제공하는 데이터 관리 서브시스템입니다.
 * @details 캐릭터 정보, 건물 데이터, 히트스톱, 넉백 등 다양한 마스터 데이터를 관리하며, 필요 시 데이터를 리로드하는 기능을 제공합니다.
 */
UCLASS()
class LATTELIBRARY_API UGameDataManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    DEFINE_SUBSYSTEM_GETTER_INLINE(UGameDataManager);
    UGameDataManager();

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(Exec)
	void ReloadMasterData();

#pragma region HIT_STOP
public:
	UPROPERTY(EditAnywhere, Category="MasterData|HitStop")
	TSoftObjectPtr<UDataTable> HitStopTable;

	UFUNCTION(BlueprintCallable, Category="MasterData|HitStop")
	bool GetHitStopData(EDamageType Type, UPARAM(ref) FHitStopData& Out) const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="MasterData|HitStop")
	float GetHitStopDelayTime(EDamageType Type) const;

private:
	void Clear_HitStopTable();
	void LoadData_HitStopTable();
	bool bLoadHitStop = false;

	UPROPERTY(VisibleAnywhere, Category = "Cache", meta = (AllowPrivateAccess = "true"))
	TMap<EDamageType, FHitStopData> HitStopCache;
#pragma endregion HIT_STOP

#pragma region KNOCKBACK
public:
	UPROPERTY(EditAnywhere, Category="MasterData|Knockback")
	TSoftObjectPtr<UDataTable> KnockbackTable;

	UFUNCTION(BlueprintCallable, Category="MasterData|Knockback")
	bool GetKnockbackData(EDamageType Type, FKnockbackData& Out) const;

private:
	void Clear_KnockbackTable();
	void LoadData_KnockbackTable();
	bool bLoadKnockback = false;

	UPROPERTY(VisibleAnywhere, Category = "Cache", meta = (AllowPrivateAccess = "true"))
	TMap<EDamageType, FKnockbackData> KnockbackCache;
#pragma endregion KNOCKBACK
	
#pragma region CHARACTER_INFO_DATA
public:
	UPROPERTY(EditAnywhere, Category="MasterData|CharacterInfo")
	TSoftObjectPtr<UDataTable> CharacterInfoTable;

	UFUNCTION(BlueprintCallable, Category="MasterData|CharacterInfo")
	bool GetCharacterInfoData(ECharacterType Type, FCharacterInfoData& Out) const;

private:
	void Clear_CharacterInfoData();
	void LoadData_CharacterInfoData();
	bool bLoadCharacterInfo = false;

	UPROPERTY(VisibleAnywhere, Category = "Cache", meta = (AllowPrivateAccess = "true"))
	TMap<ECharacterType, FCharacterInfoData> CharacterInfoCache;
#pragma endregion CHARACTER_INFO_DATA

#pragma region CHARACTER_ASSET_DATA
public:
	UPROPERTY(EditAnywhere, Category="MasterData|CharacterAsset")
	TSoftObjectPtr<UDataTable> CharacterAssetTable;

	UFUNCTION(BlueprintCallable, Category="MasterData|CharacterAsset")
	bool GetCharacterAssetData(ECharacterType Type, FCharacterAssetData& Out) const;

private:
	void Clear_CharacterAssetData();
	void LoadData_CharacterAssetData();
	bool bLoadCharacterAsset = false;

	UPROPERTY(VisibleAnywhere, Category = "Cache", meta = (AllowPrivateAccess = "true"))
	TMap<ECharacterType, FCharacterAssetData> CharacterAssetCache;
#pragma endregion CHARACTER_ASSET_DATA
};
