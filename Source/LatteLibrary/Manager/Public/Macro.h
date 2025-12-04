// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file Macro.h
 * @brief YiSan 전반에서 사용하는 공용 인터페이스를 선언합니다.
 */
#pragma once


// 공통 선언-구현 매크로
#define DEFINE_SUBSYSTEM_GETTER_INLINE(ClassName) \
UFUNCTION(BlueprintPure, Category="CoffeeLibrary|Subsystem", meta=(WorldContext="WorldContextObject")) \
static ClassName* Get(UObject* WorldContext) \
{ \
if (!WorldContext) \
return nullptr; \
if (auto* World = WorldContext->GetWorld()) \
if (auto* GI = World->GetGameInstance()) \
return GI->GetSubsystem<ClassName>(); \
return nullptr; \
}


#define DEFINE_LOCALPLAYER_SUBSYSTEM_GETTER_INLINE(ClassName) \
UFUNCTION(BlueprintPure, Category="CoffeeLibrary|Subsystem", meta=(WorldContext="WorldContextObject")) \
static ClassName* Get(UObject* WorldContextObject) \
{ \
	if (!WorldContextObject) \
		return nullptr; \
	if (UWorld* World = WorldContextObject->GetWorld()) \
	{ \
		if (UGameInstance* GameInstance = World->GetGameInstance()) \
		{ \
			if (ULocalPlayer* LocalPlayer = GameInstance->GetFirstGamePlayer()) \
			{ \
				return LocalPlayer->GetSubsystem<ClassName>(); \
			} \
		} \
	} \
	return nullptr; \
}

// Optional: PlayerController를 받아서 해당 LocalPlayer의 서브시스템을 가져오는 매크로도 만들 수 있어요
#define DEFINE_LOCALPLAYER_FROM_PC_SUBSYSTEM_GETTER_INLINE(ClassName) \
UFUNCTION(BlueprintPure, Category="CoffeeLibrary|Subsystem", meta=(WorldContext="WorldContextObject")) \
static ClassName* GetFromPlayerController(UObject* WorldContextObject, APlayerController* PC) \
{ \
	if (!WorldContextObject || !PC) \
		return nullptr; \
	if (ULocalPlayer* LP = Cast<ULocalPlayer>(PC->Player)) \
	{ \
		return LP->GetSubsystem<ClassName>(); \
	} \
	return nullptr; \
}



// 공통 선언 매크로
#define DECLARE_SUBSYSTEM_GETTER(ClassName) \
UFUNCTION(BlueprintPure, Category="CoffeeLibrary|Subsystem", meta=(WorldContext="WorldContextObject")) \
static ClassName* Get(UObject* WorldContext);

// 공통 구현 매크로
#define DEFINE_SUBSYSTEM_GETTER_IMPL(ClassName) \
static ClassName* Get(UObject* WorldContext) \
{ \
	if (!WorldContext) \
		return nullptr; \
	if (auto* World = WorldContext->GetWorld()) \
		if (auto* GI = World->GetGameInstance()) \
			return GI->GetSubsystem<ClassName>(); \
	return nullptr; \
}

#define BIND_DYNAMIC_DELEGATE(DelegateType, Obj, ClassType, FuncName) \
[] (auto* InObj) { \
DelegateType Tmp; \
Tmp.BindUFunction(InObj, GET_FUNCTION_NAME_CHECKED(ClassType, FuncName)); \
return Tmp; \
} (Obj)

#define ENUM_TO_NAME(EnumType, Value) \
(StaticEnum<EnumType>()->GetNameStringByValue(static_cast<int64>(Value)))

#define ENUM_TO_TEXT(EnumType, Value) \
(StaticEnum<EnumType>()->GetDisplayNameTextByValue(static_cast<int64>(Value)))