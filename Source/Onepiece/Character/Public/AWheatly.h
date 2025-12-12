// Copyright (c) 2025 Doppleddiggong. All rights reserved. Unauthorized copying, modification, or distribution of this file, via any medium is strictly prohibited. Proprietary and confidential.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AWheatly.generated.h"

/// @brief Wheatly 애니메이션 타입
UENUM(BlueprintType)
enum class EWheatlyAnim : uint8
{
	PowerOn         UMETA(DisplayName = "Power On"),
	Talk            UMETA(DisplayName = "Talk"),
	Reaction_01     UMETA(DisplayName = "Reaction 01"),
	Reaction_02     UMETA(DisplayName = "Reaction 02"),
	Reaction_03     UMETA(DisplayName = "Reaction 03"),
	Reaction_04     UMETA(DisplayName = "Reaction 04"),
	Reaction_05     UMETA(DisplayName = "Reaction 05"),
	Max             UMETA(Hidden)
};

/// @brief Wheatly NPC 액터
/// @details Portal의 Wheatly 캐릭터를 구현한 NPC
/// - 턴 기반 대화 시스템 지원
/// - 애니메이션 재생 및 네트워크 동기화
/// - 상태에 따른 시각적 피드백 (눈 색상 변경)
UCLASS()
class ONEPIECE_API AWheatly : public AActor
{
	GENERATED_BODY()

	//----------------------------------------------------------//
	// Initialization
	//----------------------------------------------------------//

public:
	AWheatly();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//----------------------------------------------------------//
	// Animation System
	//----------------------------------------------------------//

public:
	/// @brief 애니메이션 타입 설정
	/// @param InAnimType [in] 설정할 애니메이션 타입
	FORCEINLINE void SetAnimationType(EWheatlyAnim InAnimType) { AnimType = InAnimType; }

	/// @brief 특정 애니메이션 재생 (서버에서 호출)
	/// @param InAnimType [in] 재생할 애니메이션 타입
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void PlayAnimation(EWheatlyAnim InAnimType);

protected:
	/// @brief 애니메이션 재생 (멀티캐스트 RPC)
	/// @param InAnimType [in] 재생할 애니메이션 타입
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayAnimation(EWheatlyAnim InAnimType);

	/// @brief 몽타주 종료 콜백
	/// @param Montage [in] 종료된 몽타주
	/// @param bInterrupted [in] 중단 여부
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	//----------------------------------------------------------//
	// Speak Stage System
	//----------------------------------------------------------//

public:
	/// @brief SpeakStage 설정 (GameMode에서 호출)
	/// @param InSpeakStage [in] 연결할 SpeakStageActor
	UFUNCTION(BlueprintCallable, Category = "SpeakStage")
	void SetSpeakStage(class ASpeakStageActor* InSpeakStage);

	/// @brief 현재 질문 가져오기
	/// @return 현재 단계의 질문 문자열
	UFUNCTION(BlueprintCallable, Category = "SpeakStage")
	FString GetCurrentQuestion() const;

	//----------------------------------------------------------//
	// Interaction System
	//----------------------------------------------------------//

protected:
	/// @brief 플레이어 상호작용 핸들러
	/// @param InteractingActor [in] 상호작용을 시도하는 액터
	UFUNCTION()
	void OnInteractionTriggered(AActor* InteractingActor);

	/// @brief bIsBusy 상태 변경 시 호출되는 RepNotify 함수
	UFUNCTION()
	void OnRep_bIsBusy();

	//----------------------------------------------------------//
	// Visual System
	//----------------------------------------------------------//

private:
	/// @brief 눈 색상 변경
	/// @param newColor [in] 새로운 색상
	void ChangeEyeColor(FLinearColor newColor);

	//----------------------------------------------------------//
	// Components
	//----------------------------------------------------------//

protected:
	/// @brief 스켈레탈 메시 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> MeshComponent;

	/// @brief 상호작용 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UInteractableComponent> InteractableComp;

	/// @brief 위젯 컴포넌트 (상호작용 UI 표시)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	TObjectPtr<class UWidgetComponent> WidgetComp;

	//----------------------------------------------------------//
	// Materials
	//----------------------------------------------------------//

	/// @brief 베이스 머티리얼 (눈 발광)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Materials")
	TObjectPtr<UMaterialInterface> baseMaterial;

	/// @brief 동적 머티리얼 인스턴스 (런타임 색상 변경)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Category = "Materials", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMaterialInstanceDynamic> dynamicMaterial;

	//----------------------------------------------------------//
	// Animation Data
	//----------------------------------------------------------//

	/// @brief 애니메이션 몽타주 맵
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TMap<EWheatlyAnim, TObjectPtr<class UAnimMontage>> AnimMontage;

	/// @brief 현재 애니메이션 타입
	EWheatlyAnim AnimType;

	//----------------------------------------------------------//
	// Speak Stage Data
	//----------------------------------------------------------//

	/// @brief 연결된 SpeakStage 액터
	UPROPERTY()
	TObjectPtr<class ASpeakStageActor> SpeakStage;

	/// @brief 현재 상호작용 중인 플레이어
	UPROPERTY()
	TObjectPtr<class APlayerActor> targetPlayer;

	/// @brief 심사관 상태 (복제됨)
	UPROPERTY(Transient, ReplicatedUsing = OnRep_bIsBusy)
	bool bIsBusy;

	/// @brief 현재 심사 중인 플레이어 이름 (복제됨)
	UPROPERTY(Transient, Replicated)
	FString busyPlayerName;
};
