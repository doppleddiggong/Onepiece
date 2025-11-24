// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file UNetworkBroadcastLibrary.h
 * @brief Blueprint에서 NetworkBroadcastActor를 쉽게 사용하기 위한 Function Library
 */

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EDamageType.h"
#include "UNetworkBroadcastLibrary.generated.h"

/**
 * @brief NetworkBroadcastActor를 Blueprint에서 쉽게 사용하기 위한 Helper 함수들
 */
UCLASS()
class LATTELIBRARY_API UNetworkBroadcastLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * @brief NetworkBroadcastActor 인스턴스 가져오기
	 * @param WorldContextObject World 컨텍스트
	 * @return NetworkBroadcastActor 인스턴스 (없으면 자동 생성)
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Network Events", meta = (WorldContext = "WorldContextObject"))
	static class ANetworkBroadcastActor* GetNetworkBroadcastActor(const UObject* WorldContextObject);
};
