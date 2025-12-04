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
#include "FColorData.h"
#include "FColorStyleData.h"
#include "FLevelData.h"
#include "FListenData.h"
#include "FReadData.h"
#include "FWordStudyData.h"

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

#pragma region COLOR_DATA
public:
	UPROPERTY(EditAnywhere, Category="MasterData|Color")
	TSoftObjectPtr<UDataTable> ColorDataTable;

	UFUNCTION(BlueprintCallable, Category="MasterData|Color")
	bool GetColorData(int32 Index, FColorData& Out) const;

	UFUNCTION(BlueprintCallable, Category="MasterData|Color")
	TArray<FColorData> GetColorDataByLevel(int32 Level) const;

private:
	void Clear_ColorData();
	void LoadData_ColorData();
	bool bLoadColorData = false;

	UPROPERTY(VisibleAnywhere, Category = "Cache", meta = (AllowPrivateAccess = "true"))
	TMap<int32, FColorData> ColorDataCache;
#pragma endregion COLOR_DATA

#pragma region COLOR_STYLE_DATA
public:
	UPROPERTY(EditAnywhere, Category="MasterData|ColorStyle")
	TSoftObjectPtr<UDataTable> ColorStyleDataTable;

	UFUNCTION(BlueprintCallable, Category="MasterData|ColorStyle")
	bool GetColorStyleData(EColorStyleType Type, FColorStyleData& Out) const;

	UFUNCTION(BlueprintCallable, Category="MasterData|ColorStyle")
	TMap<EColorStyleType, FColorStyleData> GetAllColorStyleData() const;

private:
	void Clear_ColorStyleData();
	void LoadData_ColorStyleData();
	bool bLoadColorStyleData = false;

	UPROPERTY(VisibleAnywhere, Category = "Cache", meta = (AllowPrivateAccess = "true"))
	TMap<EColorStyleType, FColorStyleData> ColorStyleDataCache;
#pragma endregion COLOR_STYLE_DATA

#pragma region LEVEL_DATA
public:
	UPROPERTY(EditAnywhere, Category="MasterData|Level")
	TSoftObjectPtr<UDataTable> LevelDataTable;

	UFUNCTION(BlueprintCallable, Category="MasterData|Level")
	bool GetLevelData(int32 Step, int32 Level, FLevelData& Out) const;

private:
	void Clear_LevelData();
	void LoadData_LevelData();
	bool bLoadLevelData = false;

	UPROPERTY(VisibleAnywhere, Category = "Cache", meta = (AllowPrivateAccess = "true"))
	TMap<int32, FLevelData> LevelDataCache;
#pragma endregion LEVEL_DATA

#pragma region LISTEN_DATA
public:
	UPROPERTY(EditAnywhere, Category="MasterData|Listen")
	TSoftObjectPtr<UDataTable> ListenDataTable;

	UFUNCTION(BlueprintCallable, Category="MasterData|Listen")
	bool GetListenData(int32 Index, FListenData& Out) const;

	UFUNCTION(BlueprintCallable, Category="MasterData|Listen")
	TArray<FListenData> GetListenDataByStepAndLevel(int32 Step, int32 Level) const;

	UFUNCTION(BlueprintCallable, Category="MasterData|Listen")
	TArray<FListenData> GetListenDataByCategory(const FString& Category) const;

private:
	void Clear_ListenData();
	void LoadData_ListenData();
	bool bLoadListenData = false;

	UPROPERTY(VisibleAnywhere, Category = "Cache", meta = (AllowPrivateAccess = "true"))
	TMap<int32, FListenData> ListenDataCache;
#pragma endregion LISTEN_DATA

#pragma region READ_DATA
public:
	UPROPERTY(EditAnywhere, Category="MasterData|Read")
	TSoftObjectPtr<UDataTable> ReadDataTable;

	UFUNCTION(BlueprintCallable, Category="MasterData|Read")
	bool GetReadData(int32 Index, FReadData& Out) const;

	UFUNCTION(BlueprintCallable, Category="MasterData|Read")
	TArray<FReadData> GetReadDataByLevel(int32 Level) const;

private:
	void Clear_ReadData();
	void LoadData_ReadData();
	bool bLoadReadData = false;

	UPROPERTY(VisibleAnywhere, Category = "Cache", meta = (AllowPrivateAccess = "true"))
	TMap<int32, FReadData> ReadDataCache;
#pragma endregion READ_DATA

#pragma region WORD_DATA
public:
	UPROPERTY(EditAnywhere, Category="MasterData|Word")
	TSoftObjectPtr<UDataTable> WordStudyDataTable;

	UFUNCTION(BlueprintCallable, Category="MasterData|Word")
	bool GetWordStudyData(int32 Index, FWordStudyData& Out) const;

	UFUNCTION(BlueprintCallable, Category="MasterData|Word")
	TArray<FWordStudyData> GetAllWordStudyData() const;

private:
	void Clear_WordStudyData();
	void LoadData_WordStudyData();
	bool bLoadWordStudyData = false;

	UPROPERTY(VisibleAnywhere, Category = "Cache", meta = (AllowPrivateAccess = "true"))
	TMap<int32, FWordStudyData> WordStudyDataCache;
#pragma endregion WORD_DATA
};
