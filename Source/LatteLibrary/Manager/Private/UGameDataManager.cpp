// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file UGameDataManager.cpp
 * @brief UGameDataManager의 동작을 구현합니다.
 */
#include "UGameDataManager.h"
#include "GameLogging.h"
#include "FCharacterAssetData.h"
#include "FComponentHelper.h"

// #define HITSTOP_PATH    TEXT("/Game/CustomContents/MasterData/DT_HitStop.DT_HitStop")
// #define KNOCKBACK_PATH  TEXT("/Game/CustomContents/MasterData/DT_Knockback.DT_Knockback")
// #define CHARACTERINFO_PATH  TEXT("/Game/CustomContents/MasterData/DT_CharacterInfo.DT_CharacterInfo")
// #define CHARACTERASSET_PATH  TEXT("/Game/CustomContents/MasterData/DT_CharacterAsset.DT_CharacterAsset")
#define COLORDATA_PATH  TEXT("/Game/CustomContents/MasterData/DT_ColorData.DT_ColorData")
#define COLORSTYLEDATA_PATH  TEXT("/Game/CustomContents/MasterData/DT_ColorStyleData.DT_ColorStyleData")
#define RESOURCETEXTUREDATA_PATH  TEXT("/Game/CustomContents/MasterData/DT_ResourceTextureData.DT_ResourceTextureData")
#define LEVELDATA_PATH  TEXT("/Game/CustomContents/MasterData/DT_LevelData.DT_LevelData")
#define LISTENDATA_PATH  TEXT("/Game/CustomContents/MasterData/DT_ListenData.DT_ListenData")
#define READDATA_PATH  TEXT("/Game/CustomContents/MasterData/DT_ReadData.DT_ReadData")
#define WORDDATA_PATH  TEXT("/Game/CustomContents/MasterData/DT_WordData.DT_WordData")

UGameDataManager::UGameDataManager()
{
    ColorDataTable = FComponentHelper::LoadAsset<UDataTable>(COLORDATA_PATH);
    ColorStyleDataTable = FComponentHelper::LoadAsset<UDataTable>(COLORSTYLEDATA_PATH);
    ResourceTextureDataTable = FComponentHelper::LoadAsset<UDataTable>(RESOURCETEXTUREDATA_PATH);
    LevelDataTable  = FComponentHelper::LoadAsset<UDataTable>(LEVELDATA_PATH);
    ListenDataTable  = FComponentHelper::LoadAsset<UDataTable>(LISTENDATA_PATH);
    ReadDataTable = FComponentHelper::LoadAsset<UDataTable>(READDATA_PATH);
    WordStudyDataTable = FComponentHelper::LoadAsset<UDataTable>(WORDDATA_PATH);
}

void UGameDataManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    ReloadMasterData();
}

void UGameDataManager::Deinitialize()
{
    // Clear_HitStopTable();
    // Clear_KnockbackTable();
    // Clear_CharacterInfoData();
    // Clear_CharacterAssetData();
    Clear_ColorData();
    Clear_ColorStyleData();
    Clear_ResourceTextureData();
    Clear_LevelData();
    Clear_ListenData();
    Clear_ReadData();
    Clear_WordStudyData();

    Super::Deinitialize();
}

void UGameDataManager::ReloadMasterData()
{
    // LoadData_HitStopTable();
    // LoadData_KnockbackTable();
    // LoadData_CharacterInfoData();
    // LoadData_CharacterAssetData();
    LoadData_ColorData();
    LoadData_ColorStyleData();
    LoadData_ResourceTextureData();
    LoadData_LevelData();
    LoadData_ListenData();
    LoadData_ReadData();
    LoadData_WordStudyData();
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

#pragma region COLOR_DATA
void UGameDataManager::Clear_ColorData()
{
    ColorDataCache.Reset();
    bLoadColorData = false;
}

void UGameDataManager::LoadData_ColorData()
{
    ColorDataCache.Reset();
    bLoadColorData = false;

    UDataTable* TableObj = ColorDataTable.LoadSynchronous();
    if (!TableObj)
    {
        PRINTLOG(TEXT("Load failed: %s"), *ColorDataTable.ToString());
        return;
    }

    static const FString ContextString(TEXT("ColorDataTable"));
    for (const FName& RowName : TableObj->GetRowNames())
    {
        if (const FColorData* Row = TableObj->FindRow<FColorData>(RowName, ContextString, true))
        {
            ColorDataCache.Add(Row->Index, *Row);
        }
    }

    bLoadColorData = true;
}

bool UGameDataManager::GetColorData(int32 Index, FColorData& Out) const
{
    if (!bLoadColorData)
        return false;

    if (const FColorData* Found = ColorDataCache.Find(Index))
    {
        Out = *Found;
        return true;
    }

    PRINTLOG(TEXT("DataGetFail : ColorData Index %d"), Index);
    return false;
}

TArray<FColorData> UGameDataManager::GetColorDataByLevel(int32 Level) const
{
    TArray<FColorData> Result;
    
    if (!bLoadColorData)
        return Result;

    for (const auto& Pair : ColorDataCache)
    {
        if (Pair.Value.Level == Level)
        {
            Result.Add(Pair.Value);
        }
    }

    return Result;
}
#pragma endregion COLOR_DATA

#pragma region COLOR_STYLE_DATA
void UGameDataManager::Clear_ColorStyleData()
{
    ColorStyleDataCache.Reset();
    bLoadColorStyleData = false;
}

void UGameDataManager::LoadData_ColorStyleData()
{
    ColorStyleDataCache.Reset();
    bLoadColorStyleData = false;

    UDataTable* TableObj = ColorStyleDataTable.LoadSynchronous();
    if (!TableObj)
    {
        PRINTLOG(TEXT("Load failed: %s"), *ColorStyleDataTable.ToString());
        return;
    }

    static const FString ContextString(TEXT("ColorStyleDataTable"));
    for (const FName& RowName : TableObj->GetRowNames())
    {
        if (const FColorStyleData* Row = TableObj->FindRow<FColorStyleData>(RowName, ContextString, true))
        {
            // RowName을 Enum으로 변환
            FString EnumString = RowName.ToString();
            EColorStyleType ColorType = static_cast<EColorStyleType>(
                StaticEnum<EColorStyleType>()->GetValueByNameString(EnumString)
            );

            ColorStyleDataCache.Add(ColorType, *Row);
        }
    }

    bLoadColorStyleData = true;
}

bool UGameDataManager::GetColorStyleData(EColorStyleType Type, FColorStyleData& Out) const
{
    if (!bLoadColorStyleData)
        return false;

    if (const FColorStyleData* Found = ColorStyleDataCache.Find(Type))
    {
        Out = *Found;
        return true;
    }

    PRINTLOG(TEXT("DataGetFail : ColorStyleData Type %s"), *UEnum::GetValueAsString(Type));
    return false;
}

TMap<EColorStyleType, FColorStyleData> UGameDataManager::GetAllColorStyleData() const
{
    if (!bLoadColorStyleData)
        return TMap<EColorStyleType, FColorStyleData>();

    return ColorStyleDataCache;
}
#pragma endregion COLOR_STYLE_DATA

#pragma region RESOURCE_TEXTURE_DATA
void UGameDataManager::Clear_ResourceTextureData()
{
    ResourceTextureDataCache.Reset();
    bLoadResourceTextureData = false;
}

void UGameDataManager::LoadData_ResourceTextureData()
{
    ResourceTextureDataCache.Reset();
    bLoadResourceTextureData = false;

    UDataTable* TableObj = ResourceTextureDataTable.LoadSynchronous();
    if (!TableObj)
    {
        PRINTLOG(TEXT("Load failed: %s"), *ResourceTextureDataTable.ToString());
        return;
    }

    static const FString ContextString(TEXT("ResourceTextureDataTable"));
    for (const FName& RowName : TableObj->GetRowNames())
    {
        if (const FResourceTextureData* Row = TableObj->FindRow<FResourceTextureData>(RowName, ContextString, true))
        {
            // RowName을 Enum으로 변환
            FString EnumString = RowName.ToString();
            EResourceTextureType TextureType = static_cast<EResourceTextureType>(
                StaticEnum<EResourceTextureType>()->GetValueByNameString(EnumString)
            );

            ResourceTextureDataCache.Add(TextureType, *Row);
        }
    }

    bLoadResourceTextureData = true;
}

bool UGameDataManager::GetResourceTextureData(EResourceTextureType Type, FResourceTextureData& Out) const
{
    if (!bLoadResourceTextureData)
        return false;

    if (const FResourceTextureData* Found = ResourceTextureDataCache.Find(Type))
    {
        Out = *Found;
        return true;
    }

    PRINTLOG(TEXT("DataGetFail : ResourceTextureData Type %s"), *UEnum::GetValueAsString(Type));
    return false;
}

UTexture2D* UGameDataManager::GetTexture(EResourceTextureType Type) const
{
    if (!bLoadResourceTextureData)
        return nullptr;

    if (const FResourceTextureData* Found = ResourceTextureDataCache.Find(Type))
    {
        // TSoftObjectPtr를 동기적으로 로드
        return Found->Texture.LoadSynchronous();
    }

    return nullptr;
}
#pragma endregion RESOURCE_TEXTURE_DATA

#pragma region LEVEL_DATA
void UGameDataManager::Clear_LevelData()
{
    LevelDataCache.Reset();
    bLoadLevelData = false;
}

void UGameDataManager::LoadData_LevelData()
{
    LevelDataCache.Reset();
    bLoadLevelData = false;

    UDataTable* TableObj = LevelDataTable.LoadSynchronous();
    if (!TableObj)
    {
        PRINTLOG(TEXT("Load failed: %s"), *LevelDataTable.ToString());
        return;
    }

    static const FString ContextString(TEXT("LevelDataTable"));
    for (const FName& RowName : TableObj->GetRowNames())
    {
        if (const FLevelData* Row = TableObj->FindRow<FLevelData>(RowName, ContextString, true))
        {
            LevelDataCache.Add(Row->Index, *Row);
        }
    }

    bLoadLevelData = true;
}

bool UGameDataManager::GetLevelData(int32 Step, int32 Level, FLevelData& Out) const
{
    if (!bLoadLevelData)
        return false;

    for (const auto& Pair : LevelDataCache)
    {
        if (Pair.Value.Step == Step && Pair.Value.Level == Level)
        {
            Out = Pair.Value;
            return true;
        }
    }

    PRINTLOG(TEXT("DataGetFail : LevelData Step %d, Level %d"), Step, Level);
    return false;
}
#pragma endregion LEVEL_DATA

#pragma region LISTEN_DATA
void UGameDataManager::Clear_ListenData()
{
    ListenDataCache.Reset();
    bLoadListenData = false;
}

void UGameDataManager::LoadData_ListenData()
{
    ListenDataCache.Reset();
    bLoadListenData = false;

    UDataTable* TableObj = ListenDataTable.LoadSynchronous();
    if (!TableObj)
    {
        PRINTLOG(TEXT("Load failed: %s"), *ListenDataTable.ToString());
        return;
    }

    static const FString ContextString(TEXT("ListenDataTable"));
    for (const FName& RowName : TableObj->GetRowNames())
    {
        if (const FListenData* Row = TableObj->FindRow<FListenData>(RowName, ContextString, true))
        {
            ListenDataCache.Add(Row->Index, *Row);
        }
    }

    bLoadListenData = true;
}

bool UGameDataManager::GetListenData(int32 Index, FListenData& Out) const
{
    if (!bLoadListenData)
        return false;

    if (const FListenData* Found = ListenDataCache.Find(Index))
    {
        Out = *Found;
        return true;
    }

    PRINTLOG(TEXT("DataGetFail : ListenData Index %d"), Index);
    return false;
}

TArray<FListenData> UGameDataManager::GetListenDataByStepAndLevel(int32 Step, int32 Level) const
{
    TArray<FListenData> Result;
    
    if (!bLoadListenData)
        return Result;

    for (const auto& Pair : ListenDataCache)
    {
        if (Pair.Value.Step == Step && Pair.Value.Level == Level)
        {
            Result.Add(Pair.Value);
        }
    }

    return Result;
}

TArray<FListenData> UGameDataManager::GetListenDataByCategory(const FString& Category) const
{
    TArray<FListenData> Result;
    
    if (!bLoadListenData)
        return Result;

    for (const auto& Pair : ListenDataCache)
    {
        if (Pair.Value.Category.Equals(Category))
        {
            Result.Add(Pair.Value);
        }
    }

    return Result;
}
#pragma endregion LISTEN_DATA

#pragma region READ_DATA
void UGameDataManager::Clear_ReadData()
{
    ReadDataCache.Reset();
    bLoadReadData = false;
}

void UGameDataManager::LoadData_ReadData()
{
    ReadDataCache.Reset();
    bLoadReadData = false;

    UDataTable* TableObj = ReadDataTable.LoadSynchronous();
    if (!TableObj)
    {
        PRINTLOG(TEXT("Load failed: %s"), *ReadDataTable.ToString());
        return;
    }

    static const FString ContextString(TEXT("ReadDataTable"));
    for (const FName& RowName : TableObj->GetRowNames())
    {
        if (const FReadData* Row = TableObj->FindRow<FReadData>(RowName, ContextString, true))
        {
            ReadDataCache.Add(Row->Index, *Row);
        }
    }

    bLoadReadData = true;
}

bool UGameDataManager::GetReadData(int32 Index, FReadData& Out) const
{
    if (!bLoadReadData)
        return false;

    if (const FReadData* Found = ReadDataCache.Find(Index))
    {
        Out = *Found;
        return true;
    }

    PRINTLOG(TEXT("DataGetFail : ReadData Index %d"), Index);
    return false;
}

TArray<FReadData> UGameDataManager::GetReadDataByLevel(int32 Level) const
{
    TArray<FReadData> Result;
    
    if (!bLoadReadData)
        return Result;

    for (const auto& Pair : ReadDataCache)
    {
        if (Pair.Value.Level == Level)
        {
            Result.Add(Pair.Value);
        }
    }

    return Result;
}
#pragma endregion READ_DATA

#pragma region WORD_DATA
void UGameDataManager::Clear_WordStudyData()
{
    WordStudyDataCache.Reset();
    bLoadWordStudyData = false;
}

void UGameDataManager::LoadData_WordStudyData()
{
    WordStudyDataCache.Reset();
    bLoadWordStudyData = false;

    UDataTable* TableObj = WordStudyDataTable.LoadSynchronous();
    if (!TableObj)
    {
        PRINTLOG(TEXT("Load failed: %s"), *WordStudyDataTable.ToString());
        return;
    }

    static const FString ContextString(TEXT("WordStudyDataTable"));
    for (const FName& RowName : TableObj->GetRowNames())
    {
        if (const FWordStudyData* Row = TableObj->FindRow<FWordStudyData>(RowName, ContextString, true))
        {
            WordStudyDataCache.Add(Row->Index, *Row);
        }
    }

    bLoadWordStudyData = true;
}

bool UGameDataManager::GetWordStudyData(int32 Index, FWordStudyData& Out) const
{
    if (!bLoadWordStudyData)
        return false;

    if (const FWordStudyData* Found = WordStudyDataCache.Find(Index))
    {
        Out = *Found;
        return true;
    }

    PRINTLOG(TEXT("DataGetFail : WordData Index %d"), Index);
    return false;
}

TArray<FWordStudyData> UGameDataManager::GetAllWordStudyData() const
{
    TArray<FWordStudyData> Result;
    
    if (!bLoadWordStudyData)
        return Result;

    for (const auto& Pair : WordStudyDataCache)
    {
        Result.Add(Pair.Value);
    }

    return Result;
}
#pragma endregion WORD_DATA
