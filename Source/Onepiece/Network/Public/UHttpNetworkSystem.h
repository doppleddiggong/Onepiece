// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/// @file UHttpNetworkSystem.h
/// @brief HTTP 기반 음성·GPT 파이프라인 요청을 담당하는 서브시스템을 선언합니다.
#pragma once

#include "CoreMinimal.h"
#include "NetworkData.h"
#include "ENetworkLogType.h"
#include "Macro.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UHttpNetworkSystem.generated.h"

UCLASS()
class ONEPIECE_API UHttpNetworkSystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    DEFINE_SUBSYSTEM_GETTER_INLINE(UHttpNetworkSystem);

    /// @brief HTTP 모듈 초기화와 대기 카운터를 설정합니다.
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    /// @brief 등록된 델리게이트를 정리하고 서브시스템을 종료합니다.
    virtual void Deinitialize() override;

    /// @brief 서버 상태 확인을 위한 /health 요청을 전송합니다.
    /// @param InDelegate [in] 응답 수신 시 호출될 델리게이트입니다.
    void RequestHealth(FResponseHealthDelegate InDelegate);

    /// @brief 음성 파일과 공간 컨텍스트를 함께 업로드해 ASK 파이프라인을 실행합니다.
    /// @param FilePath [in] 업로드할 음성 파일 경로입니다.
    /// @param Context [in] 위치 및 시선 정보를 담은 GPT 컨텍스트입니다.
    /// @param InDelegate [in] 통합 응답 데이터를 전달받을 델리게이트입니다.
    void RequestASK(const FString& FilePath, const FGPTContext& Context, FResponseAskDelegate InDelegate);
    
private:
    /// @brief 공통 네트워크 로그 메시지를 출력합니다.
    static void LogNetwork(ENetworkLogType InLogType, const FString& URL, const FString& Body = "");

    /// @brief 요청 타입에 맞는 로그 접두사를 반환합니다.
    static const TCHAR* GetLogPrefix(ENetworkLogType InLogType);

    /// @brief 네트워크 처리 대기 카운트를 갱신하고 UI에 통보합니다.
    void AddNetworkWaitCount(int Value);

private:
    /// @brief 처리 중인 요청 수를 집계하는 카운터입니다.
    int NetworkWaitCount = 0;
};
