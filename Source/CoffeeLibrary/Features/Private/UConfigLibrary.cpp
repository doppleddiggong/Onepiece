// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#include "UConfigLibrary.h"
#include "Misc/ConfigCacheIni.h"

// =====================================================================
// CONSTANTS
// =====================================================================

namespace ConfigLibraryConstants
{
	constexpr const TCHAR* GlobalSection = TEXT("/Script/CoffeeLibrary.Config");
	constexpr const TCHAR* UserSection = TEXT("/Script/CoffeeLibrary.UserConfig");
}

// =====================================================================
// INTEGER OPERATIONS - GLOBAL
// =====================================================================

int32 UConfigLibrary::GetInt(const FString& Key, int32 DefaultValue)
{
	int32 Result = DefaultValue;
	GConfig->GetInt(
		ConfigLibraryConstants::GlobalSection,
		*Key,
		Result,
		GGameUserSettingsIni
	);
	return Result;
}

void UConfigLibrary::SetInt(const FString& Key, int32 Value, bool bAutoSave)
{
	GConfig->SetInt(
		ConfigLibraryConstants::GlobalSection,
		*Key,
		Value,
		GGameUserSettingsIni
	);

	if (bAutoSave)
	{
		Save();
	}
}

// =====================================================================
// FLOAT OPERATIONS - GLOBAL
// =====================================================================

float UConfigLibrary::GetFloat(const FString& Key, float DefaultValue)
{
	float Result = DefaultValue;
	GConfig->GetFloat(
		ConfigLibraryConstants::GlobalSection,
		*Key,
		Result,
		GGameUserSettingsIni
	);
	return Result;
}

void UConfigLibrary::SetFloat(const FString& Key, float Value, bool bAutoSave)
{
	GConfig->SetFloat(
		ConfigLibraryConstants::GlobalSection,
		*Key,
		Value,
		GGameUserSettingsIni
	);

	if (bAutoSave)
	{
		Save();
	}
}

// =====================================================================
// STRING OPERATIONS - GLOBAL
// =====================================================================

FString UConfigLibrary::GetString(const FString& Key, const FString& DefaultValue)
{
	FString Result = DefaultValue;
	GConfig->GetString(
		ConfigLibraryConstants::GlobalSection,
		*Key,
		Result,
		GGameUserSettingsIni
	);
	return Result;
}

void UConfigLibrary::SetString(const FString& Key, const FString& Value, bool bAutoSave)
{
	GConfig->SetString(
		ConfigLibraryConstants::GlobalSection,
		*Key,
		*Value,
		GGameUserSettingsIni
	);

	if (bAutoSave)
	{
		Save();
	}
}

// =====================================================================
// BOOLEAN OPERATIONS - GLOBAL
// =====================================================================

bool UConfigLibrary::GetBool(const FString& Key, bool bDefaultValue)
{
	return GetInt(Key, bDefaultValue ? 1 : 0) != 0;
}

void UConfigLibrary::SetBool(const FString& Key, bool bValue, bool bAutoSave)
{
	SetInt(Key, bValue ? 1 : 0, bAutoSave);
}

// =====================================================================
// INTEGER OPERATIONS - USER-SPECIFIC
// =====================================================================

int32 UConfigLibrary::GetUserInt(int32 UserId, const FString& Key, int32 DefaultValue)
{
	if (UserId <= 0)
	{
		return DefaultValue;
	}

	const FString UserKey = MakeUserKey(UserId, Key);
	int32 Result = DefaultValue;
	GConfig->GetInt(
		ConfigLibraryConstants::UserSection,
		*UserKey,
		Result,
		GGameUserSettingsIni
	);
	return Result;
}

void UConfigLibrary::SetUserInt(int32 UserId, const FString& Key, int32 Value, bool bAutoSave)
{
	if (UserId <= 0)
	{
		return;
	}

	const FString UserKey = MakeUserKey(UserId, Key);
	GConfig->SetInt(
		ConfigLibraryConstants::UserSection,
		*UserKey,
		Value,
		GGameUserSettingsIni
	);

	if (bAutoSave)
	{
		Save();
	}
}

// =====================================================================
// FLOAT OPERATIONS - USER-SPECIFIC
// =====================================================================

float UConfigLibrary::GetUserFloat(int32 UserId, const FString& Key, float DefaultValue)
{
	if (UserId <= 0)
	{
		return DefaultValue;
	}

	const FString UserKey = MakeUserKey(UserId, Key);
	float Result = DefaultValue;
	GConfig->GetFloat(
		ConfigLibraryConstants::UserSection,
		*UserKey,
		Result,
		GGameUserSettingsIni
	);
	return Result;
}

void UConfigLibrary::SetUserFloat(int32 UserId, const FString& Key, float Value, bool bAutoSave)
{
	if (UserId <= 0)
	{
		return;
	}

	const FString UserKey = MakeUserKey(UserId, Key);
	GConfig->SetFloat(
		ConfigLibraryConstants::UserSection,
		*UserKey,
		Value,
		GGameUserSettingsIni
	);

	if (bAutoSave)
	{
		Save();
	}
}

// =====================================================================
// STRING OPERATIONS - USER-SPECIFIC
// =====================================================================

FString UConfigLibrary::GetUserString(int32 UserId, const FString& Key, const FString& DefaultValue)
{
	if (UserId <= 0)
	{
		return DefaultValue;
	}

	const FString UserKey = MakeUserKey(UserId, Key);
	FString Result = DefaultValue;
	GConfig->GetString(
		ConfigLibraryConstants::UserSection,
		*UserKey,
		Result,
		GGameUserSettingsIni
	);
	return Result;
}

void UConfigLibrary::SetUserString(int32 UserId, const FString& Key, const FString& Value, bool bAutoSave)
{
	if (UserId <= 0)
	{
		return;
	}

	const FString UserKey = MakeUserKey(UserId, Key);
	GConfig->SetString(
		ConfigLibraryConstants::UserSection,
		*UserKey,
		*Value,
		GGameUserSettingsIni
	);

	if (bAutoSave)
	{
		Save();
	}
}

// =====================================================================
// BOOLEAN OPERATIONS - USER-SPECIFIC
// =====================================================================

bool UConfigLibrary::GetUserBool(int32 UserId, const FString& Key, bool bDefaultValue)
{
	return GetUserInt(UserId, Key, bDefaultValue ? 1 : 0) != 0;
}

void UConfigLibrary::SetUserBool(int32 UserId, const FString& Key, bool bValue, bool bAutoSave)
{
	SetUserInt(UserId, Key, bValue ? 1 : 0, bAutoSave);
}

// =====================================================================
// JSON OPERATIONS - USER-SPECIFIC
// =====================================================================

void UConfigLibrary::SetUserJson(int32 UserId, const FString& Key, const FString& JsonData, bool bAutoSave)
{
	// JSON은 내부적으로 String으로 저장
	SetUserString(UserId, Key, JsonData, bAutoSave);
}

FString UConfigLibrary::GetUserJson(int32 UserId, const FString& Key, const FString& DefaultJson)
{
	return GetUserString(UserId, Key, DefaultJson);
}

// =====================================================================
// KEY MANAGEMENT
// =====================================================================

bool UConfigLibrary::HasKey(const FString& Key)
{
	FString Temp;
	return GConfig->GetString(
		ConfigLibraryConstants::GlobalSection,
		*Key,
		Temp,
		GGameUserSettingsIni
	);
}

bool UConfigLibrary::HasUserKey(int32 UserId, const FString& Key)
{
	if (UserId <= 0)
	{
		return false;
	}

	const FString UserKey = MakeUserKey(UserId, Key);
	FString Temp;
	return GConfig->GetString(
		ConfigLibraryConstants::UserSection,
		*UserKey,
		Temp,
		GGameUserSettingsIni
	);
}

void UConfigLibrary::DeleteKey(const FString& Key, bool bAutoSave)
{
	GConfig->RemoveKey(
		ConfigLibraryConstants::GlobalSection,
		*Key,
		GGameUserSettingsIni
	);

	if (bAutoSave)
	{
		Save();
	}
}

void UConfigLibrary::DeleteUserKey(int32 UserId, const FString& Key, bool bAutoSave)
{
	if (UserId <= 0)
	{
		return;
	}

	const FString UserKey = MakeUserKey(UserId, Key);
	GConfig->RemoveKey(
		ConfigLibraryConstants::UserSection,
		*UserKey,
		GGameUserSettingsIni
	);

	if (bAutoSave)
	{
		Save();
	}
}

void UConfigLibrary::DeleteAll(bool bAutoSave)
{
	GConfig->EmptySection(
		ConfigLibraryConstants::GlobalSection,
		GGameUserSettingsIni
	);

	if (bAutoSave)
	{
		Save();
	}
}

void UConfigLibrary::DeleteAllUserData(int32 UserId, bool bAutoSave)
{
	if (UserId <= 0)
	{
		return;
	}

	// 해당 UserId로 시작하는 모든 키 찾기
	TArray<FString> AllKeys;
	GConfig->GetSection(
		ConfigLibraryConstants::UserSection,
		AllKeys,
		GGameUserSettingsIni
	);

	const FString UserPrefix = FString::Printf(TEXT("User_%d_"), UserId);
	for (const FString& KeyValue : AllKeys)
	{
		// "Key=Value" 형식에서 Key 부분만 추출
		FString Key;
		FString Value;
		if (KeyValue.Split(TEXT("="), &Key, &Value))
		{
			if (Key.StartsWith(UserPrefix))
			{
				GConfig->RemoveKey(
					ConfigLibraryConstants::UserSection,
					*Key,
					GGameUserSettingsIni
				);
			}
		}
	}

	if (bAutoSave)
	{
		Save();
	}
}

void UConfigLibrary::Save()
{
	GConfig->Flush(false, GGameUserSettingsIni);
}

// =====================================================================
// PRIVATE HELPERS
// =====================================================================

FString UConfigLibrary::GetGlobalSection()
{
	return ConfigLibraryConstants::GlobalSection;
}

FString UConfigLibrary::GetUserSection()
{
	return ConfigLibraryConstants::UserSection;
}

FString UConfigLibrary::MakeUserKey(int32 UserId, const FString& Key)
{
	return FString::Printf(TEXT("User_%d_%s"), UserId, *Key);
}
