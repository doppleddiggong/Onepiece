// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractableComponent.generated.h"

/** 상호작용 위젯 블루프린트 경로 */
#define INTERACT_WIDGET_PATH TEXT("/Game/CustomContents/UI/Widgets/WBP_InteractWidget.WBP_InteractWidget_C")

/**
 * @brief 상호작용 타입 정의
 */
UENUM(BlueprintType)
enum class EInteractionType : uint8
{
	None		UMETA(DisplayName = "None"),
	PickUp		UMETA(DisplayName = "Pick Up"),			// 집어올리기
	Button		UMETA(DisplayName = "Button"),			// 버튼 누르기
};

/**
 * @brief 상호작용 델리게이트
 * @param Interactor 상호작용을 시도한 액터
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionTriggered, AActor*, Interactor);

/**
 * @brief 아웃라인 상태 변경 델리게이트
 * @param bShouldShowOutline true면 아웃라인 표시, false면 아웃라인 숨김
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOutlineStateChanged, bool, bShouldShowOutline);

/**
 * @brief 상호작용 가능한 객체의 공통 컴포넌트
 * @details Overlap 기반 근접 감지 + Player LineTrace 확정 방식 사용
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ONEPIECE_API UInteractableComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInteractableComponent();
	
protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void InitDetectionRange();
	
public:
	// ========================================
	// 상호작용 설정
	// ========================================

	/** 상호작용 타입 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	EInteractionType InteractionType = EInteractionType::PickUp;

	/** UI에 표시될 프롬프트 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FString InteractionPrompt = TEXT("Press E to Interact");

	/** 감지 범위 (자동 생성) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	TObjectPtr<class UBoxComponent> DetectionRange;

	/** 상호작용 감지 거리 (cm) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float DetectionDistance = 250.0f;

	/** 디버그 표시 여부 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bShowDetectionDebug = true;

	/** 상호작용 가능 여부 */
	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	bool bCanInteract = true;

	// ========================================
	// 델리게이트
	// ========================================

	/** 상호작용 발생 시 브로드캐스트 */
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnInteractionTriggered OnInteractionTriggered;

	/** 아웃라인 상태 변경 시 브로드캐스트 */
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnOutlineStateChanged OnOutlineStateChanged;

public:
	UFUNCTION()
	void OnRep_HoldingOwner();

	/**
	 * @brief bIsPickedUp 복제 시 호출되는 콜백
	 * @details 픽업/드롭 상태 변화 시 Outline, Widget 등 비주얼 동기화
	 */
	UFUNCTION()
	void OnRep_IsPickedUp();

	// 현재 들고 있는 상태인지 확인
	// BlueprintPure : 값만 반환, 상태 변경 없을 때 사용
	UFUNCTION(BlueprintPure, Category = "Interaction")
	FORCEINLINE bool IsPickedUp() const {return bIsPickedUp;}
	
	// 물체 집어올림
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void PickUp(class AActor* NewHoldingOwner);

	/**
	 * @brief [서버 RPC] 픽업 요청
	 * @details [문제] 기존에는 Validation 없이 클라이언트 제공 NewHoldingOwner를 신뢰
	 *          [해결] WithValidation 추가하여 거리, 소유권, 중복 픽업 검증
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_PickUp(class AActor* NewHoldingOwner);

	// 물체 놓음
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void Drop();

	/**
	 * @brief [서버 RPC] 드롭 요청
	 * @details [문제] 기존에는 Validation 없이 드롭 허용
	 *          [해결] WithValidation 추가하여 실제 픽업 상태 검증
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Drop();

	// 디버그 정보 표시 (타겟팅 중일 때)
	UFUNCTION(BlueprintCallable, Category = "Debug")
	void ShowDebugInfo(AActor* ViewerActor);

	// ========================================
	// 범용 상호작용
	// ========================================

	/** 상호작용 트리거 (델리게이트 발생) */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void TriggerInteraction(AActor* Interactor);

	/**
	 * @brief 아웃라인 상태 설정 (델리게이트 브로드캐스트)
	 * @param bEnabled true면 아웃라인 표시, false면 숨김
	 * @details Widget 표시/숨김 시 호출되며, Actor가 델리게이트를 바인드하여 실제 렌더링 처리
	 */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetOutlineState(bool bEnabled);
	
public:
	// Interaction
	UPROPERTY(ReplicatedUsing=OnRep_HoldingOwner)
	AActor* HoldingOwner;
	/**
	 * @brief 픽업 상태 플래그 (복제됨)
	 * @details [문제] 기존에는 복제되지 않아 ConveyorBelt 등 다른 시스템과 상태 불일치 발생
	 *          [해결] ReplicatedUsing으로 복제하여 모든 클라이언트에서 동일한 상태 유지
	 */
	UPROPERTY(ReplicatedUsing=OnRep_IsPickedUp, BlueprintReadOnly, Category = "Interaction")
	bool bIsPickedUp = false;

	
protected:
	// ========================================
	// Overlap 콜백 (InteractionSystem 등록)
	// ========================================

	UFUNCTION()
	void OnDetectionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	                              bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnDetectionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// ========================================
	// Internal
	// ========================================

	/** 원래 물리 시뮬레이션 상태 (드롭 시 복원용) */
	bool bOriginalSimulatePhysics = false;

	/** 원래 충돌 타입 (드롭 시 복원용) */
	TEnumAsByte<ECollisionEnabled::Type> OriginalCollisionType;

protected:
	// Owner Actor의 PrimitiveComponent 찾기
	// UPrimitiveComponent : 물리/충돌/렌더링을 가진 컴포넌트
	//						 USceneComponent의 하위에 존재
	// 컴포넌트가 붙는 액터의 Mesh를 가져오는 역할
	UPrimitiveComponent* GetOwnerPrimitiveComponent() const;



#pragma region Widget
public:
	void InitWidget(class UWidgetComponent* InWidgetComp);
	void UpdateInteractPrompt(const FString& NewPrompt);

	bool IsWidgetShowEnable(const class ACharacter* Character) const;

	/// @brief 위젯 표시 제어 (외부에서 호출 가능)
	/// @param bVisible [in] 표시 여부
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetWidgetVisibility(bool bVisible);
	
	// ========================================
	// 상호작용 위젯
	// ========================================

private:
	/** 상호작용 위젯 표시 */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void ShowInteractWidget();

	/** 상호작용 위젯 숨김 */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void HideInteractWidget();

	/** 상호작용 위젯 빌보드화 (카메라를 향하도록) */
	void BillboardInteractWidget();

private:
	/** 상호작용 위젯 컴포넌트 (생성자에서 생성 또는 InitSystem으로 외부 주입) */
	UPROPERTY()
	TObjectPtr<class UWidgetComponent> WidgetComp;

#pragma endregion	
};
