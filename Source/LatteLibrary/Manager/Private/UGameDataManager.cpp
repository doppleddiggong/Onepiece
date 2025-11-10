// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file UGameDataManager.cpp
 * @brief UGameDataManager의 동작을 구현합니다.
 */
#include "UGameDataManager.h"
#include "GameLogging.h"
#include "FCharacterAssetData.h"
#include "FComponentHelper.h"

#define HITSTOP_PATH    TEXT("/Game/CustomContents/MasterData/DT_HitStop.DT_HitStop")
#define KNOCKBACK_PATH  TEXT("/Game/CustomContents/MasterData/DT_Knockback.DT_Knockback")
#define CHARACTERINFO_PATH  TEXT("/Game/CustomContents/MasterData/DT_CharacterInfo.DT_CharacterInfo")
#define CHARACTERASSET_PATH  TEXT("/Game/CustomContents/MasterData/DT_CharacterAsset.DT_CharacterAsset")

UGameDataManager::UGameDataManager()
{
    // HitStopTable = FComponentHelper::LoadAsset<UDataTable>(HITSTOP_PATH);
    // KnockbackTable  = FComponentHelper::LoadAsset<UDataTable>(KNOCKBACK_PATH);
    // CharacterInfoTable  = FComponentHelper::LoadAsset<UDataTable>(CHARACTERINFO_PATH);
    // CharacterAssetTable = FComponentHelper::LoadAsset<UDataTable>(CHARACTERASSET_PATH);
}

void UGameDataManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    ReloadMasterData();
}

void UGameDataManager::Deinitialize()
{
    Clear_HitStopTable();
    Clear_KnockbackTable();
    Clear_CharacterInfoData();
    Clear_CharacterAssetData();
    
    Super::Deinitialize();
}

void UGameDataManager::ReloadMasterData()
{
    // LoadData_HitStopTable();
    // LoadData_KnockbackTable();
    // LoadData_CharacterInfoData();
    // LoadData_CharacterAssetData();
}

#pragma region HIT_STOP
void UGameDataManager::Clear_HitStopTable()
{
    HitStopCache.Reset();
    bLoadHitStop = false;   
}

void UGameDataManager::LoadData_HitStopTable()
{
    HitStopCache.Reset();
    bLoadHitStop = false;

    UDataTable* TableObj = HitStopTable.LoadSynchronous();
    if (!TableObj)
    {
        PRINTLOG(TEXT("Load failed: %s"), *HitStopTable.ToString());
        return;
    }

    static const FString ContextString(TEXT("HitStopTable"));
    for (const FName& RowName : TableObj->GetRowNames() )
    {
        if (const FHitStopData* Row = TableObj->FindRow<FHitStopData>(RowName, ContextString, true))
        {
            HitStopCache.Add(Row->Type, *Row);
        }
    }

    bLoadHitStop = true;
}

bool UGameDataManager::GetHitStopData(EDamageType Type, FHitStopData& Out) const
{
    if (!bLoadHitStop)
        return false;

    if (const FHitStopData* Found = HitStopCache.Find(Type))
    {
        Out = *Found;
        return true;
    }

    PRINTLOG(TEXT("DataGetFail : %s"), *UEnum::GetValueAsString(Type) );
    return false;
}

float UGameDataManager::GetHitStopDelayTime(EDamageType Type) const
{
    if (!bLoadHitStop)
        return 0.0f;

    if (const FHitStopData* Found = HitStopCache.Find(Type))
        return Found->Duration;
    
    return 0.0f;
}

#pragma endregion HIT_STOP

#pragma region KNOCKBACK
void UGameDataManager::Clear_KnockbackTable()
{
    KnockbackCache.Reset();
    bLoadKnockback = false;   
}

void UGameDataManager::LoadData_KnockbackTable()
{
    KnockbackCache.Reset();
    bLoadKnockback = false;

    UDataTable* TableObj = KnockbackTable.LoadSynchronous();
    if (!TableObj)
    {
        PRINTLOG(TEXT("Load failed: %s"), *KnockbackTable.ToString());
        return;
    }

    static const FString ContextString(TEXT("KnockbackTable"));
    for (const FName& RowName : TableObj->GetRowNames() )
    {
        if (const FKnockbackData* Row = TableObj->FindRow<FKnockbackData>(RowName, ContextString, true))
        {
            KnockbackCache.Add(Row->Type, *Row);
        }
    }

    bLoadKnockback = true;
}

bool UGameDataManager::GetKnockbackData(EDamageType Type, FKnockbackData& Out) const
{
    if (!bLoadKnockback)
        return false;

    if (const FKnockbackData* Found = KnockbackCache.Find(Type))
    {
        Out = *Found;
        return true;
    }

    PRINTLOG(TEXT("DataGetFail : %s"), *UEnum::GetValueAsString(Type) );
    return false;
}
#pragma endregion KNOCKBACK

#pragma region CHARACTER_INFO_DATA
void UGameDataManager::Clear_CharacterInfoData()
{
    CharacterInfoCache.Reset();
    bLoadCharacterInfo = false;   
}

void UGameDataManager::LoadData_CharacterInfoData()
{
    CharacterInfoCache.Reset();
    bLoadCharacterInfo = false;

    UDataTable* TableObj = CharacterInfoTable.LoadSynchronous();
    if (!TableObj)
    {
        PRINTLOG(TEXT("Load failed: %s"), *CharacterInfoTable.ToString());
        return;
    }

    static const FString ContextString(TEXT("CharacterInfoTable"));
    for (const FName& RowName : TableObj->GetRowNames() )
    {
        if (const FCharacterInfoData* Row = TableObj->FindRow<FCharacterInfoData>(RowName, ContextString, true))
        {
            CharacterInfoCache.Add(Row->Type, *Row);
        }
    }

    bLoadCharacterInfo = true;
}

bool UGameDataManager::GetCharacterInfoData(ECharacterType Type, FCharacterInfoData& Out) const
{
    if (!bLoadCharacterInfo)
        return false;

    if (const FCharacterInfoData* Found = CharacterInfoCache.Find(Type))
    {
        Out = *Found;
        return true;
    }

    PRINTLOG(TEXT("DataGetFail : %s"), *UEnum::GetValueAsString(Type) );
    return false;
}
#pragma endregion CHARACTER_INFO_DATA

#pragma region CHARACTER_ASSET_DATA
void UGameDataManager::Clear_CharacterAssetData()
{
    CharacterAssetCache.Reset();
    bLoadCharacterAsset = false;   
}

void UGameDataManager::LoadData_CharacterAssetData()
{
    CharacterAssetCache.Reset();
    bLoadCharacterAsset = false;

    UDataTable* TableObj = CharacterAssetTable.LoadSynchronous();
    if (!TableObj)
    {
        PRINTLOG(TEXT("Load failed: %s"), *CharacterAssetTable.ToString());
        return;
    }

    static const FString ContextString(TEXT("CharacterAssetTable"));
    for (const FName& RowName : TableObj->GetRowNames() )
    {
        if (const FCharacterAssetData* Row = TableObj->FindRow<FCharacterAssetData>(RowName, ContextString, true))
        {
            CharacterAssetCache.Add(Row->CharacterType, *Row);
        }
    }

    bLoadCharacterAsset = true;
}

bool UGameDataManager::GetCharacterAssetData(ECharacterType Type, FCharacterAssetData& Out) const
{
    if (!bLoadCharacterAsset)
        return false;

    if (const FCharacterAssetData* Found = CharacterAssetCache.Find(Type))
    {
        Out = *Found;
        return true;
    }

    PRINTLOG(TEXT("DataGetFail : %s"), *UEnum::GetValueAsString(Type) );
    return false;
}
#pragma endregion CHARACTER_ASSET_DATA
