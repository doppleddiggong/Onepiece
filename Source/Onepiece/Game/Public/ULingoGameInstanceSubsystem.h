// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Macro.h"
#include "NetworkData.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ULingoGameInstanceSubsystem.generated.h"

/**
 * @brief K-Lingo 프로젝트 전역에서 유지되는 Subsystem
 *        - GameInstance처럼 씬 전환(OpenLevel)에도 유지됨
 *        - 로그인 정보(UserInfo), 유저 설정 등 중요 데이터 저장
 */
UCLASS()
class ONEPIECE_API ULingoGameInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	DEFINE_SUBSYSTEM_GETTER_INLINE(ULingoGameInstanceSubsystem);
	
	/* -----------------------------------------------------------
	 * UserInfo 관리 (씬 전환에도 유지되는 값)
	 * ----------------------------------------------------------- */

	/** 로그인 이후 UserInfo 설정 */
	void SetUserInfo(const FResponseUserMe& InInfo);

	/** UserInfo 반환 */
	const FResponseUserMe& GetUserInfo() const { return UserInfo; }

private:
	/** 사용자 정보 (replication 없음 – 클라이언트 로컬 전용 저장소) */
	UPROPERTY()
	FResponseUserMe UserInfo;
};