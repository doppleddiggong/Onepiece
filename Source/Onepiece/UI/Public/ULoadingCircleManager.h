// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.
#pragma once

/**
 * @file ULoadingCircleManager.h
 * @brief ULoadingCircleManager 클래스를 선언합니다.
 */
#include "CoreMinimal.h"
#include "Macro.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "ULoadingCircleManager.generated.h"

/**
 * @brief 전역 로딩 서클의 표시 여부를 관리하는 LocalPlayerSubsystem입니다.
 * @details 여러 시스템에서 동시에 로딩을 요청할 경우를 대비해 로딩 요청 카운트를 관리하며, 카운트가 1 이상일 때 로딩 서클을 표시합니다.
 */
UCLASS()
class ONEPIECE_API ULoadingCircleManager : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	DEFINE_LOCALPLAYER_SUBSYSTEM_GETTER_INLINE(ULoadingCircleManager);

	ULoadingCircleManager();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/// @brief 로딩 카운트를 증가시킵니다.
	UFUNCTION(BlueprintCallable, Category = "Loading")
	void Show();

	/// @brief 로딩 카운트를 감소시킵니다.
	UFUNCTION(BlueprintCallable, Category = "Loading")
	void Hide();

	/// @brief 현재 로딩 카운트를 반환합니다.
	UFUNCTION(BlueprintPure, Category = "Loading")
	int32 GetLoadingCount() const;

protected:
	/// @brief 현재 월드에 위젯이 없으면 생성하고 Game Viewport에 부착합니다.
	void EnsureWidgetForWorld(UWorld* World);

	/// @brief OnNetworkWaitCount 델리게이트 핸들러
	UFUNCTION()
	void OnNetworkWaitCount(int RequestCount);

protected:
	UPROPERTY()
	TSubclassOf<class ULoadginCircle> CircleWidgetClass;

	UPROPERTY()
	TObjectPtr<class ULoadginCircle> CircleWidget;

	/// @brief Manager가 관리하는 로딩 카운트 (위젯 재생성 시에도 유지됨)
	int32 LoadingCount = 0;
};
