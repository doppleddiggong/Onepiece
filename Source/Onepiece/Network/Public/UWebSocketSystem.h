// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file UWebSocketSystem.h
 * @brief 실시간 음성·텍스트 상호작용을 위한 WebSocket 서브시스템을 선언합니다.
 */
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "IWebSocket.h"
#include "Macro.h"
#include "UWebSocketSystem.generated.h"

/**
 * @brief WebSocket 기반 실시간 상호작용을 관리하는 게임 인스턴스 서브시스템입니다.
 *
 * 텍스트 및 오디오 스트림을 위한 지속 연결을 유지하며 연결 상태를 게임 전역에서 공유합니다.
 * 연결 수명은 GameInstance와 동일하며, 네트워크 교체나 종료 시 명시적으로 정리합니다.
 * 사용 위치: 음성 대화/실시간 피드백 시스템에서 공통 연결 관리.
 */
UCLASS(Blueprintable, BlueprintType)
class ONEPIECE_API UWebSocketSystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    DEFINE_SUBSYSTEM_GETTER_INLINE(UWebSocketSystem);

    /**
     * @brief WebSocket 서버와 연결을 수립합니다.
     *
     * 기존 연결이 없을 때만 새 연결을 시도하며 비동기 연결 이벤트를 발생시킵니다.
     * 사이드이펙트: 내부 WebSocket 인스턴스를 생성하고 연결 상태를 변경합니다.
     */
    UFUNCTION(BlueprintCallable, Category = "WebSocket")
    void Connect();

    /**
     * @brief 활성화된 WebSocket 연결을 종료합니다.
     *
     * 연결 종료 후 관련 콜백이 호출되며 내부 상태를 초기화합니다.
     * 사이드이펙트: 네트워크 연결을 끊고 내부 연결 상태를 변경합니다.
     */
    UFUNCTION(BlueprintCallable, Category = "WebSocket")
    void Disconnect();

    /**
     * @brief 연결 상태 유지를 위해 Ping 메시지를 전송합니다.
     *
     * 서버의 연결 유지 정책에 따라 주기적으로 호출될 수 있습니다.
     * 사이드이펙트: 네트워크로 Ping 프레임을 송신합니다.
     */
    UFUNCTION(BlueprintCallable, Category = "WebSocket")
    void SendPing();

    /**
     * @brief 현재 WebSocket 연결 상태를 조회합니다.
     *
     * 로컬 상태만 조회하며 네트워크 요청을 발생시키지 않습니다.
     * @return 연결되어 있으면 true입니다.
     */
    UFUNCTION(BlueprintPure, Category = "WebSocket")
    bool IsConnected() const;
private:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    /// @brief WebSocket 연결 완료 콜백입니다.
    void OnConnected_Native();

    /// @brief 연결 오류 발생 시 호출되는 콜백입니다.
    /// @param InErrorMessage [in] 서버에서 전달된 에러 메시지입니다.
    void OnConnectionError_Native(const FString& InErrorMessage);

    /// @brief 연결 종료 이벤트를 처리합니다.
    /// @param StatusCode [in] WebSocket 상태 코드입니다.
    /// @param Reason [in] 종료 사유입니다.
    /// @param bWasClean [in] 정상 종료 여부입니다.
    void OnClosed_Native(int32 StatusCode, const FString& Reason, bool bWasClean);

    /// @brief 텍스트 메시지를 수신했을 때 호출됩니다.
    /// @param InMessage [in] 서버에서 전달한 JSON/문자열입니다.
    void OnMessage_Native(const FString& InMessage);

    /// @brief 바이너리 메시지를 수신했을 때 호출됩니다.
    /// @param Data [in] 수신 버퍼 시작 주소입니다.
    /// @param Size [in] 버퍼 크기입니다.
    /// @param bIsLastFragment [in] 해당 프레임이 마지막 조각인지 여부입니다.
    void OnBinaryMessage_Native(const void* Data, SIZE_T Size, bool bIsLastFragment);

    /// @brief 활성 WebSocket 인스턴스입니다.
    TSharedPtr<IWebSocket> WebSocket;

    /// @brief 서버에서 오디오 스트림 시작을 예상하는지 여부입니다.
    bool bIsExpectingAudio = false;

private:
    /// @brief WebSocket 관련 공통 로그 메시지를 출력합니다.
    static void LogNetwork(const FString& Message);
};
