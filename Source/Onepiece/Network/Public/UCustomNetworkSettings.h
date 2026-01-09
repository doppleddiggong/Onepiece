// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file UCustomNetworkSettings.h
 * @brief 서버 환경별 네트워크 설정을 노출하는 DeveloperSettings를 선언합니다.
 */
#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "UCustomNetworkSettings.generated.h"

/**
 * @brief 서버 환경(개발/스테이징/라이브)을 나타내는 열거형입니다.
 *
 * 빌드/운영 환경에 따라 다른 BaseUrl과 Port를 선택하기 위한 기준값입니다.
 * 설정 값은 config 기반이며 런타임에 네트워크 요청 라우팅 기준으로만 사용됩니다.
 * 사용 위치: 네트워크 설정 선택(예: UCustomNetworkSettings, NetworkConfig).
 */
UENUM(BlueprintType)
enum class EServerMode : uint8
{
    Dev,   ///< 개발 서버 모드입니다.
    Stage, ///< 스테이징 서버 모드입니다.
    Live   ///< 라이브 서버 모드입니다.
};

/**
 * @brief 서버 접속 정보를 담는 설정 구조체입니다.
 *
 * HTTP/WS 요청이 어떤 호스트와 포트로 향하는지 정의합니다.
 * 런타임에서 값을 소유하지 않고 config 로딩 결과를 그대로 사용합니다.
 * 사용 위치: UCustomNetworkSettings의 모드별 네트워크 설정.
 */
USTRUCT(BlueprintType)
struct FServerConfig
{
    GENERATED_BODY()

    /**
     * @brief 서버 Base URL입니다.
     *
     * 스킴 포함(https://) 문자열이며 포트 결합 전의 기본 주소입니다.
     * 소유권/수명: config에 저장된 값이며 런타임에서 읽기 전용으로 사용됩니다.
     * 동기화: 네트워크 복제와 무관합니다.
     */
    UPROPERTY(EditAnywhere, config, Category="Network")
    FString BaseUrl;

    /**
     * @brief 서버 포트 번호입니다.
     *
     * 범위: 0(미지정) 또는 1~65535이며, 443/0은 기본 포트로 취급됩니다.
     * 소유권/수명: config에 저장된 값이며 런타임에서 읽기 전용으로 사용됩니다.
     * 동기화: 네트워크 복제와 무관합니다.
     */
    UPROPERTY(EditAnywhere, config, Category="Network")
    int32 Port = 0;

    /**
     * @brief WebSocket 접속 URL입니다.
     *
     * ws:// 또는 wss:// 형태의 주소를 저장합니다.
     * 소유권/수명: config에 저장된 값이며 런타임에서 읽기 전용으로 사용됩니다.
     * 동기화: 네트워크 복제와 무관합니다.
     */
    UPROPERTY(EditAnywhere, config, Category="Network")
    FString WebSocketUrl;

    /**
     * @brief 엔드포인트를 결합한 전체 URL을 반환합니다.
     *
     * 기본 포트(443) 또는 포트 미지정(0)일 때는 포트 결합을 생략합니다.
     * @param Endpoint [in] 호출할 상대 경로입니다.
     * @return BaseUrl과 포트 설정이 반영된 전체 URL입니다.
     */
    FString GetFullUrl(const FString& Endpoint) const
    {
        if (Port == 443 || Port == 0)
        {
            return FString::Printf(TEXT("%s%s"), *BaseUrl, *Endpoint);
        }

        return FString::Printf(TEXT("%s:%d%s"), *BaseUrl, Port, *Endpoint);
    }
};

/**
 * @brief 서버 환경별 네트워크 설정을 제공하는 DeveloperSettings 클래스입니다.
 *
 * 런타임에서 네트워크 요청의 대상 서버를 결정하기 위한 단일 진입점입니다.
 * command-line 또는 config 값을 읽어 현재 모드를 선택하며, 보관용 상태를 가지지 않습니다.
 * 사용 위치: NetworkConfig::GetFullUrl, WebSocket 연결 초기화 등.
 */
UCLASS(config=MyNetwork, defaultconfig, meta=(DisplayName="Network Selector Setting"))
class ONEPIECE_API UCustomNetworkSettings : public UDeveloperSettings
{
    GENERATED_BODY()

public:
    /**
     * @brief 명령줄 또는 설정으로 선택된 서버 모드를 반환합니다.
     *
     * 서버 전환 정책을 중앙화하기 위한 조회 API이며 내부 상태를 변경하지 않습니다.
     * @return 현재 선택된 서버 모드입니다.
     */
    static EServerMode GetCurrentServerMode();

    /**
     * @brief 기본 서버 모드입니다.
     *
     * config에 저장되는 기본 값이며, 명령줄 옵션이 없을 때만 사용됩니다.
     * 소유권/수명: 설정 객체에 의해 유지되며 런타임에서 읽기 전용으로 사용됩니다.
     * 동기화: 네트워크 복제와 무관합니다.
     */
    UPROPERTY(config, EditAnywhere, Category="Network")
    EServerMode DefaultMode = EServerMode::Dev;

    /**
     * @brief 개발 서버 설정입니다.
     *
     * 개발 환경에서 사용하는 BaseUrl/Port/WebSocketUrl을 묶어둡니다.
     * 소유권/수명: 설정 객체에 의해 유지되며 런타임에서 읽기 전용으로 사용됩니다.
     * 동기화: 네트워크 복제와 무관합니다.
     */
    UPROPERTY(config, EditAnywhere, Category="Network")
    FServerConfig DevConfig;

    /**
     * @brief 스테이징 서버 설정입니다.
     *
     * QA/스테이징 환경에서 사용하는 접속 정보를 보관합니다.
     * 소유권/수명: 설정 객체에 의해 유지되며 런타임에서 읽기 전용으로 사용됩니다.
     * 동기화: 네트워크 복제와 무관합니다.
     */
    UPROPERTY(config, EditAnywhere, Category="Network")
    FServerConfig StageConfig;

    /**
     * @brief 라이브 서버 설정입니다.
     *
     * 운영 환경에서 사용하는 접속 정보를 보관합니다.
     * 소유권/수명: 설정 객체에 의해 유지되며 런타임에서 읽기 전용으로 사용됩니다.
     * 동기화: 네트워크 복제와 무관합니다.
     */
    UPROPERTY(config, EditAnywhere, Category="Network")
    FServerConfig LiveConfig;

    /**
     * @brief 서버 모드에 해당하는 설정을 반환합니다.
     *
     * 호출자가 직접 모드를 결정해야 하는 상황에서 공통 접근을 제공합니다.
     * @param Mode [in] 조회할 서버 모드입니다.
     * @return 선택한 모드의 서버 구성입니다.
     */
    const FServerConfig& GetConfig(EServerMode Mode) const;
};
