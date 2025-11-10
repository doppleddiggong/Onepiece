// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file IControllable.h
 * @brief UControllable 클래스를 선언합니다.
 */
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IControllable.generated.h"

/**
 * @brief 플레이어 입력을 받아 처리하는 객체를 위한 인터페이스입니다.
 * @details 이 인터페이스를 구현하는 클래스는 이동, 점프, 카메라 회전 등 플레이어 컨트롤러로부터 전달되는 다양한 명령을 처리할 수 있습니다.
 */
UINTERFACE(MinimalAPI)
class UControllable : public UInterface
{
	GENERATED_BODY()
};

class ONEPIECE_API IControllable
{
	GENERATED_BODY()

public:
	virtual void Cmd_Move(const FVector2D& Axis) = 0;		// 이동
	virtual void Cmd_Look(const FVector2D& Axis) = 0;		// 카메라 회전
	virtual void Cmd_Jump() = 0;							// 점프

	virtual void Cmd_Landing() {};							// 랜딩
	
	virtual void Cmd_AltitudeUp() {};						// 고도 상승
	virtual void Cmd_AltitudeDown() {};						// 고도 하강
	virtual void Cmd_AltitudeReleased() {};					// 고도 조절 해제
};
