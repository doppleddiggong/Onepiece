// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractableComponent.generated.h"

/**
 * @brief 상호작용 타입 정의
 */
UENUM(BlueprintType)
enum class EInteractionType : uint8
{
	None		UMETA(DisplayName = "None"),
	PickUp		UMETA(DisplayName = "Pick Up"),		// 집어올리기
	Button		UMETA(DisplayName = "Button"),			// 버튼 누르기
};

/**
 * @brief 상호작용 델리게이트
 * @param Interactor 상호작용을 시도한 액터
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionTriggered, AActor*, Interactor);

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
	float DetectionDistance = 150.0f;

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

public:
	// Interaction
	UPROPERTY(ReplicatedUsing=OnRep_HoldingOwner)
	AActor* HoldingOwner;

	UFUNCTION()
	void OnRep_HoldingOwner();
	
	// 현재 들고 있는 상태인지 확인
	// BlueprintPure : 값만 반환, 상태 변경 없을 때 사용
	UFUNCTION(BlueprintPure, Category = "Interaction")
	FORCEINLINE bool IsPickedUp() const {return bIsPickedUp;}
	
	// 물체 집어올림
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void PickUp();

	UFUNCTION(Server, Reliable)
	void Server_PickUp();
	
	// 물체 놓음
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void Drop();

	UFUNCTION(Server, Reliable)
	void Server_Drop();

	// 디버그 정보 표시 (타겟팅 중일 때)
	UFUNCTION(BlueprintCallable, Category = "Debug")
	void ShowDebugInfo(AActor* ViewerActor);

	/** 감지 거리 변경 (런타임) */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetDetectionDistance(float NewDistance);

	// ========================================
	// 범용 상호작용
	// ========================================

	/** 상호작용 트리거 (델리게이트 발생) */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void TriggerInteraction(AActor* Interactor);

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	bool bIsPickedUp = false;

	bool bOriginalSimulatePhysics = false;
	TEnumAsByte<ECollisionEnabled::Type> OriginalCollisionType;

protected:
	// Owner Actor의 PrimitiveComponent 찾기
	// UPrimitiveComponent : 물리/충돌/렌더링을 가진 컴포넌트
	//						 USceneComponent의 하위에 존재
	// 컴포넌트가 붙는 액터의 Mesh를 가져오는 역할
	UPrimitiveComponent* GetOwnerPrimitiveComponent() const;
};
