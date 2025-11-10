// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

/**
 * @file UFlySystem.h
 * @brief UFlySystem 클래스를 선언합니다.
 */
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UFlySystem.generated.h"

DECLARE_DYNAMIC_DELEGATE(FEndCallback);

/**
 * @brief 캐릭터의 상하 이동과 비행 상태를 제어하는 시스템.
 *
 * 상승/하강 입력, 하강 가속, 마찰 계수를 포함한 전반적인 비행 수명을 관리한다.
 */
UCLASS(Blueprintable, BlueprintType, ClassGroup=(Dopple), meta=(BlueprintSpawnableComponent))
class ONEPIECE_API UFlySystem : public UActorComponent
{
	GENERATED_BODY()

public:
	/** @brief 비행 파라미터의 기본값을 구성한다. */
	UFlySystem();

protected:
	/** @brief 소유 컴포넌트 및 입력 상태를 초기화한다. */
	virtual void BeginPlay() override;

public:
	/**
	 * @brief 매 프레임 비행 상태를 업데이트한다.
	 * @param DeltaTime 틱 간격(초).
	 * @param TickType 틱 호출 유형.
	 * @param ThisTickFunction 내부 틱 함수 포인터.
	 */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;


	/**
	 * @brief 비행 시스템을 초기화하고 종료 콜백을 등록한다.
	 * @param InOwner 비행을 제어할 게임 캐릭터.
	 * @param InCallback 비행이 종료될 때 실행할 델리게이트.
	 */
	UFUNCTION(BlueprintCallable, Category="Fly")
	void InitSystem(class APlayerActor* InOwner, FEndCallback InCallback);

	/**
	 * @brief 상승/하강 입력이 눌렸을 때 상태를 갱신한다.
	 * @param bIsUp true면 상승, false면 하강 입력을 의미한다.
	 */
	UFUNCTION(BlueprintCallable, Category="Fly")
	void OnAltitudePress(const bool bIsUp);
	/** @brief 상승/하강 입력이 해제되었을 때 호출된다. */
	UFUNCTION(BlueprintCallable, Category="Fly")
	void OnAltitudeRelease();

	
	/** @brief 점프 입력을 처리해 초기 가속을 부여한다. */
	UFUNCTION(BlueprintCallable, Category="Fly")
	void OnJump();
	/**
	 * @brief 착지 시 호출되어 상태를 정리한다.
	 * @param Hit 착지 충돌 정보.
	 */
	UFUNCTION(BlueprintCallable, Category="Fly")
	void OnLand(const FHitResult& Hit);

	/** @brief 상승(Upstream) 모드를 활성화한다. */
	UFUNCTION(BlueprintCallable, Category="Fly")
	void ActivateUpstream();
	/** @brief 하강(Downstream) 모드를 활성화한다. */
    UFUNCTION(BlueprintCallable, Category="Fly")
    void ActivateDownstream();

private:
	/**
	 * @brief 상승 중 틱 로직을 처리한다.
	 * @param DeltaTime 틱 간격(초).
	 */
    void UpstreamTick(const float DeltaTime);
	/**
	 * @brief 하강 중 틱 로직을 처리한다.
	 * @param DeltaTime 틱 간격(초).
	 */
    void DownstreamTick(const float DeltaTime);
	/**
	 * @brief 상승/하강 입력에 따른 목표 속도를 계산한다.
	 * @param DeltaTime 틱 간격(초).
	 * @return 입력에 기반한 속도 변화 벡터.
	 */
    FVector AltitudeTick(const float DeltaTime);

    // FVector HandleFlyDeceleration(float DeltaTime);
	
public:

	/** @brief 초기화된 소유 캐릭터. */
	UPROPERTY(Transient, BlueprintReadOnly, Category="Fly")
	TObjectPtr<class APlayerActor> Owner;
	/** @brief 비행에 사용되는 이동 컴포넌트. */
	UPROPERTY(Transient, BlueprintReadOnly, Category="Fly")
	TObjectPtr<class UCharacterMovementComponent> MoveComp;
	
	/** @brief 상승 모드가 활성화되었는지 여부. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Fly")
	bool bIsUpstream = false;
	/** @brief 상승 모드가 유지되는 시간(초). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fly")
	float UpstreamDuration = 1.5f;
	/** @brief 상승 모드에서 목표 상승 높이(cm). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fly")
	float UpstreamHeight = 1500.0f;
	/** @brief 비행 중 허용되는 최대 고도(cm). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fly")
	float MaxFlightHeight = 3000.0f;

	/** @brief 하강 모드가 활성화되었는지 여부. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Fly")
	bool bIsDownstream = false;
	/** @brief 하강 모드 지속 시간(초). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fly")
	float DownstreamDuration = 1.5f;


	/** @brief 목표 지점과의 거리 오차 허용값(cm). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Fly")
	float AlmostDist = 50.0f;

	/** @brief 현재 비행 속도(cm/s). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Fly|Stat")	
	float CurrentFlySpeed = 0.0f;
	/** @brief 최소 비행 속도(cm/s). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Fly|Stat")	
	float FlySpeedMin = 300.0f;
	/** @brief 최대 비행 속도(cm/s). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Fly|Stat")	
	float FlySpeedMax = 1500.0f;
	/** @brief 초당 가속도(cm/s^2). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Fly|Stat")	
	float Acceleration = 1000.0f; // per second
	/** @brief 현재 고도 입력이 활성 상태인지 여부. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Fly|Stat")	
	bool bAltitudeInput = false;
	/** @brief true면 상승, false면 하강 입력을 의미한다. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Fly|Stat")
    bool bIsAltitudeUp = true;

    /** @brief 하강 시 적용할 Z축 감속 값(cm/s^2). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fly|Braking")
    float BrakingDecelerationZ = 2048.0f;
    /** @brief 하강 시 사용할 마찰 계수. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fly|Braking")
    float BrakingFriction = 2.0f;

private:
	/** @brief 비행 종료 시 통지할 콜백. */
	UPROPERTY()
	FEndCallback Callback;

	/** @brief 연속 점프 횟수. */
	int JumpCount = 0;
	/** @brief 상승/하강 경과 시간(초). */
	float ElapsedTime = 0;

	/** @brief 상승 시작 위치. */
	FVector StartLocation;
	/** @brief 상승 종료 목표 위치. */
	FVector EndLocation;
};
